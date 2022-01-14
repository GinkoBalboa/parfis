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
#include <fstream>
#include <algorithm>
#include "datastruct.h"

/// Main namespace of the library
namespace parfis 
{
    struct Global {
        /// Get current date and time in the format YYYY-MM-DD HH:mm:ss
        static std::string currentDateTime();
        static std::string removeWhitespace(const std::string& str);
        
        static std::tuple<std::string, std::string> splitKeyValue(const std::string& str);
        static std::tuple<std::string, std::string> splitKeyString(const std::string& str);
        static std::string childName(const std::string& str);

        /// Check if file @param fname exists
        static bool fileExists (const std::string& fname);

        static std::vector<std::string> getVector(const std::string& str, char bra, char ket);
        static std::vector<std::string> getInheritanceVector(const std::string& str);

        /// Convert double to string
        static std::string to_string(double num);
        /// Convert float to string
        static std::string to_string(float num);
        /// Comvert int to string (for completness and use in Vec3D struct)
        static std::string to_string(int num);
    };

    /// Physical and mathematical constats
    struct Const {
        /// Pi
        static constexpr double pi = 3.14159265358979323846;
        /// Pi/2
        static constexpr double halfPi = 1.57079632679489661923;
        /// One atomic mass unit [kg]
        static constexpr double amuKg = 1.66053906660e-27;
        /// Electron mass [kg]
        static constexpr double eMass = 9.1093837015e-31;
        /// Elementary charge [C]
        static constexpr double eCharge = 1.602176634e-19;
        /// One joule in electronvolts [eV]
        static constexpr double JeV = 1.602176634e-19;
        /// One electronvolt in joules [J]
        static constexpr double eVJ = 6.2415090744608e18;
        /// Version string
        static uint32_t logLevel;
        /// Version string
        static const char* version;
        /// Git tag string
        static const char* gitTag;
        /// Maximum number of cell ids for cellIdVec containter
        static constexpr cellId_t cellIdMax = UINT32_MAX;
        /// Id that represents that no cell exists
        static constexpr cellId_t noCellId = UINT32_MAX;
    };
}

#endif // PARFIS_GLOBAL_H