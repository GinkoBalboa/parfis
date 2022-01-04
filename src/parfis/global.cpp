#include <sstream>
#include <iomanip>
#include <stdio.h>
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
 * @return Key value pair as tuple
 */
std::tuple<std::string, std::string> parfis::Global::splitKeyValue(const std::string& str)
{
    size_t eqPos = str.find('=');
    size_t endPos = std::min(str.find('<'), std::min(str.find('('), str.find('#'))); 
    return {str.substr(0,eqPos), str.substr(eqPos+1, endPos-eqPos-1)};
}

std::tuple<std::string, std::string> parfis::Global::splitKeyString(const std::string& str)
{
    size_t eqPos = str.find('=');
    size_t endPos = str.find('#'); 
    return {str.substr(0,eqPos), str.substr(eqPos+1, endPos-eqPos-1)};
}

std::string parfis::Global::childName(const std::string& str)
{
    size_t dotPos = str.find_last_of('.'); 
    return str.substr(dotPos+1, str.size() - dotPos - 1);
}

bool parfis::Global::fileExists(const std::string& fname) 
{
    std::ifstream f(fname.c_str());
    return f.good();
}

std::vector<std::string> parfis::Global::getVector(const std::string& str, char bra, char ket)
{
    std::vector<std::string> vec;
    size_t posStart, posEnd;
    std::string line = str;
    posStart = line.find(bra);
    line = line.substr(posStart + 1, line.find(ket) - posStart - 1);
    posStart = 0;
    posEnd = line.find(','); 
    while(posEnd != std::string::npos) {
        vec.push_back(line.substr(posStart, posEnd - posStart));
        posStart = posEnd + 1;
        posEnd = line.find(',', posStart);
    }
    vec.push_back(line.substr(posStart, posEnd - posStart));
    return vec;
}

std::vector<std::string> parfis::Global::getInheritanceVector(const std::string& str)
{
    std::vector<std::string> vec;
    size_t posStart, posEnd;
    std::string line = str;
    posStart = 0;
    posEnd = line.find('.'); 
    while(posEnd != std::string::npos) {
        vec.push_back(line.substr(posStart, posEnd - posStart));
        posStart = posEnd + 1;
        posEnd = line.find('.', posStart);
    }
    vec.push_back(line.substr(posStart, posEnd - posStart));
    return vec;
}

std::string parfis::Global::to_string(double num)
{
    char buf[50];
    std::snprintf(buf, sizeof(buf), "%.15g", num);
    return std::string(buf);
}

std::string parfis::Global::to_string(float num)
{
    char buf[50];
    std::snprintf(buf, sizeof(buf), "%.7g", num);
    return std::string(buf);
}