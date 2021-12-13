/**
 * @file global.h
 *
 * @brief Global definitions and functions.
 */
#ifndef PARFIS_GLOBAL_H
#define PARFIS_GLOBAL_H

#include <string>
#include <vector>
#include <map>

#if defined(PARFIS_LOG_LEVEL)
#define LOG_LEVEL PARFIS_LOG_LEVEL
#else
#define LOG_LEVEL 0
#endif

#if defined(STATE_TYPE_DOUBLE)
#define STATE_TYPE double
#else
#define STATE_TYPE float
#endif

/// Main namespace of the library
namespace parfis 
{
    /// Get current date and time in the format YYYY-MM-DD HH:mm:ss
    const std::string currentDateTime() 
    {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m %d.%X", &tstruct);
        return buf;
    }


    /** @defgroup phys_const Constants
     * @brief Physical and mathematical constants
     * @{
     */
    /// Pi
    constexpr double PI = 3.14159265358979323846;
    /// Pi/2
    constexpr double PI_2 = 1.57079632679489661923;
    /// Atomic mass unit [kg]
    constexpr double KG_IN_AMU = 1.66053906660e-27;
    /// Electron mass [kg]
    constexpr double ELECTRON_MASS = 9.1093837015e-31;
    /// Elementary charge [C]
    constexpr double ELEMENTARY_CHARGE = 1.602176634e-19;
    /// One joule in electronvolts [eV]
    constexpr double JOULE_IN_EV = 1.602176634e-19;
    /// One electronvolt in joules [J]
    constexpr double EV_IN_JOULE = 6.2415090744608e18;
    /** @} */ // end of group phys_const

    
    typedef STATE_TYPE state_t;
}

#endif // PARFIS_GLOBAL_H