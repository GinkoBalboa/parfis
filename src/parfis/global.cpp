#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <ctype.h>
#include "parfis.h"
#include "global.h"
#include "version.h"

const char* parfis::Const::version = VERSION;
const char* parfis::Const::gitTag = GIT_TAG;
const char* parfis::Const::buildConfig = BUILD_CONFIG;
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
    cstr.erase(std::remove_if(cstr.begin(), cstr.end(), ::isspace), cstr.end());
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

/**
 * @brief Gets string after '=' and '#' or end of string. Captures all
 * data as value, range and type of the data string. Used mainly for parsing
 * data that is not the value (ex. range).
 * 
 * @param str The string that has data in the forman key=value <additional data> (range data)
 * @return Key value pair as str 
 */
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

/**
 * @brief Splits string with a dot to two strings, before and after the dot.
 * 
 * @param str The string that has data in the forman str1.str2
 * @return Two strings, str1 str2 as tuple
 */
std::tuple<std::string, std::string> parfis::Global::splitDot(const std::string& str) 
{
    size_t dotPos = str.find('.');
    size_t endPos = str.size();
    return {str.substr(0,dotPos), str.substr(dotPos+1, endPos-dotPos-1)};
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
    // Remove quotes from the string 
    line.erase(remove(line.begin(), line.end(), '"'), line.end());
    line.erase(remove(line.begin(), line.end(), '\''), line.end());
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
 * @brief Parses string of the format key=[val1, val2, val3, ...], where vals are int. Fills
 * the supplied vector referece. 
 * 
 * @tparam  int
 * @param vecRef Vector referece to be filled with data
 * @param str Data defined as string
 * @param bra Vector starts with bra character (usually '[')
 * @param ket Vector ends with character (usually ']')
 * @return int Zero on success
 */
template<>
int parfis::Global::setValueVec(
    std::vector<int>& vecRef, 
    const std::string& str, 
    char bra, 
    char ket) 
{
    std::tuple<std::string, std::string> keyValue = Global::splitKeyValue(str);
    std::string strTmp = std::get<1>(keyValue);
    auto valvec = Global::getVector(strTmp, bra, ket);
    vecRef.clear();
    for (auto& val: valvec)
        vecRef.push_back(std::strtol(val.c_str(), nullptr, 10));

    return 0;
}

/**
 * @brief Parses string of the format key=[val1, val2, val3, ...], where vals are double. Fills
 * the supplied vector referece. 
 * 
 * @tparam  double
 * @param vecRef Vector referece to be filled with data
 * @param str Data defined as string
 * @param bra Vector starts with bra character (usually '[')
 * @param ket Vector ends with character (usually ']')
 * @return int Zero on success
 */
template<>
int parfis::Global::setValueVec(
    std::vector<double>& vecRef, 
    const std::string& str, 
    char bra, 
    char ket) 
{
    std::tuple<std::string, std::string> keyValue = Global::splitKeyValue(str);
    std::string strTmp = std::get<1>(keyValue);
    auto valvec = Global::getVector(strTmp, bra, ket);
    vecRef.clear();
    for (auto& val: valvec)
        vecRef.push_back(std::strtold(val.c_str(), nullptr));

    return 0;
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