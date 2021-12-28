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

/// Logging level defined from cmake is or-ed with bitmask to log strings.
#if defined(PARFIS_LOG_LEVEL)
#define LOG_LEVEL PARFIS_LOG_LEVEL
#else
#define LOG_LEVEL 0
#endif

/// Logging macro function that yields calls to Logger::log only when LOG_LEVEL is not zero.
#if LOG_LEVEL > 0
#define LOG(logger, level, msg) {Logger::log(logger, level, msg);}
#else
#define LOG(logger, level, msg) {}
#endif // LOG_LEVEL


namespace parfis {

    /** 
     * @brief Log bitmask that corresponds to log level
     * @details Loging is performed by calling the macro with the desired log message type.
     * The macro calls a function from the parfis::Logger::log and based on the LOG_LEVEL saves 
     * the message. For example the comand <c>LOG(m_logger, LogMask::Info, "Test message");</c> 
     * will log if the LOG_LEVEL has set bit on position four.
     */ 
    enum LogMask: uint32_t {
        /// No logging (mask: 0000)
        None = 0b0,
        /// Log error messages (mask: 0001)
        Error = 0b1,
        /// Log warning messages (mask: 0010)
        Warning = 0b10,
        /// Log messages concerned with memory allocation/deallocation (mask: 0100)
        Memory = 0b100,
        /// Log info messages (mask: 1000)
        Info = 0b1000
    };

    /**
     * @brief Logging class
     */
    struct Logger
    {
        Logger(): m_fname(""), m_str(""){};

        /// String with the log text
        std::string m_str;
        /// File name where the log text is written
        std::string m_fname;

        void initialize(const std::string& fname);
        void logToStr(LogMask mask, const std::string& msg);
        void printLogFile();
        static std::string getLogFileName(uint32_t id, uint32_t cnt);
        /**
         * @brief Function for logging.
         * @details Functiion is meant to be called through the LOG macro function. Which yelds
         * no code if LOG_LEVEL is zero.
         * @param logger Reference to a logger to be used
         * @param mask parfis::LogMask enum value
         * @param msg String for logging
         */
        static void log(Logger& logger, LogMask mask, const std::string& msg) {
            if (LOG_LEVEL > 0) {
                if(mask & LOG_LEVEL) logger.logToStr(mask, msg);
            }
        }
    };

    struct ParamBase
    {
        std::string m_name;
        std::string m_type;
        size_t m_size;
        ParamBase* m_parent;
        bool inRange(const std::string& valstr);
        bool isDomain();
        std::string getValueString();
        std::map<std::string, std::unique_ptr<ParamBase>> m_childMap;
    };

    template<class T>
    struct Param : public ParamBase
    {
        std::vector<T> m_valueVec;
        std::vector<T> m_rangeVec;
        void setType();
        bool inRange(T value);
        void addChild(const std::string& name);
    };

    /**
     * @struct Domain
     * @brief Domain class that is used as base class
     * @details Objects used in the Parfis class, are objects from classes that inherit Domain.
     * Domain is used as an abstract class to have its functionality exposed to the API, along
     * with the speciffic functionalities of its children.
     */
    struct Domain: public Param<std::string> {
        Domain() = default;
        Domain(const std::string& dname, Logger& logger);
        Domain(const Domain&) = default;
        Domain& operator=(const Domain&) = default;
        virtual ~Domain() = default;

        int initialize(const std::string& cstr);
        parfis::Param<std::string>* getParent(const std::string& cstr);
        virtual int configure(const std::string& cstr) = 0;

        /// Pointer to the Logger object from parfis
        Logger* m_logger;
    };

    /** 
     * @struct Parfis
     * @brief The main class in the parfis library
     * @details Class methods are accessed through the C API functions.
     */
    struct Parfis
    {
        Parfis() = default;
        Parfis(uint32_t id, const std::string& cfgstr = "");
        Parfis(const Parfis&) = default;
        Parfis& operator=(const Parfis&) = default;
        ~Parfis();

        static Parfis* newParfis();

        int initialize();
        int configure(const char* str);

        Domain* getDomain(const std::string& cstr);
        void initializeDomains();

        /// Logging object
        Logger m_logger;

        /// Configuration string
        std::string m_cfgstr;

        /// Id of the created object (same as Parfis::s_parfisMap id)
        uint32_t m_id;

        /// Map of all domains (name -> Domain)
        std::map<std::string, std::unique_ptr<Domain>> m_domainMap;

        /// Static map of pointers to Parfis objects
        static std::map<uint32_t, std::unique_ptr<Parfis>> s_parfisMap;

        static Parfis* getParfis(uint32_t);
    };

    /// Holds C functions visibile from outside (exported functions)
    namespace ParfisAPI {
        
        /** @defgroup ParfisAPI C functions API
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
            PARFIS_EXPORT const char* version();
            PARFIS_EXPORT uint32_t newParfis();
            PARFIS_EXPORT int configure(uint32_t id, const char* str);
            PARFIS_EXPORT const char* defaultConfiguration();
            PARFIS_EXPORT const char* configuration(uint32_t id);
        };
        /** @} */ // end of group C fucntions API
    }
}

#endif // PARFIS_H