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

namespace parfis {
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
        ~Parfis() = default;

        static Parfis* newParfis();

        /** Id of the created object (same as Parfis::s_parfisMap id) */
        uint32_t m_id;

        /** The static map tracks created Parfis objects. */
        static std::map<uint32_t, std::unique_ptr<Parfis>> s_parfisMap;
    };

    namespace cAPI {
        
        /** @defgroup cAPI C functions API
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
        };
        /** @} */ // end of group C fucntions API
    }
}

#endif // PARFIS_H