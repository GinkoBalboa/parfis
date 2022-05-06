/**
 * @mainpage Parfis
 * @section intro_sec Introduction
 * Parfis is an acronym for PARticles and FIeld Simulator. Parfis is written in 
 * C++ with Python bindings. The algorithm is based upon the particle-in-cell 
 * (PIC) method used for interacting many-particle systems. In the PIC method
 * individual particles (or fluid elements) in a Lagrangian frame are tracked 
 * in a continuous phase space, whereas moments of the distribution such as 
 * densities and currents are computed simultaneously on Eulerian (stationary) 
 * mesh points.
 * @subsection Installation
 * Installation is explained in the sphinx doc page: 
 * <a href="https://www.parfis.com/installation.html">Parfis installation</a>
 */

/**
 * @file parfis.h
 * @brief Main declarations for Parfis objects.
 */
#ifndef PARFIS_H
#define PARFIS_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "datastruct.h"

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || \
    defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || \
    defined(__WIN32__) || defined(__NT__)
#define DECL_EXPORT __declspec(dllexport)
#define DECL_IMPORT __declspec(dllimport)
#else
/// Export macro
#define DECL_EXPORT __attribute__((visibility("default")))
/// Import macro
#define DECL_IMPORT __attribute__((visibility("default")))
#endif

/// Export/import macro reconfiguration if compiling/calling the library.
#if defined(PARFIS_SHARED_LIB)
#define PARFIS_EXPORT DECL_EXPORT
#else
#define PARFIS_EXPORT DECL_IMPORT
#endif

namespace parfis {
    /** 
     * @struct Parfis
     * @brief The main class in the parfis library
     * @details Class methods are accessed through the C API functions.
     */
    struct Parfis
    {
        Parfis() = default;
        Parfis(uint32_t id, const std::string& cfgstr = "", uint32_t writeLogFile = 0);
        Parfis(const Parfis&) = default;
        Parfis& operator=(const Parfis&) = default;
        ~Parfis();

        static Parfis* newParfis(const std::string& cfgstr = "", uint32_t writeLogFile = 0);

        int initialize();
        int loadSimData();
        int loadCfgData();
        int runCommandChain(const std::string& str);
        int configure(const char* str);

        Domain* getDomain(const std::string& cstr);
        std::string getParamValueString(const std::string& key);
        void initializeDomains();

        /// Logging object
        Logger m_logger;

        /// Configuration string
        std::string m_cfgstr;

        /// Configuration data, from cfg strings and calculated
        CfgData m_cfgData;

        /// Simulation data, particles and cell data
        SimData m_simData;

        /// Id of the created object (same as Parfis::s_parfisMap id)
        uint32_t m_id;

        /// Domain vector
        std::vector<std::string> m_domainVec;

        /// Map of all domains (name -> Domain)
        std::map<std::string, std::unique_ptr<Domain>> m_domainMap;

        /// Static map of pointers to Parfis objects
        static std::map<uint32_t, std::unique_ptr<Parfis>> s_parfisMap;

        /// Map of command chains
        std::map<std::string, std::unique_ptr<CommandChain>> m_cmdChainMap;

        /// Parfis id counter, unique id for every parfis even when deleted
        static uint32_t s_parfisMapId;

        static Parfis* getParfis(uint32_t);
    };

    /// Exported C functions
    namespace api {
        
        /** @defgroup api
         *  @brief Exported C Functions of the main class Parfis
         *  @details Functions are compiled with the **extern "C"** thus forbiding
         * C++ to mangle the function names. This is essential for using these 
         * functions with python ctypes. This functions are the only way to access 
         * library functionality from outside.
         * @{
         */
        extern "C" 
        {
            PARFIS_EXPORT const char* info();
            PARFIS_EXPORT const char* parfisInfo(uint32_t id);
            PARFIS_EXPORT const char* getLogFileName(uint32_t id);
            PARFIS_EXPORT const char* version();
            PARFIS_EXPORT uint32_t newParfis(const char* cfgStr = "", uint32_t writeLogFile=0);            
            PARFIS_EXPORT int setConfig(uint32_t id, const char* str);
            PARFIS_EXPORT int loadCfgData(uint32_t id);
            PARFIS_EXPORT int loadSimData(uint32_t id);
            PARFIS_EXPORT int setPyCfgData(uint32_t id);
            PARFIS_EXPORT int setPySimData(uint32_t id);
            PARFIS_EXPORT const char* defaultConfiguration();
            PARFIS_EXPORT const char* getConfig(uint32_t id);
            PARFIS_EXPORT const char* getConfigParam(uint32_t id, const char* key);
            PARFIS_EXPORT const CfgData* getCfgData(uint32_t id);
            PARFIS_EXPORT const PyCfgData* getPyCfgData(uint32_t id);
            PARFIS_EXPORT const SimData* getSimData(uint32_t id);
            PARFIS_EXPORT const PySimData* getPySimData(uint32_t id);
            PARFIS_EXPORT int deleteParfis(uint32_t id);
            PARFIS_EXPORT int deleteAll();
            PARFIS_EXPORT const std::vector<uint32_t>& getParfisIdVec();
            PARFIS_EXPORT int runCommandChain(uint32_t id, const char* key);
            PARFIS_EXPORT const char* toStringDouble(double num);
            PARFIS_EXPORT const char* toStringFloat(float num);
        }
        /** @} api */
    }
}

#endif // PARFIS_H