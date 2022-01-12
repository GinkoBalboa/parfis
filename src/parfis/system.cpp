#include <fstream>
#include "config.h"
#include "system.h"
#include "global.h"

/**
 * @brief Loads data into CfgData object 
 * @details We access the CfgData object through a pointer, and the data object itself is a 
 * member of the Parfis class. We create all available id for cells, no matter if the 
 * cell will later exists or no.
 */
int parfis::System::loadCfgData()
{
    getParamToValue("geometry", m_pCfgData->geometry);
    getParamToValue("timestep", m_pCfgData->timestep);
    getParamToValue("geometrySize", m_pCfgData->geometrySize);
    getParamToValue("cellSize", m_pCfgData->cellSize);
    getParamToValue("periodicBoundary", m_pCfgData->periodicBoundary);

    m_pCfgData->cellCount.x = cellId_t(ceil(
        m_pCfgData->geometrySize.x / m_pCfgData->cellSize.x));
    m_pCfgData->cellCount.y = cellId_t(ceil(
        m_pCfgData->geometrySize.y / m_pCfgData->cellSize.y));
    m_pCfgData->cellCount.z = cellId_t(ceil(
        m_pCfgData->geometrySize.z / m_pCfgData->cellSize.z));

    // Check if you have enough memory to represent all cells with id
    uint64_t cellIdCount = m_pCfgData->cellCount.x*m_pCfgData->cellCount.y*m_pCfgData->cellCount.z;
    if (cellIdCount >= uint64_t(Const::cellIdMax)) {
        std::string msg = 
        "System::" + std::string(__FUNCTION__) + 
        " cell number limit exceeded. Requested " + std::to_string(cellIdCount) + 
        " cells, where the maximum number of cells is " + std::to_string(Const::cellIdMax) + "\n";
        LOG(*m_pLogger, LogMask::Error, msg);
        return 1;
    }

    m_pSimData->cellIdVec.resize(
        m_pCfgData->cellCount.x*m_pCfgData->cellCount.y*m_pCfgData->cellCount.z, Const::noCellId);

    // if (m_pCfgData->geometry == "cylindrical") {
    //     m_cmdCreate.m_name = "System::createCellsCylindrical";
    //     m_cmdCreate.m_func = System::createCellsCylindrical;
    // }
    return 0;
}

/**
 * @brief Create cells for a cylindrical geometry
 * @return Zero on success
 */
int parfis::System::createCellsCylindrical()
{
    // This should be the same for cylindrical geometry
    if (m_pCfgData->geometrySize.x != m_pCfgData->geometrySize.y)
        return 1;

    // XYZ<double> rt;
    // uint8_t xypin;
    // uint8_t pin;

    Vec3D<double> geoCenter = {
        0.5 * m_pCfgData->geometrySize.x, 
        0.5 * m_pCfgData->geometrySize.y,
        0.5 * m_pCfgData->geometrySize.z};
    Vec3D<double> nodePosition;
    double radiusSquared = geoCenter.x*geoCenter.x + geoCenter.y+geoCenter.y; 
    nodeMask_t xyNode;
    nodeMask_t node;
    cellId_t cellId;

    for (cell1D_t i = 0; i < m_pCfgData->cellCount.x; i++) {
         // Find points inside the geometry
         for (cell1D_t j = 0; j < m_pCfgData->cellCount.y; j++) {
            xyNode = 0;
            nodePosition.x = i * m_pCfgData->cellSize.x;
            nodePosition.y = j * m_pCfgData->cellSize.y;
            if (xyDistSq(nodePosition, geoCenter) < radiusSquared)
                xyNode |= 0b00010001;

            nodePosition.x = (i + 1) * m_pCfgData->cellSize.x;
            if (xyDistSq(nodePosition, geoCenter) < radiusSquared)
                xyNode |= 0b00100010;

            nodePosition.x = i * m_pCfgData->cellSize.x;
            nodePosition.y = (j + 1) * m_pCfgData->cellSize.y;
            if (xyDistSq(nodePosition, geoCenter) < radiusSquared)
                xyNode |= 0b01000100;

            nodePosition.x = (i + 1) * m_pCfgData->cellSize.x;
            if (xyDistSq(nodePosition, geoCenter) < radiusSquared)
                xyNode |= 0b10001000;

            for (cell1D_t k = 0; k < m_pCfgData->cellCount.z; k++) {
                node = xyNode;
                if (k == 0)
                    node &= 0b11110000;
                else if (k == m_pCfgData->cellCount.z - 1)
                    node &= 0b00001111;

                // Create cells that have at least one point inside the 
                // defined geometry (node > 0)
                if (node) {
                    m_pSimData->cellVec.push_back({ node, {i, j, k} });
                    m_pSimData->cellIdVec[
                        m_pCfgData->getCellVecPosition(m_pSimData->cellVec.back().cell3D)
                        ] = m_pSimData->cellVec.size() - 1;
                }
            }
         }
    }

    std::string msg = "created " + std::to_string(m_pSimData->cellVec.size()) + 
        " cells for cylindrical geometry";
    LOG(*m_pLogger, LogMask::Memory, msg);

    //pData->headIdVec.resize(pData->cellVec.size()*pData->specieCnt, NULLCID);

    return 0;
}