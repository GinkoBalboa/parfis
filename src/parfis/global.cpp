#include "parfis.h"
#include "global.h"
#include "version.h"

const char* parfis::Const::version = VERSION;
const char* parfis::Const::gitTag = GIT_TAG;
uint32_t parfis::Const::logLevel = LOG_LEVEL;

std::string parfis::Global::currentDateTime() 
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m %d.%X", &tstruct);
    return buf;
}

std::string parfis::Global::removeWhitespace(const std::string& str)
{
    std::string cstr = str;
    cstr.erase(std::remove_if(cstr.begin(), cstr.end(), ::isspace), cstr.end());
    return cstr;
}

/**
 * @brief Create key,value pair from "key=value" string
 * 
 * @param str String must be without whitespace characters
 * @return std::tuple<std::string, std::string> Key value pair as tuple
 */
std::tuple<std::string, std::string> parfis::Global::splitKeyValue(const std::string& str)
{
    size_t eqPos = str.find('=');
    return {str.substr(0,eqPos), str.substr(eqPos+1,str.size())};
}

bool parfis::Global::fileExists(const std::string& fname) 
{
    std::ifstream f(fname.c_str());
    return f.good();
}