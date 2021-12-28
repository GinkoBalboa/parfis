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
void parfis::Param<int>::setType() 
{
    m_type = "int";
}


template<>
void parfis::Param<std::string>::setType() 
{
    m_type = "std::string";
}

template<>
void parfis::Param<double>::setType() 
{
    m_type = "double";
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

bool parfis::ParamBase::isDomain()
{
    return m_parent == this;
}

template<class T>
void parfis::Param<T>::addChild(const std::string& name) 
{
    m_childMap[name] = std::unique_ptr<ParamBase>(new Param<T>());
    m_childMap[name]->m_name = name;
    m_childMap[name]->m_parent = this;
    static_cast<Param<T>*>(m_childMap[name].get())->setType();
}

template void parfis::Param<std::string>::addChild(const std::string& name);

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

int parfis::Domain::initialize(const std::string& cstr)
{
    std::tuple<std::string, std::string> keyValue = Global::splitKeyValue(cstr);
    std::tuple<std::string, std::string> keyString = Global::splitKeyString(cstr);
    std::string childName = Global::childName(std::get<0>(keyValue));
    Param<std::string>* pp = getParent(std::get<0>(keyValue));
    if (cstr.find("<parfis::Param>") != std::string::npos) {
        pp->m_valueVec = Global::getVector(std::get<1>(keyValue), '[', ']');
        pp->m_rangeVec = Global::getVector(std::get<1>(keyString), '(', ')');
        pp->setType();
        pp->m_size = m_valueVec.size();
    }
    else if (cstr.find("<std::string>") != std::string::npos) {
        pp->addChild(childName);
        Param<std::string>* cp = static_cast<Param<std::string>*>(m_childMap[childName].get());
        cp->m_valueVec = Global::getVector(std::get<1>(keyValue), '[', ']');
        cp->m_rangeVec = Global::getVector(std::get<1>(keyString), '(', ')');
        cp->setType();
        cp->m_size = cp->m_valueVec.size();
    }
    else if (cstr.find("double") != std::string::npos) {
        pp->addChild(childName);
        Param<double>* cp = static_cast<Param<double>*>(m_childMap[childName].get());
        auto valvec = Global::getVector(std::get<1>(keyValue), '[', ']');
        for (auto& val: valvec)
            cp->m_valueVec.push_back(std::strtold(val.c_str(), nullptr));
        cp->setType();
        cp->m_size = cp->m_valueVec.size();
    }
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
    m_str += "\n" + std::string(ParfisAPI::info());
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
 * @details Saves a pointer to the dynamicaly created object in the static 
 * map Parfis::s_parfisMap.
 * @return parfis::Parfis* Pointer to the created Parfis object
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
        m_cfgstr = DEFAULT_CONFIGURATION;
    else 
        m_cfgstr = cfgstr;
    initialize();
}

/**
 * @brief Initialize all domains
 * @return int Zero for success
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
 * @details These settings are compilation definitions and values
 * of the current runtime.
 * @return const char* The info string
 */
PARFIS_EXPORT const char* parfis::ParfisAPI::info()
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
 * @return const char* The object's info string
 */
PARFIS_EXPORT const char* parfis::ParfisAPI::parfisInfo(uint32_t id)
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
 * @return const char* The version string
 */
PARFIS_EXPORT const char* parfis::ParfisAPI::version()
{
    return VERSION;
}

/**
 * @brief Creates new Parfis object and returns its id.
 * @return uint32_t Id of the created object
 */
PARFIS_EXPORT uint32_t parfis::ParfisAPI::newParfis()
{
    return Parfis::newParfis()->m_id;
}

PARFIS_EXPORT int parfis::ParfisAPI::configure(uint32_t id, const char* str)
{
    if (Parfis::getParfis(id) == nullptr) 
        return 1;
    int retval = Parfis::getParfis(id)->configure(str);
    // Domain name not recognized
    if (retval == -1)
        return 2;
    return 0;
}

PARFIS_EXPORT const char* parfis::ParfisAPI::defaultConfiguration()
{
    return DEFAULT_CONFIGURATION;
}

/**
 * @brief Returns configuration of Parfis object
 * @param id Object id
 * @return const char* The serialized configuration string
 */
PARFIS_EXPORT const char* parfis::ParfisAPI::configuration(uint32_t id)
{
    static std::string APIStaticString;
    std::string str;
    std::string domainName;

    std::function<std::string(ParamBase* pb)> addChildString;
    addChildString = [&str, &addChildString, &domainName](ParamBase* pb)->std::string {
        str += pb->m_name + "=" + pb->getValueString() + "\n";
        if (pb->m_childMap.size() > 0) {
            // str += "[";
            //for(auto& cpb: pb->m_childMap)
            //    str += cpb.first + ",";
            // str[str.size()-1] = ']';
            for(auto& cpb: pb->m_childMap)
                addChildString(cpb.second.get());
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
