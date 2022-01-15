#ifndef PARFIS_DATASTRUCT_H
#define PARFIS_DATASTRUCT_H

/**
 * @file datastruct.h
 * @brief Data objects of Parfis.
 */

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <math.h>
#include <functional>
#include <memory>

/// Logging level defined from cmake is or-ed with bitmask to log strings.
#if defined(PARFIS_LOG_LEVEL)
#define LOG_LEVEL PARFIS_LOG_LEVEL
#else
#define LOG_LEVEL 0
#endif

/**
 * @defgroup logging
 * @brief Classes and functions used for logging
 * @{
 */
/// Logging macro function that yields calls to Logger::log only when LOG_LEVEL is not zero.
#if LOG_LEVEL > 0
#define LOG(logger, level, msg) {Logger::log(logger, level, msg);}
#else
#define LOG(logger, level, msg) {}
#endif // LOG_LEVEL
/** @} logging */

#if defined(STATE_TYPE_DOUBLE)
#define STATE_TYPE double
#else
#define STATE_TYPE float
#endif

namespace parfis {

    /// Type of state space variables of each particle (float or double)
    typedef STATE_TYPE state_t;
    /// Type for cell id
    typedef uint32_t cellId_t;
    /// Type for cell position vector components
    typedef uint16_t cell1D_t;
    /// Type for node bitwise marking
    typedef uint8_t nodeMask_t;
    /// Type for cell state
    typedef uint8_t cellState_t;

    /**
     * @addtogroup logging
     * @{
     */ 
    /** 
     * @brief Log bitmask that corresponds to log level
     * @details Loging is performed by calling the macro with the desired log message type.
     * The macro calls a function from the parfis::Logger::log and based on the LOG_LEVEL saves 
     * the message. For example the comand <c>LOG(m_logger, LogMask::Info, "Test message");</c> 
     * will log if the LOG_LEVEL has set bit on position four.
     */ 
    enum LogMask: uint32_t {
        /// No logging (mask: 0000)
        None = 0b0,
        /// Log error messages (mask: 0001)
        Error = 0b1,
        /// Log warning messages (mask: 0010)
        Warning = 0b10,
        /// Log messages concerned with memory allocation/deallocation (mask: 0100)
        Memory = 0b100,
        /// Log info messages (mask: 1000)
        Info = 0b1000
    };

    /**
     * @brief Logger class
     */
    struct Logger
    {
        Logger(): m_fname(""), m_str(""){};

        /// String with the log text
        std::string m_str;
        /// File name where the log text is written
        std::string m_fname;

        void initialize(const std::string& fname);
        void logToStr(LogMask mask, const std::string& msg);
        void printLogFile();
        static std::string getLogFileName(uint32_t id, uint32_t cnt);
        /**
         * @brief Function for logging.
         * @details Functiion is meant to be called through the LOG macro function. Which yelds
         * no code if LOG_LEVEL is zero.
         * @param logger Reference to a logger to be used
         * @param mask parfis::LogMask enum value
         * @param msg String for logging
         */
        static void log(Logger& logger, LogMask mask, const std::string& msg) {
            if (LOG_LEVEL > 0) {
                if(mask & LOG_LEVEL) logger.logToStr(mask, msg);
            }
        }
    };
    /** @} logging */

    /**
     * @defgroup data
     * @brief Data structures.
     * @{
     */
    /** 
     * @struct Vec3D
     * @brief Struct that represents a 3d vector with components of type T.
     * @details The struct has three member variables x, y and z of type T.
     */
    template <class T>
    struct Vec3D
    {
        /// Variable x
        T x;
        /// Variable y
        T y;
        /// Variable z
        T z;

        /**
         * @brief Compares if vectors are equal, that is if every component equals.
         */
        friend bool operator==(const Vec3D<T>& lhs, const Vec3D<T>& rhs) {
            return (lhs.x == rhs.x && lhs.y == rhs.y && rhs.z == rhs.z);
        }

        /** 
         * @brief Returns the vector length in Eucledian metric.
         * @details The length is calculated as @f$ \sqrt{x^2 + y^2 + z^2} @f$
         */
        T len() { return sqrt(x * x + y * y + z * z); }

        /** 
         * @brief Returns the squared vector length in Eucledian metric.
         * @details The length is calculated as @f$ x^2 + y^2 + z^2 @f$
         */
        T lenSq() { return x * x + y * y + z * z; }
    };

    /**
     * @brief Squared distance between two points in the x-y plane.
     * @param a first point
     * @param b second point
     * @return distance of two points as double
     */
    inline double xyDistSq(Vec3D<double>& a, Vec3D<double>& b) {
        return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y);
    };

    /**
     * @brief Structure used to represent space cells
     * @details 
     *  Cells nodes can be inside or outside the simulation space - i.e. geometry. 
     *  If all cell nodes are outside the geometry then the cell is not used in the 
     *  simulation.
     * 
     *  We use a 8bit number as a bitwise marker for nodes:
     *
     *         e-------g       | z
     *        /|      /|       |
     *       / |     / |       + ---- y
     *      f--|----h  |      /
     *      |  a----|--c     / x
     *      | /     | /
     *      b-------d
     *
     *  node position inside a cell are bitwise ordered: h g f e d c b a
     *
     *  In the given node representation for a single cell, if the bit is 1 then that 
     *  node is inside the simulation space.
     *
     *  Nodes laying exactly on the border are considered OUTSIDE the geometry and are 
     *  marked with 0.
     *
     *  For example the cylindrical geometry is defined with box boundaries inside which
     *  lies a cylinder that defines the simulation space. Nodes inside the cylnder are 
     *  part of the simulation space. For clindrical geometry, if the XY point is inside, 
     *  then both Z points are inside, except on the two basis of the cylinder laying on 
     *  the boundaries.
     */
    struct Cell
    {
        /// Bit mask for nodes inside or outside the simulation space
        nodeMask_t nodeMask;
        /// State for the whole cell
        cellState_t state;
        /// Cell position is represented with three integers x,y,z
        Vec3D<cell1D_t> cell3D;
    };

    /**
     * @brief Holds information about each specie
     * 
     */
    struct Specie
    {
        /// Specie name
        std::string name;
        /// States per cell for creating initial particles
        int statesPerCell;
    };

    /**
     * @brief Configuration parameters data
     */
    struct CfgData {
        /// Geometry type
        std::string geometry;
        /// Timestep for the system in seconds
        double timestep;
        /// Geometry size in meters (bounding box of the simulation space)
        Vec3D<double> geometrySize;
        /// Single cell size in meters
        Vec3D<double> cellSize;
        /// Periodic boundary 0-no, 1-yes
        Vec3D<int> periodicBoundary;
        /// Number of cells in every direction
        Vec3D<int> cellCount;
        /// Vector of species
        std::vector<Specie> specieVec;
        /// Get absolute cell id from i,j,k
        inline cellId_t getCellVecPosition(Vec3D<cell1D_t>& cell3D) {
            return cellCount.x * (cellCount.y * cell3D.z + cell3D.y ) + cell3D.x;
        };
    };

    /**
     * @brief Simulation data
     */
    struct SimData {
        /// Vector of cells
        std::vector<Cell> cellVec;
        /**
         * @brief Vector of cellId pointers to the cellVec.
         * @details Maping of absolute cell id (calculated from cellPos) to real cell id
         * which is the position in the cellVec vector
         */ 
        std::vector<cellId_t> cellIdVec;
    };
    /** @} data */

    /**
     * @brief Chain of commands
     * @details Used for chaining functions from the Domain objects that have a 
     * certain signature. Has a single-linked list structure. Commands are saved in a map
     * which has a key corresponding to m_name.
     */
    struct Command
    {
        Command(std::string name = "") :
            m_name(name), m_funcName(""), m_func(nullptr), m_pNext(nullptr) {}

        /// Name for the command
        std::string m_name;

        /// Function name
        std::string m_funcName;

        /// Function to run in the command object
        std::function<int()> m_func;

        /// Pointer to the next command in the chain
        Command* m_pNext;

        /// Set next command in the chain
        void setNext(Command& comRef) { m_pNext = &comRef; }

        /// Get the next command in the chain
        Command* getNext() { return m_pNext; };
    };

    /**
     * @brief Defines the whole command chain
     */
    struct CommandChain : public Command
    {
        /// Map of commands
        std::map<std::string, std::unique_ptr<Command>> m_cmdMap;
    };

    /**
     * @defgroup configuration
     * @brief Tree data structure used for configuring parfis.
     * @{
     */
    /**
     * @brief Base class used for configurations
     * @details The ParamBase acts as a node in a tree data structure. Root nodes are of
     * Domain type.
     */
    struct ParamBase
    {
        std::string m_name;
        std::string m_type;
        size_t m_size;
        ParamBase* m_parent;
        bool inRange(const std::string& valstr);
        std::string getValueString(bool printType=false);
        /// Map of children ParamBase objects (functions as a data containter)
        std::map<std::string, std::unique_ptr<ParamBase>> m_childMap;
        template<class S>
        void addChild(const std::string& name);
        static void setValueVec(ParamBase* ppb, const std::string& valstr);
        static void setRangeVec(ParamBase* ppb, const std::string& ranstr);
    };

    /**
     * @brief Class derived from ParamBase that holds parameter values.
     * @details Class is used as leaf node in the configuration data tree structure. 
     * @tparam T type can be on of the following (int, double, std::string, parfis::state_t)
     */
    template<class T>
    struct Param : public ParamBase
    {
        Param();
        /// Vector of parameter values
        std::vector<T> m_valueVec;
        /// Vector with the range of allowed values
        std::vector<T> m_rangeVec;
        void setValueVec(const std::string& valstr);
        void setRangeVec(const std::string& ranstr);
        bool inRange(T value);
    };

    template<>
    inline Param<double>::Param() { m_type = "double"; }
    
    template<>
    inline Param<int>::Param() { m_type = "int"; }

    template<>
    inline Param<std::string>::Param() { m_type = "std::string"; }


    /**
     * @struct Domain
     * @brief Class is used as a base class
     * @details Objects used in the Parfis class, are objects from classes that inherit Domain.
     * Domain is used as an abstract class to have its functionality exposed to the API, along
     * with the speciffic functionalities of its children.
     */
    struct Domain: public Param<std::string> {
        Domain() = default;
        Domain(const std::string& dname, Logger& logger, CfgData& cfgData, SimData& simData, 
                std::map<std::string, std::unique_ptr<CommandChain>>& cmdChainMap) 
            : m_pLogger(&logger), m_pCfgData(&cfgData), m_pSimData(&simData), 
            m_pCmdChainMap(&cmdChainMap) { m_name = dname; };
        Domain(const Domain&) = default;
        Domain& operator=(const Domain&) = default;
        virtual ~Domain() = default;

        parfis::Param<std::string>* getParent(const std::string& cstr);
        int initialize(const std::string& cstr);
        int configure(const std::string& cstr);
        virtual int loadCfgData() = 0;

        static std::unique_ptr<Domain> generateDomain(const std::string& dname, Logger& logger, 
            CfgData& cfgData, SimData& simData, 
            std::map<std::string, std::unique_ptr<CommandChain>>& cmdChainMap);

        template<class T>
        void getParamToValue(const std::string& key, T& valRef);

        template<class T>
        void getParamToVector(const std::string& key, std::vector<T>& vecRef);

        /// Pointer to the Logger object from parfis
        Logger* m_pLogger;
        /// Pointer to configuration data
        CfgData* m_pCfgData;
        /// Pointer to simulation data
        SimData* m_pSimData;
        /// Pointer to command map
        std::map<std::string, std::unique_ptr<CommandChain>>* m_pCmdChainMap;
    };
    /** @} configuration */
}

#endif // PARFIS_DATASTRUCT_H