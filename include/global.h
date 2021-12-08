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

#define LOG_LEVEL 15

#if defined(PARFIS_STATE_TYPE_DOUBLE)
#define PARFIS_STATE_TYPE double
#else
#define PARFIS_STATE_TYPE float
#endif

namespace parfis 
{

    /** @defgroup physical_constants Physical constants 
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
    /** @} */ // end of group physical_constants

    
    typedef PARFIS_STATE_TYPE state_t;
}

#endif // PARFIS_GLOBAL_H