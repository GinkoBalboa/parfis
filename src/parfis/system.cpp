#include <fstream>
#include "config.h"
#include "system.h"
#include "global.h"

/**
 * @brief Loads data into CfgData object 
 * @details We access the CfgData object through a pointer, and the data object itself is a 
 * member of the Parfis class. 
 */
int parfis::System::loadCfgData()
{
    getParamToValue("timestep", m_cfgData->timestep);
    getParamToValue("geometrySize", m_cfgData->geometrySize);
    getParamToValue("cellSize", m_cfgData->cellSize);
    getParamToValue("periodicBoundary", m_cfgData->periodicBoundary);

    m_cfgData->cellCount.x = cellId_t(ceil(
        m_cfgData->geometrySize.x / m_cfgData->cellSize.x));
    m_cfgData->cellCount.y = cellId_t(ceil(
        m_cfgData->geometrySize.y / m_cfgData->cellSize.y));
    m_cfgData->cellCount.z = cellId_t(ceil(
        m_cfgData->geometrySize.z / m_cfgData->cellSize.z));

    // Check if you have enough memory to represent all cells with id
    uint64_t cellIdCount = m_cfgData->cellCount.x*m_cfgData->cellCount.y*m_cfgData->cellCount.z;
    if (cellIdCount >= uint64_t(Limit::cellIdMax)) {
        std::string msg = 
        "System::" + std::string(__FUNCTION__) + 
        " cell number limit exceeded. Requested " + std::to_string(cellIdCount) + 
        " cells, where the maximum number of cells is " + std::to_string(Limit::cellIdMax) + "\n";
        LOG(*m_logger, LogMask::Error, msg);
        return 1;
    }
    return 0;
}