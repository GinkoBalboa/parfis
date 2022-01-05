#ifndef PARFIS_DATASTRUCT_H
#define PARFIS_DATASTRUCT_H

/**
 * @file datastruct.h
 * @brief Data objects of Parfis.
 */

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <math.h>
#include <functional>
#include "parfis.h"

namespace parfis {

    /**
     * @addtogroup data
     * @{
     */
    /**
     * @brief Configuration parameters data
     */
    struct CfgData : public DataBase {
        /// Timestep for the system in seconds
        double systemTimestep;
    };
    /** @} data */
}

#endif // PARFIS_DATASTRUCT_H