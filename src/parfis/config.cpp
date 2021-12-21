#include "config.h"

std::map<std::string, uint32_t> parfis::Config::s_domainNameIdMap = 
{
    {"system", 0}
};

// template<> std::string parfis::Param<std::string>::s_type = "std::string";
// template<> std::vector<std::string> parfis::Param<std::string>::s_range = {
//     "cylindrical",
//     "cubical"
// };

// template<>
// bool parfis::Param<std::string>::inRange(const std::string& val) {
//     for (auto& str: s_range) if (val == str) return true;
//     return false;
// }

/**
 * @brief Get the id of the domain name, defined in Info::domainIdMap
 * 
 * @param cstr Domain name
 * @return int Domain id, if doesn't exsits returns -1
 */
int parfis::Config::getDomainId(const std::string& cstr) {
    auto dmap = s_domainNameIdMap.find(cstr);
    if (dmap == s_domainNameIdMap.end())
        return -1;
    else 
        return dmap->second;
}

std::string parfis::Config::getDomainName(uint32_t id) {
    for (auto& it: s_domainNameIdMap) {
        if (it.second == id)
            return it.first;
    }
    return "";
}