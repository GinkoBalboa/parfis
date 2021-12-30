#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include "parfis.h"
#include "global.h"
#include "version.h"
#include "system.h"
#include "config.h"

std::map<uint32_t, std::unique_ptr<parfis::Parfis>> parfis::Parfis::s_parfisMap;

parfis::Domain* parfis::Parfis::getDomain(const std::string& cstr) 
{
    auto dmap = m_domainMap.find(cstr);
    if (dmap == m_domainMap.end())
        return nullptr;
    else 
        return dmap->second.get();
}

std::string parfis::Parfis::getParamValue(const std::string& key)
{
    auto inhvec = Global::getInheritanceVector(key);
    ParamBase* pp = m_domainMap[inhvec[0]].get();
    for(size_t i=1; i<inhvec.size(); i++) {
        pp = pp->m_childMap[inhvec[i]].get();
    }
    return pp->getValueString();
}

void parfis::Parfis::initializeDomains() 
{
    if (m_domainMap.size() != 0)
        return;

    std::string line;
    size_t start = 0;
    size_t end = m_cfgstr.find('\n');
    std::vector<std::string> domainVec;
    while(end != std::string::npos) {
        line = m_cfgstr.substr(start, end - start);
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line.find('#') != std::string::npos)
            line.erase(line.find('#'), line.size() - line.find('#'));
        if (line.size() > 0) {
            // Initialize Domains
            if (line.find("<parfis::Domain>") != std::string::npos) {
                domainVec = Global::getVector(line, '[', ']');
                for (auto& domain : domainVec) {
                    if (domain == "system")
                        m_domainMap["system"] = std::unique_ptr<Domain>(
                            new System("system", m_logger));
                }
            }
            // Configure domains
            else {
                for (auto& domain : domainVec) {
                    std::string sstr = line.substr(0, domain.size() + 1);
                    if ( sstr == domain + "." || sstr == domain + "=" ) {
                        m_domainMap[domain]->initialize(line);
                        break; 
                    }
                }
            }
        }
        start = end + 1;
        end = m_cfgstr.find('\n', start);
    }

}

template<>
void parfis::Param<std::string>::setValueVec(const std::string& valstr) 
{
    m_valueVec = Global::getVector(valstr, '[', ']');
    m_size = m_valueVec.size();
}

template<>
void parfis::Param<std::string>::setRangeVec(const std::string& ranstr) 
{
    m_rangeVec = Global::getVector(ranstr, '(', ')');
}

template<>
void parfis::Param<double>::setValueVec(const std::string& valstr) 
{
    m_valueVec.clear();
    auto valvec = Global::getVector(valstr, '[', ']');
    for (auto& val: valvec)
        m_valueVec.push_back(std::strtold(val.c_str(), nullptr));
    m_size = m_valueVec.size();
}

template<>
void parfis::Param<double>::setRangeVec(const std::string& ranstr) 
{
    auto ranvec = Global::getVector(ranstr, '(', ')');
    for (auto& ran: ranvec)
        m_rangeVec.push_back(std::strtold(ran.c_str(), nullptr));
}

template<class T>
bool parfis::Param<T>::inRange(T value)
{
    return value >= m_rangeVec[0] && value <= m_rangeVec[1];
}

template<>
bool parfis::Param<std::string>::inRange(std::string valstr)
{
    for(auto& str: m_rangeVec)
        if (str == valstr)
            return true;
    return false;
}

template<>
parfis::Param<double>::Param() { m_type = "double"; }
template<>
parfis::Param<std::string>::Param() { m_type = "std::string"; }

template<class S>
void parfis::ParamBase::addChild(const std::string& name) 
{
    m_childMap[name] = std::unique_ptr<ParamBase>(new Param<S>());
    m_childMap[name]->m_name = name;
    m_childMap[name]->m_parent = this;
}

template void parfis::ParamBase::addChild<std::string>(const std::string& name);
template void parfis::ParamBase::addChild<double>(const std::string& name);

bool parfis::ParamBase::inRange(const std::string& valstr)
{
    if (m_type == "int")
        return static_cast<Param<int>*>(this)->inRange(std::strtol(valstr.c_str(), nullptr, 10));
    else if (m_type == "double")
        return static_cast<Param<double>*>(this)->inRange(std::strtod(valstr.c_str(), nullptr));
    else if (m_type == "std::string")
        return static_cast<Param<std::string>*>(this)->inRange(valstr);
    return false;
}

void parfis::ParamBase::setValueVec(ParamBase* ppb, const std::string& valstr)
{
    if (ppb->m_type == "double")
        static_cast<Param<double>*>(ppb)->setValueVec(valstr);
    else if (ppb->m_type == "std::string")
        static_cast<Param<std::string>*>(ppb)->setValueVec(valstr);
}

void parfis::ParamBase::setRangeVec(ParamBase* ppb, const std::string& ranstr)
{
    if (ppb->m_type == "double")
        static_cast<Param<double>*>(ppb)->setRangeVec(ranstr);
    else if (ppb->m_type == "std::string")
        static_cast<Param<std::string>*>(ppb)->setRangeVec(ranstr);
}

std::string parfis::ParamBase::getValueString()
{
    if (m_size == 0)
        return "";
    std::string str;
    if (m_type == "int") {
        if (m_size == 1)
            return std::to_string(static_cast<Param<int>*>(this)->m_valueVec[0]);
        else {
            str += '[';
            for (auto& val : static_cast<Param<int>*>(this)->m_valueVec)
                str += std::to_string(val) + ",";
            str.back() = ']';
            return str;
        }
    }
    else if (m_type == "std::string") {
        if (m_size == 1)
            return static_cast<Param<std::string>*>(this)->m_valueVec[0];
        else {
            str += '[';
            for (auto& val : static_cast<Param<std::string>*>(this)->m_valueVec)
                str += val + ",";
            str.back() = ']';
            return str;
        }
    }
    else if (m_type == "double") {
        if (m_size == 1)
            return Global::to_string(static_cast<Param<double>*>(this)->m_valueVec[0]);
        else {
            str += '[';
            for (auto& val : static_cast<Param<double>*>(this)->m_valueVec)
                str += Global::to_string(val) + ",";
            str.back() = ']';
            return str;
        }
    }
    return "";
}

parfis::Domain::Domain(const std::string& dname, Logger& logger):
    m_logger(&logger)
{
    m_name = dname;
    m_type = "std::string";
    m_parent = this;
}

parfis::Param<std::string>* parfis::Domain::getParent(const std::string& cstr) {
    
    auto inheritvec = Global::getInheritanceVector(cstr);
    ParamBase* pp = this;
    for(size_t i=1; i<inheritvec.size() - 1; i++) {
        pp = pp->m_childMap[inheritvec[i]].get();
    }
    return this;
}

/**
 * @brief Initializes Domain from DEFAULT_INITIALIZATION_STRING
 * @param cstr Initialization string is in the format key=value<type>(range). Value 
 * can be of type array in which case is given as [value1, value2, ...]. 
 * @return Zero on success 
 */
int parfis::Domain::initialize(const std::string& cstr)
{
    std::tuple<std::string, std::string> keyValue = Global::splitKeyValue(cstr);
    std::tuple<std::string, std::string> keyString = Global::splitKeyString(cstr);
    std::string childName = Global::childName(std::get<0>(keyValue));
    Param<std::string>* pp = getParent(std::get<0>(keyValue));
    ParamBase* cp = nullptr;
    if (cstr.find("<parfis::Param>") != std::string::npos) {
        cp = pp;
    }
    else if (cstr.find("<std::string>") != std::string::npos) {
        pp->addChild<std::string>(childName);
        cp = m_childMap[childName].get();
    }
    else if (cstr.find("double") != std::string::npos) {
        pp->addChild<double>(childName);
        cp = m_childMap[childName].get();
    }
    ParamBase::setValueVec(cp, std::get<1>(keyValue));
    ParamBase::setRangeVec(cp, std::get<1>(keyString));
    return 0;
}

/**
 * @brief Configures initialized Domain
 * @param cstr Initialization string is in the format key=value 
 * @return Zero on success 
 */
int parfis::Domain::configure(const std::string& cstr) 
{
    std::tuple<std::string, std::string> keyValue = Global::splitKeyValue(cstr);
    std::string childName = Global::childName(std::get<0>(keyValue));
    Param<std::string>* pp = getParent(std::get<0>(keyValue));
    ParamBase::setValueVec(pp->m_childMap[childName].get(), std::get<1>(keyValue));
    return 0;
}

parfis::Parfis* parfis::Parfis::getParfis(uint32_t id) 
{
    auto it = Parfis::s_parfisMap.find(id);
    if (it == Parfis::s_parfisMap.end())
        return nullptr;
    return it->second.get();
}


std::string parfis::Logger::getLogFileName(uint32_t id, uint32_t cnt) {
    return "./parfisLog_id" + std::to_string(id) + "_cnt" + std::to_string(cnt) + ".log";
}

/**
 * @brief Initializes log string and file name 
 * @param fname Name of the file to write log to
 */
void parfis::Logger::initialize(const std::string& fname) {
    m_fname = fname;
    m_str = "Parfis log file";
    m_str += "\n--------------";
    m_str += "\nCreated on: " + Global::currentDateTime();
    m_str += "\n" + std::string(api::info());
    m_str += "\n--------------";
}

/**
 * @brief Logs strings into Logger::m_str
 * @param mask Logging mask defined by parfis::LogMask
 * @param msg String to log to Logger::m_str
 */
void parfis::Logger::logToStr(LogMask mask, const std::string& msg)
{
    m_str += msg;
}


/**
 * @brief Prints the log string to the defined file
 */
void parfis::Logger::printLogFile()
{
    if (m_fname != "") {
        std::ofstream logFile(m_fname, std::ofstream::app);
        logFile << m_str;
        logFile.close();
        m_str.clear();
    }
}

/**
 * @brief Creates new Parfis object in memory
 * @details Saves a uniquie pointer to the dynamicaly created object in the static 
 * map Parfis::s_parfisMap.
 * @return Pointer to the created Parfis object
 */
parfis::Parfis* parfis::Parfis::newParfis()
{
    uint32_t id = 0;
    for (auto& pfis : Parfis::s_parfisMap)
        id = pfis.first > id ? pfis.first : id;
    if (Parfis::s_parfisMap.size() > 0) id++;
    Parfis::s_parfisMap[id] = std::unique_ptr<Parfis>(new Parfis(id));
    return Parfis::s_parfisMap.at(id).get();
}

/**
 * @brief Constructor with an id
 * @param id Id of the created object
 */
parfis::Parfis::Parfis(uint32_t id, const std::string& cfgstr) :
    m_id(id)
{
    int fcnt = 0;
    std::string fname = Logger::getLogFileName(m_id, 0);
    while(Global::fileExists(fname)) {
        fcnt++;
        fname = Logger::getLogFileName(m_id, fcnt);
    }
    m_logger.initialize(fname);
    LOG(m_logger, LogMask::Memory, std::string("\n") + __FUNCTION__ + 
        " constructor with id = " + std::to_string(m_id));
    if (cfgstr == "")
        m_cfgstr = DEFAULT_INITIALIZATION_STRING;
    else 
        m_cfgstr = cfgstr;
    initialize();
}

/**
 * @brief Initialize all domains
 * @return Zero for success
 */
int parfis::Parfis::initialize() 
{
    initializeDomains();
    return 0;
}


int parfis::Parfis::configure(const char* str) 
{
    std::string cstr = Global::removeWhitespace(str);
    size_t fdot = cstr.find('.');
    std::string dname = cstr.substr(0, fdot);
    cstr = cstr.substr(fdot+1, cstr.size() - fdot);
    Domain* dptr = Parfis::getDomain(dname);
    if (dptr == nullptr)
        return -1;
    dptr->configure(cstr);
    return 0;
}

/**
 * @brief Destroy the parfis::Parfis object
 * @details Print log file
 */
parfis::Parfis::~Parfis() {
    m_logger.printLogFile();
}

/**
 * @brief Returns info about the program settings.
 * @details These settings are compilation definitions and values of the current runtime.
 * @return The info string
 */
PARFIS_EXPORT const char* parfis::api::info()
{
    static std::string str;
    if (sizeof(state_t) == 4)
        str = "parfis::state_t = float";
    else if (sizeof(state_t) == 8)
        str = "parfis::state_t = double";
    else
        str = "parfis::state_t = unknown";
    str += "\nparfis::logLevel = " + std::to_string(Const::logLevel);
    str += "\nparfis::version = " + std::string(Const::version);
    str += "\nparfis::gitTag = " + std::string(Const::gitTag);
    int pfSize = Parfis::s_parfisMap.size();
    str += "\nParfis object count = " + std::to_string(pfSize);
    str += "\nParfis object id = [";
    int i = 0;
    for (auto& pfis: Parfis::s_parfisMap) {
        str += std::to_string(pfis.first);
        if (i != pfSize - 1)
            str += ", ";
        i++;
    }
    str += "]";
    return str.c_str();
}

/**
 * @brief Returns info about the Parfis object.
 * @param id Id of the Parfis object
 * @return The object's info string
 */
PARFIS_EXPORT const char* parfis::api::parfisInfo(uint32_t id)
{
    static std::string APIStaticString;
    
    if (Parfis::s_parfisMap.count(id) == 0) {
        APIStaticString = "Parfis with id = " + std::to_string(id) + " doesn't exist";
    }
    else {
        APIStaticString = "Parfis::m_id = " + std::to_string(Parfis::s_parfisMap.at(id)->m_id);
        APIStaticString += "\nParfis::m_logger.m_fname = " + 
            Parfis::s_parfisMap.at(id)->m_logger.m_fname;
    }

    return APIStaticString.c_str();
}

/**
 * @brief Returns version string
 * @return The version string
 */
PARFIS_EXPORT const char* parfis::api::version()
{
    return VERSION;
}

/**
 * @brief Creates new Parfis object and returns its id.
 * @return Id of the created object
 */
PARFIS_EXPORT uint32_t parfis::api::newParfis()
{
    return Parfis::newParfis()->m_id;
}

/**
 * @brief Configures the Parfis object with the given id.
 * @param id Id of the Parfis object to configure
 * @param str Configuration string
 * @return Zero for success
 */
PARFIS_EXPORT int parfis::api::setConfig(uint32_t id, const char* str)
{
    if (Parfis::getParfis(id) == nullptr) 
        return 1;
    int retval = Parfis::getParfis(id)->configure(str);
    // Domain name not recognized
    if (retval == -1)
        return 2;
    return 0;
}

/**
 * @brief Returns the default configuration hardcoded in DEFAULT_INITIALIZATION_STRING.
 * @return The default configuration 
 */
PARFIS_EXPORT const char* parfis::api::defaultConfiguration()
{
    return DEFAULT_INITIALIZATION_STRING;
}

/**
 * @brief Returns configuration of Parfis object
 * @details The function serializes the Domain objects and its children.
 * @param id Object id
 * @return The serialized configuration string
 */
PARFIS_EXPORT const char* parfis::api::getConfig(uint32_t id)
{
    static std::string APIStaticString;
    std::string str;

    APIStaticString = "";
    std::function<std::string(ParamBase* pb)> addChildString;
    addChildString = [&str, &addChildString](ParamBase* pb)->std::string {
        str += pb->m_name + "=" + pb->getValueString() + "\n";
        if (pb->m_childMap.size() > 0) { 
            for(auto& cpb: pb->m_childMap) {
                str += pb->m_name + ".";
                addChildString(cpb.second.get());
            }
        }
        return "";
    };

    if (Parfis::getParfis(id) == nullptr) 
        return "Given id doesn't exist";
        
    for(auto& dom: Parfis::getParfis(id)->m_domainMap) {
        str = "";
        addChildString(dom.second.get());
        APIStaticString += str;
    }
    return APIStaticString.c_str();
}

/**
 * @brief Returns value for a single configuration parameter of Parfis object.
 * @details The function serializes the single children of the Domain object.
 * @param id Object id
 * @param key The parameter key
 * @return The parameter value as string
 */
PARFIS_EXPORT const char* parfis::api::getConfigParam(uint32_t id, const char* key)
{
    static std::string APIStaticString;
    std::string str;

    APIStaticString = Parfis::getParfis(id)->getParamValue(key);
    return APIStaticString.c_str();
}