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
#define DECL_EXPORT __attribute__((visibility("default")))
#define DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(PARFIS_SHARED_LIB)
#define PARFIS_EXPORT DECL_EXPORT
#else
#define PARFIS_EXPORT DECL_IMPORT
#endif

#if defined(PARFIS_LOG_LEVEL)
#define LOG_LEVEL PARFIS_LOG_LEVEL
#else
#define LOG_LEVEL 0
#endif

namespace parfis {

    /// Log bitmask that corresponds to log level
    enum LogMask: uint32_t {
        None = 0b0,
        Error = 0b1,
        Warning = 0b10,
        Memory = 0b100,
        Info = 0b1000
    };

    /// Logging class
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
    };

    /**
     * @brief Constexpr function for logging
     * 
     * @param logger Reference to a logger to be used
     * @param mask LogMask enum value
     * @param msg String for logging
     */
    constexpr void log(Logger& logger, LogMask mask, const std::string& msg) {
        if (LOG_LEVEL > 0) {
            if(mask & LOG_LEVEL) logger.logToStr(mask, msg);
        }
    }

    struct ParamBase
    {
        uint32_t m_id;
        std::string m_name;
        std::string m_domain;
        std::string m_type;
        std::vector<ParamBase> m_paramVec;
    };

    template<class T>
    struct Param : public ParamBase
    {
        T m_default;
        T m_value;
        std::vector<T> m_range;
        bool inRange(const T& val) { 
            return val >= m_range[0] && val <= m_range[1]; 
        }
    };

    /**
     * @struct Domain
     * @brief Domain class that is used as base class
     * @details Objects used in the Parfis class, are objects from classes that inherit Domain.
     * Domain is used as an abstract class to have its functionality exposed to the API, along
     * with the speciffic functionalities of its children.
     */
    struct Domain {
        Domain() = default;
        Domain(Logger& logger, const std::string& dname);
        Domain(const Domain&) = default;
        Domain& operator=(const Domain&) = default;
        virtual ~Domain() = default;

        virtual int initialize() = 0;
        virtual int configure(const std::string& cstr) = 0;

        /// Pointer to the Logging object from parfis
        Logger* m_logger;
        /// Parameters for this domain 
        std::vector<ParamBase> m_paramVec;

        std::string m_domainName;
    };

    /** 
     * @struct Parfis
     * @brief The main class in the parfis library
     * @details Class methods are accessed through the C API functions.
     */
    struct Parfis
    {
        Parfis() = default;
        Parfis(uint32_t id);
        Parfis(const Parfis&) = default;
        Parfis& operator=(const Parfis&) = default;
        ~Parfis();

        static Parfis* newParfis();

        int initialize();
        int configure(const char* str);

        /// Logging object
        Logger m_logger;

        /// Id of the created object (same as Parfis::s_parfisMap id)
        uint32_t m_id;

        /// Static map of pointers to Parfis objects
        static std::map<uint32_t, std::unique_ptr<Parfis>> s_parfisMap;

        static Parfis* getParfis(uint32_t);
        Domain* getDomain(uint32_t);

        /// Map of pointers Domain objects (System, Particle, Field)
        std::map<uint32_t, std::unique_ptr<Domain>> m_domainMap;
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
        };
        /** @} */ // end of group C fucntions API
    }
}

#endif // PARFIS_H