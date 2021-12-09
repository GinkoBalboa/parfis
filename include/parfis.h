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
     * 
     * @brief Parfis main class
     *
     * @details Class is accessed through cAPI functions and is not exported 
     *          outside this library.
     */
    struct Parfis
    {
        Parfis() = default;
        Parfis(uint32_t id);
        Parfis(const Parfis&) = default;
        Parfis& operator=(const Parfis&) = default;
        ~Parfis() = default;

        /** The static map tracks all created Parfis objects. */
        static std::map<uint32_t, std::unique_ptr<Parfis>> s_parfisMap;
    };

    namespace cAPI {
        
        /** @defgroup cAPI
         *
         *  @brief Exported C Functions of the interface class ParfisAPI
         *
         *  @details Functions are compiled with the **extern "C"** thus forbiding
         *           C++ to mangle the function names. This is essential for 
         *           using these functions with python ctypes.
         * 
         * @{
         */
        extern "C" 
        {
            PARFIS_EXPORT const char* info();
            PARFIS_EXPORT const char* version();
            PARFIS_EXPORT Parfis* newParfis();
        };
        /** @} */ // end of group cAPI
    }
}

#endif // PARFIS_H