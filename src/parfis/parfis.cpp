#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <functional>
#include "parfis.h"
#include "global.h"
#include "version.h"
#include "system.h"
#include "config.h"

std::map<uint32_t, std::unique_ptr<parfis::Parfis>> parfis::Parfis::s_parfisMap;
uint32_t parfis::Parfis::s_parfisMapId = 0;

parfis::Domain* parfis::Parfis::getDomain(const std::string& cstr) 
{
    auto dmap = m_domainMap.find(cstr);
    if (dmap == m_domainMap.end())
        return nullptr;
    else 
        return dmap->second.get();
}

std::string parfis::Parfis::getParamValueString(const std::string& key)
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
    std::vector<std::string> commandVec;
    while(end != std::string::npos) {
        line = m_cfgstr.substr(start, end - start);
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line.find('#') != std::string::npos)
            line.erase(line.find('#'), line.size() - line.find('#'));
        if (line.size() > 0) {
            // Initialize Domains
            if (line.find("<parfis::Domain>") != std::string::npos) {
                m_domainVec = Global::getVector(line, '[', ']');
                for (auto& domain : m_domainVec)
                    m_domainMap[domain] = Domain::generateDomain(
                        domain, m_logger, m_cfgData, m_simData, m_cmdChainMap);
            }
            // Initialize command chains
            else if (line.find("<parfis::CommandChain>") != std::string::npos) {
                commandVec = Global::getVector(line, '[', ']');
                for (auto& cmdName : commandVec) {
                    m_cmdChainMap.insert(
                            {cmdName, std::unique_ptr<CommandChain>(new CommandChain())});    
                    m_cmdChainMap[cmdName]->m_name = cmdName;
                    m_cmdChainMap[cmdName]->m_func = [&]()->int { return 0; };
                    m_cmdChainMap[cmdName]->m_funcName = "";
                }
            }
            // Initialize commands
            else if (line.find("<parfis::Command>") != std::string::npos) {
                std::string cmdChainName = "commandChain";
                for (auto& cmdChain : m_cmdChainMap) {
                    std::string sstr = line.substr(
                            0, cmdChainName.size() + cmdChain.first.size() + 2);
                    if ( sstr == cmdChainName + '.' + cmdChain.first + "=") {
                        commandVec = Global::getVector(line, '[', ']');
                        Command *pcom = cmdChain.second.get();
                        for (auto& cmdName : commandVec) {
                            // Insert into map of commands and connect with chain
                            cmdChain.second->m_cmdMap.insert({
                                cmdName, std::unique_ptr<Command>(new Command(cmdName))});
                            pcom->setNext(*cmdChain.second->m_cmdMap[cmdName].get());
                            pcom = cmdChain.second->m_cmdMap[cmdName].get();
                        }
                    }
                }
            }
            // Everything else is domain configuration
            else {
                for (auto& domain : m_domainVec) {
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

/// @todo Write doxy
parfis::Parfis* parfis::Parfis::getParfis(uint32_t id) 
{
    auto it = Parfis::s_parfisMap.find(id);
    if (it == Parfis::s_parfisMap.end())
        return nullptr;
    return it->second.get();
}

/**
 * @brief Creates new Parfis object in memory
 * @details Saves a uniquie pointer to the dynamicaly created object in the static 
 * map Parfis::s_parfisMap.
 * @return Pointer to the created Parfis object
 */
parfis::Parfis* parfis::Parfis::newParfis(const std::string& cfgStr, uint32_t writeLogFile)
{
    uint32_t id = Parfis::s_parfisMapId;
    Parfis::s_parfisMap[id] = std::unique_ptr<Parfis>(new Parfis(id, cfgStr, writeLogFile));
    Parfis::s_parfisMapId++;
    return Parfis::s_parfisMap[id].get();
}

/**
 * @brief Constructor with an id
 * @param id of the created object
 * @param cfgstr configuration string ("" by default)
 */
parfis::Parfis::Parfis(uint32_t id, const std::string& cfgstr, uint32_t writeLogFile) :
    m_id(id)
{
    int fcnt = 0;
    std::string fname = "";
    if (writeLogFile != 0) {
        Logger::getLogFileName(m_id, 0);
        while(Global::fileExists(fname)) {
            fcnt++;
            fname = Logger::getLogFileName(m_id, fcnt);
        }
    }
    m_logger.initialize(fname);
    
    // If configuration string is given - use that, else use default cfg string
    if (cfgstr != "")
        m_cfgstr = cfgstr;
    else
        m_cfgstr = DEFAULT_INITIALIZATION_STRING;
    initialize();

    LOG(m_logger, LogMask::Info, std::string(__FUNCTION__) + 
        " constructor with id = " + std::to_string(m_id) + "\n");
    std::string msg = Const::multilineSeparator;
    msg += m_cfgstr;
    msg += Const::multilineSeparator;
    LOG(m_logger, LogMask::Info, std::string(__FUNCTION__) + 
        " configuration string:\n" + msg);
}

/**
 * @brief Initialize all domains create command chains
 * @details Creates Data structs and calls loaders for the data that reside inside
 * specific domains.
 * @return Zero for success
 */
int parfis::Parfis::initialize() 
{
    initializeDomains();
    int retval = 0;
    return retval;
}

int parfis::Parfis::loadSimData()
{
    int retval = 0;
    for (auto& domain : m_domainVec) {
        retval = m_domainMap[domain]->loadSimData();
        if (retval != 0)
            break;
    }
    // Set evolution counter
    m_simData.evolveCnt = 0;
    return retval;
}

int parfis::Parfis::loadCfgData()
{
    int retval = 0;
    for (auto& domain : m_domainVec) {
        retval = m_domainMap[domain]->loadCfgData();
        if (retval != 0)
            break;
    }
    std::string msg = Const::multilineSeparator;
    msg += parfis::api::getConfig(m_id);
    msg += Const::multilineSeparator;
    LOG(m_logger, LogMask::Info, std::string(__FUNCTION__) + 
        " configuration string:\n" + msg);
    return retval;
}

/**
 * @brief Run command chain
 * @param chainChainName name of the command chain
 * @return Zero on success
 */
int parfis::Parfis::runCommandChain(const std::string& chainChainName) 
{
    int retval = 0;
    Command* pcom;
    pcom = m_cmdChainMap[chainChainName].get();
    while(pcom != nullptr) {
        retval = pcom->m_func();
        pcom = pcom->getNext();
    }
    if (chainChainName == "evolve")
        m_simData.evolveCnt++;
    return retval;
}

/**
 * @brief Configures the domain with a string and loads the CfgData
 * @param str with the configuration text of the type "key=value"
 * @return Zero on success
 */
int parfis::Parfis::configure(const char* str) 
{
    int retval = 0;
    std::string cstr = Global::removeWhitespace(str);
    size_t fdot = cstr.find('.');
    std::string dname = cstr.substr(0, fdot);
    Domain* dptr = Parfis::getDomain(dname);
    if (dptr == nullptr) {
        if (dname == "commandChain") {
            // This is reconfiguration of commandChain definition
            if (cstr[fdot] == '=') {
                std::vector<std::string> commandVec = Global::getVector(cstr, '[', ']');
                m_cmdChainMap.clear();
                for (auto& cmdName : commandVec) {
                    m_cmdChainMap.insert(
                        {cmdName, std::unique_ptr<CommandChain>(new CommandChain())});    
                    m_cmdChainMap[cmdName]->m_name = cmdName;
                    m_cmdChainMap[cmdName]->m_func = [&]()->int { return 0; };
                    m_cmdChainMap[cmdName]->m_funcName = "";
                }
            }
            else {
                // This is reconfiguration of one commandChain
                for (auto& cmdChain : m_cmdChainMap) {
                    std::string sstr = cstr.substr(0, dname.size() + cmdChain.first.size() + 2);
                    if ( sstr == dname + '.' + cmdChain.first + "=") {
                        std::vector<std::string> commandVec = Global::getVector(cstr, '[', ']');
                        Command *pcom = cmdChain.second.get();
                        cmdChain.second->m_cmdMap.clear();
                        for (auto& cmdName : commandVec) {
                            // Insert into map of commands and connect with chain
                            cmdChain.second->m_cmdMap.insert({
                                cmdName, std::unique_ptr<Command>(new Command(cmdName))});
                            pcom->setNext(*cmdChain.second->m_cmdMap[cmdName].get());
                            pcom = cmdChain.second->m_cmdMap[cmdName].get();
                        }
                    }
                }
            }
        }
        else {
            retval = -1;
        }
    }
    else {
        // This is reconfiguration of a variable or a parameter
        retval = dptr->configure(cstr);
    }
    return retval;
}

/**
 * @brief Destroy the Parfis object.
 * @details Print log file.
 */
parfis::Parfis::~Parfis() {
    m_logger.printLogFile();
}

/**
 * @brief Returns info about the program settings.
 * @details Settings are the compilation definitions and values of the current runtime.
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
    str += "\nparfis::buildConfig = " + std::string(Const::buildConfig);
    str += "\nparfis::gitTag = " + std::string(Const::gitTag);
    int pfSize = int(Parfis::s_parfisMap.size());
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
        APIStaticString = "Parfis::m_id = " + std::to_string(Parfis::s_parfisMap[id]->m_id);
        APIStaticString += "\nParfis::m_logger.m_fname = " + 
            Parfis::s_parfisMap[id]->m_logger.m_fname;
    }

    return APIStaticString.c_str();
}

PARFIS_EXPORT const char* parfis::api::getLogFileName(uint32_t id)
{
    static std::string APIStaticString = Parfis::s_parfisMap[id]->m_logger.m_fname;
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
 * @param cfgStr Configuration string (default is "")
 * @param writeLogFile Turn on/off writing logs to file (default is 0 - off)
 * @return Id of the created object
 */
PARFIS_EXPORT uint32_t parfis::api::newParfis(const char * cfgStr, uint32_t writeLogFile)
{
    return Parfis::newParfis(cfgStr)->m_id;
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
    int retval = 0;
    std::string cfgStr = str;
    size_t start = 0;
    size_t end = cfgStr.find('\n', start);
    if (end == std::string::npos) {
        // One line cfgStr
        retval = Parfis::getParfis(id)->configure(str);
    }
    else {
        // Multi line cfgStr
        std::string line;
        line = cfgStr.substr(start, end - start + 1);
        while(line.size() > 0) {
            retval = Parfis::getParfis(id)->configure(line.c_str());
            start = end + 1;
            end = cfgStr.find('\n', start);
            if (end == std::string::npos)
                line.clear();
            else
                line = cfgStr.substr(start, end - start + 1); 
        }
    }
    // Domain name not recognized
    if (retval == -1)
        return 2;
    else if (retval != 0)
        return 3;
    return 0;
}

PARFIS_EXPORT int parfis::api::loadSimData(uint32_t id)
{
    if (Parfis::getParfis(id) == nullptr) 
        return 1;
    int retval = Parfis::getParfis(id)->loadSimData();
    return retval;
}

PARFIS_EXPORT int parfis::api::loadCfgData(uint32_t id)
{
    if (Parfis::getParfis(id) == nullptr) 
        return 1;
    int retval = Parfis::getParfis(id)->loadCfgData();
    return retval;
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
    std::string str, inhStr;

    std::function<std::string(ParamBase* pb)> addChildString;
    addChildString = [&str, &inhStr, &addChildString](ParamBase* pb)->std::string {
        if (pb->m_childMap.size() > 0) {
            std::string oldInhStr = inhStr;
            if (inhStr == "")
                inhStr += pb->m_name;
            else
                inhStr += "." + pb->m_name;
            str += inhStr + " = " + pb->getValueString(true) + "\n";
            for(auto& child: pb->m_childMap)
                addChildString(child.second.get());
            inhStr = oldInhStr;
        }
        else {
            str += inhStr + "." + pb->m_name + " = " + pb->getValueString(true) + "\n";
        }
        return "";
    };

    if (Parfis::getParfis(id) == nullptr) 
        return "Given id doesn't exist";

    APIStaticString = "domain = [";
    for(auto& domainName: Parfis::getParfis(id)->m_domainVec)
        APIStaticString += domainName + ",";
    APIStaticString.back() = ']';
    APIStaticString += " <parfis::Domain>\n";
    for(auto& domainName: Parfis::getParfis(id)->m_domainVec) {
        str = "";
        addChildString(Parfis::getParfis(id)->m_domainMap[domainName].get());
        APIStaticString += str;
    }
    APIStaticString += "commandChain = [";
    for(auto& cmdChain: Parfis::getParfis(id)->m_cmdChainMap) {
        APIStaticString += cmdChain.first + ",";
    }
    APIStaticString.back() = ']';
    APIStaticString += " <parfis::CommandChain>\n";    
    for(auto& cmdChain: Parfis::getParfis(id)->m_cmdChainMap) {
        APIStaticString += "commandChain." + cmdChain.first + " = [";
        Command* pcom = cmdChain.second->getNext();
        while(pcom != nullptr) {
            APIStaticString += pcom->m_name + ",";
            pcom = pcom->getNext();
        }
        APIStaticString.back() = ']';
        APIStaticString += " <parfis::Command>\n";
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

    APIStaticString = Parfis::getParfis(id)->getParamValueString(key);
    return APIStaticString.c_str();
}

/**
 * @brief Returns pointer to the CfgData of the Parfis object given by id
 * @param id of the Parfis object
 */
PARFIS_EXPORT const parfis::CfgData* parfis::api::getCfgData(uint32_t id)
{
    return &Parfis::getParfis(id)->m_cfgData;
}

/**
 * @brief Loads the PyCfgData
 * @param id of the Parfis object
 */
PARFIS_EXPORT int parfis::api::setPyCfgData(uint32_t id)
{
    return Parfis::getParfis(id)->m_cfgData.setPyCfgData();
}

/**
 * @brief Loads the PySimData
 * @param id of the Parfis object
 */
PARFIS_EXPORT int parfis::api::setPySimData(uint32_t id)
{
    return Parfis::getParfis(id)->m_simData.setPySimData();
}

/**
 * @brief Returns pointer to the PyCfgData of the Parfis object given by id
 * @param id of the Parfis object
 */
PARFIS_EXPORT const parfis::PyCfgData* parfis::api::getPyCfgData(uint32_t id)
{
    return &Parfis::getParfis(id)->m_cfgData.pyCfgData;
}

/**
 * @brief Returns pointer to the PySimData of the Parfis object given by id
 * @param id of the Parfis object
 */
PARFIS_EXPORT const parfis::PySimData* parfis::api::getPySimData(uint32_t id)
{
    return &Parfis::getParfis(id)->m_simData.pySimData;
}

/**
 * @brief Returns pointer to the SimData of the Parfis object given by id
 * @param id of the Parfis object
 */
PARFIS_EXPORT const parfis::SimData* parfis::api::getSimData(uint32_t id)
{
    return &Parfis::getParfis(id)->m_simData;
}

/**
 * @brief Deletes the Parfis object given by the id
 * @param id of the Parfis object
 * @return Zero on success 
 */
PARFIS_EXPORT int parfis::api::deleteParfis(uint32_t id)
{
    if (Parfis::getParfis(id) == nullptr) 
        return 1;
    Parfis::s_parfisMap.erase(id);
    return 0;
}

/**
 * @brief Deletes all Parfis objects
 * @return Zero on success 
 */
PARFIS_EXPORT int parfis::api::deleteAll()
{
    std::vector<uint32_t> keysToErase;
    for (auto& pfis: Parfis::s_parfisMap)
        keysToErase.push_back(pfis.first);
    for (auto idtoerase: keysToErase)
        Parfis::s_parfisMap.erase(idtoerase);
    return 0;
}

/**
 * @brief Get the Parfis vector of ids from s_parfisMap
 * @return vector of uint32_t 
 */
PARFIS_EXPORT const std::vector<uint32_t>& parfis::api::getParfisIdVec()
{
    static std::vector<uint32_t> APIStaticUin32Vec;

    APIStaticUin32Vec.clear();
    for (auto& pfis: Parfis::s_parfisMap) {
        APIStaticUin32Vec.push_back(pfis.first);
    }

    return APIStaticUin32Vec;
}

/**
 * @brief Run command chain given by id an name of the command chain
 * @param id of the Parfis object
 * @param key command chain name
 * @return Zero on success
 */
PARFIS_EXPORT int parfis::api::runCommandChain(uint32_t id, const char* key) 
{
    return Parfis::getParfis(id)->runCommandChain(key);
}

/**\n
 * @brief Expose the custom Global::to_string conversion from double
 * @param num double number to be converted to string
 * @return const char* representation of the number
 */
PARFIS_EXPORT const char* parfis::api::toStringDouble(double num)
{
    static std::string APIStaticString = Global::to_string(num);

    return APIStaticString.c_str();
}

/**
 * @brief Expose the custom Global::to_string conversion from float
 * @param num float number to be converted to string
 * @return const char* representation of the number
 */
PARFIS_EXPORT const char* parfis::api::toStringFloat(float num)
{
    static std::string APIStaticString = Global::to_string(num);

    return APIStaticString.c_str();
}