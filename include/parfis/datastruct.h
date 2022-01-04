#ifndef PARFIS_DATASTRUCT_H
#define PARFIS_DATASTRUCT_H

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

    struct CfgData : public DataBase {
        double systemTimestep;
    };
}

#endif // PARFIS_DATASTRUCT_H