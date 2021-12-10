#ifndef PARFIS_INFO_H
#define PARFIS_INFO_H

#include <vector>
#include <map>

#define RETONERR() if (m_result.isErr()) return;

#define RESULT_KEY_NOT_FOUND(x) { 1, "keyword '" + std::string(x) + "' not found" };
#define RESULT_VALUE_NOT_RECOGNIZED(x, k) { 1, "value '" + std::string(x) + "' for key '" + std::string(k) + " ' not recognized" };

#define IS_IN_VECTOR(x, v) (find(v.begin(), v.end(), x) != v.end())

#define GET_KEY_SUBSTR(x, key) x.substr(key.size() + 1, x.size() - key.size() - 1).c_str()
#define COMPARE_KEY_SUBSTR(x, key) x.compare(0, key.size(), key)

namespace parfis
{
    struct KeySetSystem {
        const std::string geometry = "geometry";
        const std::string timestep = "timestep";
        const std::string eFieldType = "E_field_type";
        const std::string eFieldValue = "E_field_value";
        const std::string bFieldType = "B_field_type";
        const std::string bFieldValue = "B_field_value";
        const std::string gasName = "gas_name";
        const std::string gasTemperature = "gas_temperature";
        const std::string gasDensity = "gas_density";
        const std::string collisionAlgorithm = "collision_algorithm";
    };

    struct KeySetSpace {
        const std::string geometrySize = "geometry_size";
        const std::string unitCellSize = "unit_cell_size";
        const std::string periodicBoundary = "periodic_boundary";
    };

    struct KeySetParticle {
        const std::string name = "name";
        const std::string particlesPerCell = "particles_per_cell";
        const std::string timestepRatio = "timestep_ratio";
        const std::string mass = "mass";
        const std::string charge = "charge";
        const std::string xsectionName = "xsection_name";
    };

    struct KeySetXSection {
        const std::string name = "name";
        const std::string type = "type";
        const std::string thres = "threshold"; 
        const std::string xAxis = "x_axis";
        const std::string yAxis = "y_axis";
    };

    struct KeyGet {
        const std::string unitCellSize = "unit_cell_size";
        const std::string unitCellCnt = "unit_cell_cnt";
        const std::string geometrySize = "geometry_size";
        const std::string particleCnt = "particle_cnt";
    };

    struct KeyWord {
        const std::string system = "system";
        const std::string space = "space";
        const std::string particle = "particle";
        const std::string xsection = "xsection";
        const std::string cylindrical = "cylindrical";
        const std::string cubical = "cubical";
        const std::string periodic = "periodic";
        const std::string constant = "constant";
        const std::string step = "step";
        const std::string none = "none";
        const std::string collisionTime = "collision_time";
    };

    struct Key {
        static KeyWord word;
        static KeySetSystem setSystem;
        static KeySetParticle setParticle;
        static KeySetSpace setSpace;
        static KeySetXSection setXSection;
        static KeyGet keyGet;
        static std::vector<std::string> sectionNames;
        static std::vector<std::string> geometryValues;
        static std::vector<std::string> eFieldTypeValues;
        static std::vector<std::string> bFieldTypeValues;
        static std::vector<std::string> xSecTypeValues;
        static std::vector<std::string> collisionTypeValues;
        static inline int getWordIndex(
            std::vector<std::string>& strVec, std::string& word)
        {
            int i = 0;
            for (auto& str : strVec) {
                if (str == word)
                    return i;
                i++;
            }
            return -1;
        }
    };

    struct Info {
        static std::map<std::string, std::tuple<std::string, std::string, std::string>> keyMap;
        static std::string logFileHeader;
    };

    struct Log
    {
        enum struct Lvl: uint32_t {
            None = 0b0,
            Error = 0b1,
            Warning = 0b10,
            Code = 0b100,
            Info = 0b1000
        };

        static std::string s_log;
        static std::string s_logFileName;

        static void log(Log::Lvl lvl, std::string& str);
        static void log(Log::Lvl lvl, const char* str);
        static void setLogFile(std::string fname);
        static void printLogFile();
    };

}

#endif // PARFIS_INFO_H