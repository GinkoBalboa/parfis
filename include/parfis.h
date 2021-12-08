#ifndef PARFIS_H
#define PARFIS_H

#include <string>
#include <vector>
#include <map>

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
    namespace parfisAPI {
        
        /** @defgroup parfisAPI
         *
         *  @brief C Functions implementation of the interface class ParfisAPI
         *
         *  @details Functions are compiled with the **extern "C"** thus forbiding
         *           C++ to mangle the function names. This is essential for 
         *           using these functions with python ctypes.
         * 
         * @{
         */
        extern "C" 
        {
            PARFIS_EXPORT void printLog();
            PARFIS_EXPORT const char* version();
        };
        /** @} */ // end of group parfisAPI
    }
}

#endif // PARFIS_H