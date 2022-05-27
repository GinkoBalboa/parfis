#include <fstream>
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
    getParamToVector("gas", m_pCfgData->gasNameVec);

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
        " cells, where the maximum number of cells is " + std::to_string(Const::cellIdMax) + "\n  ";
        LOG(*m_pLogger, LogMask::Error, msg);
        return 1;
    }

    return 0;
}

 /**
  * @brief Loads the defined data from CfgData into SimData
  * 
  * @return int 
  */
int parfis::System::loadSimData()
{
    getParamToValue("field.typeE", m_pSimData->field.typeE);
    getParamToValue("field.typeB", m_pSimData->field.typeB);
    getParamToValue("field.strengthE", m_pSimData->field.strengthE);
    getParamToValue("field.strengthB", m_pSimData->field.strengthB);

    std::string strTmp;
    m_pSimData->gasVec.resize(m_pCfgData->gasNameVec.size());
    for (size_t i = 0; i < m_pCfgData->gasNameVec.size(); i++) {
        m_pSimData->gasVec[i].id = i;
        m_pSimData->gasVec[i].name = m_pCfgData->gasNameVec[i].c_str();
        getParamToValue("gas." + m_pCfgData->gasNameVec[i] + ".amuMass", 
            m_pSimData->gasVec[i].amuMass);
        getParamToValue("gas." + m_pCfgData->gasNameVec[i] + ".volumeFraction", 
            m_pSimData->gasVec[i].volumeFraction);
        getParamToValue("gas." + m_pCfgData->gasNameVec[i] + ".temperature", 
            m_pSimData->gasVec[i].temperature);
        getParamToValue("gas." + m_pCfgData->gasNameVec[i] + ".molDensity", 
            m_pSimData->gasVec[i].molDensity);
    }

    // Create vector for cell id
    m_pSimData->cellIdVec.resize(
        m_pCfgData->cellCount.x*m_pCfgData->cellCount.y*m_pCfgData->cellCount.z, Const::noCellId);

    // Set command for creating cells
    Command *pcom;
    std::string cmdChainName = "create";
    std::string cmdName;
    // Check if creation is defined
    if (m_pCmdChainMap->find(cmdChainName) != m_pCmdChainMap->end()) {
        // Check if cell creation is defined
         cmdName = "createCells";
        if (m_pCmdChainMap->at(cmdChainName)->m_cmdMap.find(cmdName) != 
            m_pCmdChainMap->at(cmdChainName)->m_cmdMap.end()) {
            pcom = m_pCmdChainMap->at(cmdChainName)->m_cmdMap[cmdName].get();
            // Do this differently for different geometries
            if (m_pCfgData->geometry == 1) {
                // Instead of lambda expression here, we can also use 
                // pcom->m_func = std::bind(&System::createCellsCylindrical, this);
                pcom->m_func = [&]()->int { return createCellsCylindrical(); };
                pcom->m_funcName = "System::createCellsCylindrical";
                std::string msg = "createCells command defined with " + pcom->m_funcName + "\n";
                LOG(*m_pLogger, LogMask::Info, msg);
            }
        }
    }
    return 0;
}

/**
 * @brief Create cells for a cylindrical geometry
 * @return Zero on success
 */
int parfis::System::createCellsCylindrical()
{
    // This two vectors components should be equal for the cylindrical geometry
    if (m_pCfgData->geometrySize.x != m_pCfgData->geometrySize.y)
        return 1;

    // Center of the geometry
    Vec3D<double> geoCenter = {
        0.5 * m_pCfgData->geometrySize.x, 
        0.5 * m_pCfgData->geometrySize.y,
        0.5 * m_pCfgData->geometrySize.z};
    Vec3D<double> nodePosition;
    double radiusSquared = geoCenter.x*geoCenter.x; 
    nodeFlag_t xyNode;
    nodeFlag_t nodeFlag;
    cellId_t cellId;

    for (cellPos_t i = 0; i < m_pCfgData->cellCount.x; i++) {
         // Find points inside the geometry
         for (cellPos_t j = 0; j < m_pCfgData->cellCount.y; j++) {
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

            for (cellPos_t k = 0; k < m_pCfgData->cellCount.z; k++) {
                nodeFlag = xyNode;
                if (k == 0)
                    nodeFlag &= 0b11110000;
                else if (k == m_pCfgData->cellCount.z - 1)
                    nodeFlag &= 0b00001111;

                // Create cells that have at least one point inside the 
                // defined geometry (node > 0)
                if (nodeFlag) {
                    cellId = m_pSimData->cellVec.size();
                    m_pSimData->cellVec.push_back({ i, j, k });
                    m_pSimData->nodeFlagVec.push_back(nodeFlag);
                    m_pSimData->cellIdVec[
                        m_pCfgData->getAbsoluteCellId(m_pSimData->cellVec.back().pos)] = cellId;
                }
            }
         }
    }

    // For cylindrical we split to cells that have states that can go 
    // off the boundary (reflection checking) and those that can't go outside
    // the boundary
    cellId_t nCellId;
    Vec3D<cellPos_t> neigbourPos;
    bool add;
    for (cellId = 0; cellId < m_pSimData->cellVec.size(); cellId++) {
        nodeFlag = m_pSimData->nodeFlagVec[cellId];
        // Check neigbours of inside cells
        if (nodeFlag == 0b11111111) {
            add = false;
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    for (int k = -1; k < 2; k++) {
                        neigbourPos = m_pSimData->cellVec[cellId].pos;
                        // neighbourPos cant have any of the components equal to zero 
                        // (since the condition nodeFlag == 0b11111111) so the following 
                        // lines are ok to compute
                        neigbourPos.x += i;
                        neigbourPos.y += j;
                        neigbourPos.z += k;
                        nCellId = m_pSimData->cellIdVec[m_pCfgData->getAbsoluteCellId(neigbourPos)];
                        // If neigbour is not fully inside than the cell is stored to BVec
                        if (nCellId != Const::noCellId) {
                            if (m_pSimData->nodeFlagVec[nCellId] != 0b11111111) {
                                m_pSimData->cellIdBVec.push_back(cellId);
                                add = true;
                            }
                        }
                        // Force break from nested for loops
                        if (add) {
                            j = 2; 
                            i = 2;
                            k = 2;
                        }
                    }
                }
            }
            if (!add) m_pSimData->cellIdAVec.push_back(cellId);
        }
        else {
            m_pSimData->cellIdBVec.push_back(cellId);
        }
    }

    std::string msg = "created " + std::to_string(m_pSimData->cellVec.size()) + 
        " cells for cylindrical geometry\n";
    LOG(*m_pLogger, LogMask::Memory, msg);

    return 0;
}