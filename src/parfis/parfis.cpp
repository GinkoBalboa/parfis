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
#include "datastruct.h"

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
                        m_domainMap[domain] = std::unique_ptr<Domain>(
                            new System(domain, m_logger, m_cfgData, m_simData));
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
parfis::Parfis* parfis::Parfis::newParfis()
{
    uint32_t id = Parfis::s_parfisMapId;
    Parfis::s_parfisMap[id] = std::unique_ptr<Parfis>(new Parfis(id));
    Parfis::s_parfisMapId++;
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
    LOG(m_logger, LogMask::Memory, std::string(__FUNCTION__) + 
        " constructor with id = " + std::to_string(m_id) + "\n");
    if (cfgstr == "")
        m_cfgstr = DEFAULT_INITIALIZATION_STRING;
    else 
        m_cfgstr = cfgstr;
    initialize();
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
    for (auto& domain : m_domainMap) {
        retval = domain.second->loadCfgData();
        if (retval != 0)
            break;
    }
    return retval;
}

/**
 * @brief Create command chains
 */
int parfis::Parfis::createCommandChains(const std::string& chainHeadName) 
{
//     m_cmdChainCreate.m_name = chainHeadName;
//     m_cmdChainCreate.m_head = 
//     m_cmdChainCreate.setNext(m_domainMap["system"]->m_cmdCreate);
// 
    return 0;
}

/**
 * @brief Configures the domain with a string and loads the CfgData
 * @param str with the configuration text of the type "key=value"
 * @return Zero on success
 */
int parfis::Parfis::configure(const char* str) 
{
    std::string cstr = Global::removeWhitespace(str);
    size_t fdot = cstr.find('.');
    std::string dname = cstr.substr(0, fdot);
    cstr = cstr.substr(fdot+1, cstr.size() - fdot);
    Domain* dptr = Parfis::getDomain(dname);
    if (dptr == nullptr)
        return -1;
    int retval = 0;
    retval = dptr->configure(cstr);
    retval = dptr->loadCfgData();
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
    else if (retval != 0)
        return 3;
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