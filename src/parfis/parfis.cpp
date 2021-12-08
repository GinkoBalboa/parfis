#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "parfis.h"
#include "global.h"
#include "version.h"


PARFIS_EXPORT void parfis::parfisAPI::printLog()
{
    std::cout << "state_t size: " << sizeof(parfis::state_t) << std::endl;
}

PARFIS_EXPORT const char* parfis::parfisAPI::version()
{
    return VERSION;
}
