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

#if LOG_LEVEL > 0
#define LOG(lvl, msg) if(uint32_t(lvl) & LOG_LEVEL) Log::log(lvl, msg);
#define LOGFUNC(lvl, msg) if(uint32_t(lvl) & LOG_LEVEL) Log::log(lvl, "["+std::string(__FUNCTION__)+"] " + msg);
#define LOGBLOCK(lvl, header, msg) if(uint32_t(lvl) & LOG_LEVEL) { Log::log(lvl, "---------- START BLOCK: " + header + " ---------\n" + msg); Log::log(lvl, "---------- END BLOCK: " + header + " -----------");}
#else
#define LOG(lvl, msg)  
#define LOGFUNC(lvl, msg)  
#endif // LOG_LEVEL

#if defined(STATE_TYPE_DOUBLE)
#define STATE_TYPE double
#else
#define STATE_TYPE float
#endif

namespace parfis 
{

    /** @defgroup phys_const Constants
     * @brief Physical and mathematical constants
     * @{
     */
    /** Pi */
    const double PI = 3.14159265358979323846;
    /** Pi/2 */
    const double PI_2 = 1.57079632679489661923;
    /** Atomic mass unit [kg] */
    const double KG_IN_AMU = 1.66053906660e-27;
    /** Electron mass [kg] */
    const double ELECTRON_MASS = 9.1093837015e-31;
    /** Elementary charge [C] */
    const double ELEMENTARY_CHARGE = 1.602176634e-19;
    /** One joule in electronvolts [eV] */
    const double JOULE_IN_EV = 1.602176634e-19;
    /** One electronvolt in joules [J] */
    const double EV_IN_JOULE = 6.2415090744608e18;
    /** @} */ // end of group phys_const

    
    typedef STATE_TYPE state_t;
}

#endif // PARFIS_GLOBAL_H