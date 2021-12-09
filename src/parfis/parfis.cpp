#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include "parfis.h"
#include "global.h"
#include "version.h"

std::map<uint32_t, std::unique_ptr<parfis::Parfis>> parfis::Parfis::s_parfisMap;

PARFIS_EXPORT const char* parfis::cAPI::info()
{
    static std::string str;
    str = "info:";
    str += "\nparfis::state_t size: " + std::to_string(sizeof(parfis::state_t));
    str += "\nLOG_LEVEL: " + std::to_string(LOG_LEVEL);

    return str.c_str();
}

PARFIS_EXPORT const char* parfis::cAPI::version()
{
    return VERSION;
}

PARFIS_EXPORT parfis::Parfis* parfis::cAPI::newParfis()
{
    static Parfis pfis;
    return &pfis;
}
