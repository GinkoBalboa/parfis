#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include "config.h"
#include "parfis.h"
#include "global.h"
#include "version.h"
#include "system.h"

std::map<uint32_t, std::unique_ptr<parfis::Parfis>> parfis::Parfis::s_parfisMap;

parfis::Domain::Domain(Logger& logger, const std::string& dname):
    m_logger(&logger),
    m_domainName(dname)
{
    // Config::initialize(m_domainName);
}

parfis::Parfis* parfis::Parfis::getParfis(uint32_t id) 
{
    auto it = Parfis::s_parfisMap.find(id);
    if (it == Parfis::s_parfisMap.end())
        return nullptr;
    return it->second.get();
}

parfis::Domain* parfis::Parfis::getDomain(uint32_t id) 
{
    return Parfis::m_domainMap.find(id)->second.get();
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
 * @param mask LogMask
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
parfis::Parfis::Parfis(uint32_t id) :
    m_id(id)
{
    int fcnt = 0;
    std::string fname = Logger::getLogFileName(m_id, 0);
    while(Global::fileExists(fname)) {
        fcnt++;
        fname = Logger::getLogFileName(m_id, fcnt);
    }
    m_logger.initialize(fname);
    log(m_logger, LogMask::Memory,
        std::string("\n") + __FUNCTION__ + " constructor with id = " + std::to_string(m_id));
    initialize();
}

/**
 * @brief Initializes
 * @return int Zero for success
 */
int parfis::Parfis::initialize() 
{
    // Initialize all avalable Domains
    for (auto& it : Config::s_domainNameIdMap) {
        if (it.first == "system")
            m_domainMap[it.second] = std::unique_ptr<Domain>(new System(m_logger));
    }
    return 0;
}


int parfis::Parfis::configure(const char* str) 
{
    std::string cstr = Global::removeWhitespace(str);
    size_t fdot = cstr.find('.');
    std::string dname = cstr.substr(0, fdot);
    cstr = cstr.substr(fdot+1, cstr.size() - fdot);
    int id = Config::getDomainId(dname);
    if (id < 0)
        return -1;
    getDomain(id)->configure(cstr);
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
 * @param pfis Id of the Parfis object
 * @return const char* The object's info string
 */
PARFIS_EXPORT const char* parfis::ParfisAPI::parfisInfo(uint32_t id)
{
    static std::string str;
    
    if (Parfis::s_parfisMap.count(id) == 0) {
        str = "Parfis with id = " + std::to_string(id) + " doesn't exist";
    }
    else {
        str = "Parfis::m_id = " + std::to_string(Parfis::s_parfisMap.at(id)->m_id);
        str += "\nParfis::m_logger.m_fname = " + Parfis::s_parfisMap.at(id)->m_logger.m_fname;
    }

    return str.c_str();
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
