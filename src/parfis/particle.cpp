#include <fstream>
#include <random>
#include "datastruct.h"
#include "particle.h"
#include "global.h"

/**
 * @brief Loads data into CfgData object 
 * @details We access the CfgData object through a pointer, and the data object itself is a 
 * member of the Parfis class.
 */
int parfis::Particle::loadCfgData()
{
    std::string str;
    getParamToVector("specie", m_pCfgData->specieNameVec);

    return 0;
}

int parfis::Particle::loadSimData()
{
    m_pSimData->specieVec.resize(m_pCfgData->specieNameVec.size());
    for (size_t i = 0; i < m_pCfgData->specieNameVec.size(); i++) {
        m_pSimData->specieVec[i].id = i;
        m_pSimData->specieVec[i].name = m_pCfgData->specieNameVec[i];
        getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".statesPerCell", 
            m_pSimData->specieVec[i].statesPerCell);
        getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".timestepRatio", 
            m_pSimData->specieVec[i].timestepRatio);
        getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".amuMass", 
            m_pSimData->specieVec[i].amuMass);
        getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".eCharge", 
            m_pSimData->specieVec[i].eCharge);
    }

    for (auto& spec: m_pSimData->specieVec) {
        spec.mass = spec.amuMass*Const::amuKg;
        spec.charge = spec.eCharge*Const::eCharge;
        spec.dt = double(spec.timestepRatio)*m_pCfgData->timestep;
        spec.idt = 1.0 / spec.dt;
        spec.maxVel = {
            m_pCfgData->cellSize.x * spec.idt,
            m_pCfgData->cellSize.y * spec.idt,
            m_pCfgData->cellSize.z * spec.idt
        };
        spec.qm = spec.charge*Const::eCharge / spec.mass;
        double maxEnergy = 0.5*spec.maxVel.lenSq()*spec.mass*Const::eVJ;

        std::string msg = 
            "specie " + spec.name + " configuration:\n" +
            Const::multilineSeparator + 
            "mass [kg]: " + Global::to_string(spec.mass) + "\n" +
            "dt [s]: " + Global::to_string(spec.dt) + "\n" +
            "max velocity [m/s]: [" + 
                Global::to_string(spec.maxVel.x) + ", " + 
                Global::to_string(spec.maxVel.y) + ", " + 
                Global::to_string(spec.maxVel.z) + "]" + "\n" +
            "max energy [eV]: " + Global::to_string(maxEnergy) + "\n" +
            "charge/mass ratio [C/kg]: " + Global::to_string(spec.qm) + "\n" +
            Const::multilineSeparator;
        LOG(*m_pLogger, LogMask::Info, msg);
    }

    // Set command for creating cells
    Command *pcom;
    std::string cmdChainName = "create";
    std::string cmdName;
    // Check if creation is defined
    if (m_pCmdChainMap->find(cmdChainName) != m_pCmdChainMap->end()) {
        // Check if cell creation is defined
        cmdName = "createStates";
        if (m_pCmdChainMap->at(cmdChainName)->m_cmdMap.find(cmdName) != 
            m_pCmdChainMap->at(cmdChainName)->m_cmdMap.end()) {
            pcom = m_pCmdChainMap->at(cmdChainName)->m_cmdMap[cmdName].get();
            // Do this differently for different geometries
            if (m_pCfgData->geometry == "cylindrical") {
                // Instead of lambda expression here, we can also use 
                // pcom->m_func = std::bind(&System::createCellsCylindrical, this);
                pcom->m_func = [&]()->int { return createStates(); };
                pcom->m_funcName = "Particle::createStates";
                std::string msg = "createStates command defined with " + pcom->m_funcName + "\n";
                LOG(*m_pLogger, LogMask::Info, msg);
            }
        }
    }
    return 0;
}

int parfis::Particle::createStates()
{
    // Reserve space for states
    int stateSum = 0;
    for (auto& spec : m_pSimData->specieVec)
        stateSum += spec.statesPerCell;

    // Reserve space as if all states 
    m_pSimData->stateVec.reserve(stateSum * m_pSimData->cellVec.size());
    std::string msg = "reserved " + std::to_string(stateSum * m_pSimData->cellVec.size()) + 
        " states for all species" + "\n";
    LOG(*m_pLogger, LogMask::Memory, msg);
    m_pSimData->headIdVec.resize(m_pSimData->specieVec.size());
    for (auto& spec : m_pSimData->specieVec) {
        m_pSimData->headIdVec[spec.id].resize(m_pSimData->cellVec.size(), Const::noStateId);
        msg = "created " + std::to_string(m_pSimData->cellVec.size()) + 
            " head pointers for specie " + spec.name + "\n";
        LOG(*m_pLogger, LogMask::Memory, msg);    
        createStatesOfSpecie(spec);
    }
    return 0;
}

int parfis::Particle::createStatesOfSpecie(Specie& spec)
{
    // Use random_device to generate a seed for Mersenne twister engine.
    std::random_device rd{};
    // Use Mersenne twister engine to generate pseudo-random numbers.
    std::mt19937 engine{ rd() };
    // The range is[inclusive, inclusive].
    std::uniform_real_distribution<state_t> dist{ 0.0, 1.0 };
    State state;
    State* pState;
    Cell* pCell;
    stateId_t headId;
    // Set count to zero
    spec.stateCount = 0;
    double uCellSizeX = m_pCfgData->cellSize.x;
    double uCellSizeY = m_pCfgData->cellSize.y;
    // Center of the geometry
    Vec3D<double> geoCenter = {
        0.5 * m_pCfgData->geometrySize.x, 
        0.5 * m_pCfgData->geometrySize.y,
        0.5 * m_pCfgData->geometrySize.z};
    double radiusSquared = geoCenter.x*geoCenter.x; 
    double rx, ry;
    for (cellId_t ci = 0; ci < m_pSimData->cellVec.size(); ci++) {
        for (stateId_t si = 0; si < spec.statesPerCell; si++) {
            state.pos.x = dist(engine);
            state.pos.y = dist(engine);
            state.pos.z = dist(engine);
            state.vel.x = 2.0*dist(engine) - 1.0;
            state.vel.y = 2.0*dist(engine) - 1.0;
            state.vel.z = 2.0*dist(engine) - 1.0;
            pCell = &m_pSimData->cellVec[ci];
            // If the cell is not whole in the geometry check state position, 
            // and if the state is not in the geometry, don't add it
            if (pCell->nodeMask != 0xFF && m_pCfgData->geometry == "cylindrical") {                
                rx = uCellSizeX*(state.pos.x + pCell->pos.x) - geoCenter.x;
                ry = uCellSizeY*(state.pos.y + pCell->pos.y) - geoCenter.y;
                if (rx * rx + ry * ry > radiusSquared)
                    continue;
            }
            // Add states in list
            m_pSimData->stateVec.push_back(state);
            pState = &m_pSimData->stateVec.back();
            pState->next = Const::noStateId;
            pState->prev = Const::noStateId;
            headId = m_pSimData->headIdVec[spec.id][ci];
            // If it is not first state in the cell
            if (headId != Const::noStateId) {double uCellSizeX = m_pCfgData->cellSize.x;
    double uCellSizeY = m_pCfgData->cellSize.y;
            }
            // Set head pointer
            m_pSimData->headIdVec[spec.id][ci] = stateId_t(m_pSimData->stateVec.size() - 1);
            spec.stateCount++;
        }
    }

    std::string msg = "created " + std::to_string(spec.stateCount) + " states for specie " +
        spec.name + "\n";
    LOG(*m_pLogger, LogMask::Memory, msg);

    return 0;
}


int parfis::Particle::moveCylindrical()
{
    Specie *pSpec;
    State *pState;
    Cell *pCell, *pNewCell;
    Cell newCell;
    cellId_t newCellId;
    stateId_t stateId;
    Vec3D<state_t> dtvmax;
    Vec3D<cellPos_t> newCellPos;
    uint16_t mark;
    // Center of the geometry
    Vec3D<double> geoCenter = {
        0.5 * m_pCfgData->geometrySize.x, 
        0.5 * m_pCfgData->geometrySize.y,
        0.5 * m_pCfgData->geometrySize.z};
    double radiusSquared = geoCenter.x*geoCenter.x; 
    double uCellSizeX = m_pCfgData->cellSize.x;
    double uCellSizeY = m_pCfgData->cellSize.y;
    double rx, ry;

    for (size_t specId = 0; specId < m_pSimData->specieVec.size(); specId++) {
        // Define timestep and 1/timestep for particle
        pSpec = &m_pSimData->specieVec[specId];
        dtvmax = {
            state_t(pSpec->dt*pSpec->maxVel.x),
            state_t(pSpec->dt*pSpec->maxVel.y),
            state_t(pSpec->dt*pSpec->maxVel.z)
        };
        // Go through full cells (no boundary conditions to wory about)
        for (cellId_t cellId = 0; cellId < m_pSimData->fullCellIdVec.size(); cellId++) {
            // New position for traversing cells
            pCell = &m_pSimData->cellVec[cellId];
            newCellPos = pCell->pos;
            // Get the head state
            stateId = m_pSimData->headIdVec[specId][cellId];
            // Go through all states of the specie in one cell
            while (stateId != Const::noStateId) {
                pState = &m_pSimData->stateVec[stateId];
                pState->pos.x += pState->vel.x * dtvmax.x;
                pState->pos.y += pState->vel.y * dtvmax.y;
                pState->pos.z += pState->vel.z * dtvmax.z;
            }
            // Mark crossing cell boundaries
            if (pState->pos.x < 0.0) {
                pState->pos.x += 1.0;
                newCell.pos.x -= 1;
            }
            else if (pState->pos.x > 1.0) {
                pState->pos.x -= 1.0;
                newCell.pos.x += 1;
            }
            if (pState->pos.y < 0.0) {
                pState->pos.y += 1.0;
                newCell.pos.y -= 1;
            }
            else if (pState->pos.y > 1.0) {
                pState->pos.y -= 1.0;
                newCell.pos.y += 1;
            }
            if (pState->pos.z < 0.0) {
                pState->pos.z += 1.0;
                newCell.pos.z -= 1;
            }
            else if (pState->pos.z > 1.0) {
                pState->pos.z -= 1.0;
                newCell.pos.z += 1;
            }
            // If cell is traversed
            if (newCell.pos != pCell->pos) {
                newCellId = m_pCfgData->getAbsoluteCellId(newCellPos);
                // newCellId mustn't be Global::noCellId by definition, so if the following
                // line segfaults something has been faulty coded
                pNewCell = &m_pSimData->cellVec[newCellId];
                // Check if maybe state hit a boundary in the new cell
                if (pNewCell->nodeMask != 0xFF) {
                    rx = uCellSizeX*(pState->pos.x + pNewCell->pos.x) - geoCenter.x;
                    ry = uCellSizeY*(pState->pos.y + pNewCell->pos.y) - geoCenter.y;
                    if (rx * rx + ry * ry > radiusSquared)
                    continue;
                }
            }
        }
    }
    return 0;
}