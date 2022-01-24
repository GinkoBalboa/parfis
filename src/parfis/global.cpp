#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <ctype.h>
#include "parfis.h"
#include "global.h"
#include "version.h"

const char* parfis::Const::version = VERSION;
const char* parfis::Const::gitTag = GIT_TAG;
const uint32_t parfis::Const::logLevel = LOG_LEVEL;
const std::string parfis::Const::multilineSeparator = "---------------------------------------\n";

/**
 * @brief Get current date and time as a string
 * @return String of current date and time
 */
std::string parfis::Global::currentDateTime() 
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

/**
 * @brief Remove all characters that return true to ::isspace
 * @param str original string
 * @return String with the removed whitespaces
 */
std::string parfis::Global::removeWhitespace(const std::string& str)
{
    std::string cstr = str;
    cstr.erase(std::remove_if(cstr.begin(), cstr.end(), isspace), cstr.end());
    return cstr;
}

/**
 * @brief Create key,value pair from "key=value" string
 * @param str String must be without whitespace characters
 * @return Key value pair as tuple
 */
std::tuple<std::string, std::string> parfis::Global::splitKeyValue(const std::string& str)
{
    size_t eqPos = str.find('=');
    size_t endPos = std::min(str.find('<'), std::min(str.find('('), str.find('#'))); 
    return {str.substr(0,eqPos), str.substr(eqPos+1, endPos-eqPos-1)};
}

/// @todo Write doxy
std::tuple<std::string, std::string> parfis::Global::splitKeyString(const std::string& str)
{
    size_t eqPos = str.find('=');
    size_t endPos = str.find('#'); 
    return {str.substr(0,eqPos), str.substr(eqPos+1, endPos-eqPos-1)};
}

/**
 * @brief Return last child name if there is a '.' as a parent-child classifier
 * @param str string of the format "parent.child.child..."
 * @return last child name
 */
std::string parfis::Global::childName(const std::string& str)
{
    size_t dotPos = str.find_last_of('.'); 
    if (dotPos == std::string::npos)
        return "";
    return str.substr(dotPos+1, str.size() - dotPos - 1);
}

bool parfis::Global::fileExists(const std::string& fname) 
{
    std::ifstream f(fname.c_str());
    return f.good();
}

/**
 * @brief Parses string and returns vector of elements
 * @param str string to be parsed of the form [a,b,c]
 * @param bra starting array character (ex. '[' or '(')
 * @param ket ending array character (ex. ']' or ')')
 * @return Vector of strings 
 */
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

/**
 * @brief Returns vector of iheritance
 * @details Splits a string where the split character is dot and constructs a vector
 * @param str string in the format grandparent.parent.child
 * @return vector of parents and children
 */
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

std::string parfis::Global::to_string(int num)
{
    return std::to_string(num);
}