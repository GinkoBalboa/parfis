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

std::map<uint32_t, std::unique_ptr<parfis::Parfis>> parfis::Parfis::s_parfisMap;

/**
 * @brief Initializes log string and file name 
 * @param fname Name of the file to write log to
 */
void parfis::Logger::initialize(const std::string& fname) {
    m_fname = fname;
    m_str = "Parfis log file";
    m_str += "\n--------------";
    m_str += "\nCreated on: " + parfis::currentDateTime();
    m_str += "\n" + std::string(cAPI::info());
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
    initialize();
    log(m_logger, LogMask::Memory, 
        std::string("\n") + __FUNCTION__ + " constructor with id = " + std::to_string(m_id));
}

/**
 * @brief Initializes
 * @return int Zero for success
 */
int parfis::Parfis::initialize() 
{
    int fcnt = 0;
    std::string fname = "./parfisLog_id" + std::to_string(m_id) + "_0";
    while(fileExists(fname)) {
        fcnt++;
        fname = "./parfisLog_id" + std::to_string(m_id) + '_' + std::to_string(fcnt);
    }
    m_logger.initialize(fname);
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
PARFIS_EXPORT const char* parfis::cAPI::info()
{
    static std::string str;
    str = "state_t size = " + std::to_string(sizeof(parfis::state_t));
    str += "\nLOG_LEVEL = " + std::to_string(LOG_LEVEL);
    str += "\nVERSION = " + std::string(VERSION);
    str += "\nGIT_TAG = " + std::string(GIT_TAG);
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
PARFIS_EXPORT const char* parfis::cAPI::parfisInfo(uint32_t id)
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
PARFIS_EXPORT const char* parfis::cAPI::version()
{
    return VERSION;
}

/**
 * @brief Creates new Parfis object and returns its id.
 * @return uint32_t Id of the created object
 */
PARFIS_EXPORT uint32_t parfis::cAPI::newParfis()
{
    return Parfis::newParfis()->m_id;
}
