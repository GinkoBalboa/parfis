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
        m_pSimData->specieVec[i].name = m_pCfgData->specieNameVec[i].c_str();
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
            "specie " + std::string(spec.name) + " configuration:\n" +
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

    // Set command for creating states
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
    // Command chain for pushing states
    cmdChainName = "evolve";
    if (m_pCmdChainMap->find(cmdChainName) != m_pCmdChainMap->end()) {
        cmdName = "pushStates";
        if (m_pCmdChainMap->at(cmdChainName)->m_cmdMap.find(cmdName) != 
            m_pCmdChainMap->at(cmdChainName)->m_cmdMap.end()) {
            pcom = m_pCmdChainMap->at(cmdChainName)->m_cmdMap[cmdName].get();
            // Do this differently for different geometries
            if (m_pCfgData->geometry == "cylindrical") {
                pcom->m_func = [&]()->int { return pushStatesCylindrical(); };
                pcom->m_funcName = "Particle::pushStatesCylindrical";
                std::string msg = "pushStates command defined with " + pcom->m_funcName + "\n";
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
    m_pSimData->stateFlagVec.reserve(stateSum * m_pSimData->cellVec.size());
    std::string msg = "reserved " + std::to_string(stateSum * m_pSimData->cellVec.size()) + 
        " states for all species" + "\n";
    LOG(*m_pLogger, LogMask::Memory, msg);
    // Resize headIdVec
    m_pSimData->headIdVec.resize(
        m_pSimData->specieVec.size()*m_pSimData->cellVec.size(), Const::noStateId);
    msg = "created " + std::to_string(m_pSimData->headIdVec.size()) + " head pointers\n";
    LOG(*m_pLogger, LogMask::Memory, msg);    
    for (auto& spec : m_pSimData->specieVec) {
        spec.headIdOffset = spec.id*m_pSimData->cellVec.size();
        createStatesOfSpecie(spec);
    }
    return 0;
}

int parfis::Particle::createStatesOfSpecie(Specie& spec)
{
    // Use random_device to generate a seed for Mersenne twister engine.
    std::random_device rd{};
    // Use Mersenne twister engine to generate pseudo-random numbers.
    // std::mt19937 engine{ rd() };
    std::mt19937 engine;
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
    std::string msg;
    for (cellId_t ci = 0; ci < m_pSimData->cellVec.size(); ci++) {
        for (stateId_t si = 0; si < spec.statesPerCell; si++) {
            state.pos.x = dist(engine);
            state.pos.y = dist(engine);
            state.pos.z = dist(engine);
            state.vel.x = 2.0*dist(engine) - 1.0;
            state.vel.y = 2.0*dist(engine) - 1.0;
            state.vel.z = 2.0*dist(engine) - 1.0;
            if (ci==0 && si==0) {
                msg = "the first three position vector components: " + 
                std::to_string(state.pos.x) + ", " + std::to_string(state.pos.y) + ", " + 
                std::to_string(state.pos.z) + "\n";
                LOG(*m_pLogger, LogMask::Info, msg);
            }

            pCell = &m_pSimData->cellVec[ci];
            // If the cell is not whole in the geometry check state position, 
            // and if the state is not in the geometry, don't add it
            if (m_pSimData->nodeFlagVec[ci] != NodeFlag::InsideGeo && 
                m_pCfgData->geometry == "cylindrical") {                
                rx = uCellSizeX*(state.pos.x + pCell->pos.x) - geoCenter.x;
                ry = uCellSizeY*(state.pos.y + pCell->pos.y) - geoCenter.y;
                if (rx * rx + ry * ry > radiusSquared)
                    continue;
            }
            // Add states in list
            m_pSimData->stateVec.push_back(state);
            m_pSimData->stateFlagVec.push_back(StateFlag::None);
            pState = &m_pSimData->stateVec.back();
            pState->next = Const::noStateId;
            pState->prev = Const::noStateId;
            headId = m_pSimData->headIdVec[spec.headIdOffset + ci];
            // If it is not first state in the cell
            if (headId != Const::noStateId) {
                m_pSimData->stateVec[headId].prev = size_t(m_pSimData->stateVec.size() - 1);
                m_pSimData->stateVec.back().next = headId;
            }
            // Set head pointer
            m_pSimData->headIdVec[spec.headIdOffset + ci] = size_t(m_pSimData->stateVec.size() - 1);
            spec.stateCount++;
        }
    }

    msg = "created " + std::to_string(spec.stateCount) + " states for specie " +
        spec.name + "\n";
    LOG(*m_pLogger, LogMask::Memory, msg);

    return 0;
}


int parfis::Particle::pushStatesCylindrical()
{
    Specie *pSpec;
    State *pState;
    Cell *pCell;
    Cell newCell;
    cellId_t cellId, newCellId;
    stateId_t stateId;
    Vec3D<state_t> dtvmax;
    Vec3D<state_t> invDtvmax;
    uint16_t mark;
    // Center of the geometry
    Vec3D<double> geoCenter = {
        0.5 * m_pCfgData->geometrySize.x, 
        0.5 * m_pCfgData->geometrySize.y,
        0.5 * m_pCfgData->geometrySize.z};
    double rx, ry;
    double radiusSquared = geoCenter.x*geoCenter.x; 
    double invRadius = 1.0 / geoCenter.x;
    // Reset pushed state vector
    std::fill(m_pSimData->stateFlagVec.begin(), m_pSimData->stateFlagVec.end(), 0);
    for (specieId_t specId = 0; specId < m_pSimData->specieVec.size(); specId++) {
        // Define timestep and 1/timestep for specie
        pSpec = &m_pSimData->specieVec[specId];
        dtvmax = {
            state_t(pSpec->dt/**pSpec->maxVel.x*/),
            state_t(pSpec->dt/**pSpec->maxVel.y*/),
            state_t(pSpec->dt/**pSpec->maxVel.z*/)
        };
        invDtvmax = {
            state_t(1.0/(pSpec->dt/**pSpec->maxVel.x*/)),
            state_t(1.0/(pSpec->dt/**pSpec->maxVel.y*/)),
            state_t(1.0/(pSpec->dt/**pSpec->maxVel.z*/))
        };
        // Go through cells that lie inside the geo
        for (cellId_t cellId : m_pSimData->cellIdAVec) {
            // New position for traversing cells
            pCell = &m_pSimData->cellVec[cellId];
            // Get the head state
            stateId = m_pSimData->headIdVec[pSpec->headIdOffset + cellId];
            // Go through all states of the specie in one cell
            while (stateId != Const::noStateId) {
                // If it was pushed previously - just continue
                if (m_pSimData->stateFlagVec[stateId] == StateFlag::PushedState) {
                    stateId = m_pSimData->stateVec[stateId].next;
                    continue;
                }
                pState = &m_pSimData->stateVec[stateId];
                pState->pos.x += pState->vel.x * dtvmax.x;
                pState->pos.y += pState->vel.y * dtvmax.y;
                pState->pos.z += pState->vel.z * dtvmax.z;
                newCell.pos = pCell->pos;
                traverseCell(*pState, newCell);
                // If cell is traversed
                if (newCell.pos != pCell->pos) {
                    // newCellId must exist (no Global::noCellId, no id that doesn't exist) 
                    // so if the following line segfaults something has been faulty coded
                    newCellId = m_pSimData->cellIdVec[m_pCfgData->getAbsoluteCellId(newCell.pos)];
                    if (m_pSimData->nodeFlagVec[newCellId] != NodeFlag::InsideGeo &&
                        (m_pSimData->nodeFlagVec[newCellId] != NodeFlag::NegZBound ||
                         m_pSimData->nodeFlagVec[newCellId] != NodeFlag::PosZBound)) {
                        rx = m_pCfgData->cellSize.x*(pState->pos.x + newCell.pos.x) - geoCenter.x;
                        ry = m_pCfgData->cellSize.y*(pState->pos.y + newCell.pos.y) - geoCenter.y;
                        if (rx * rx + ry * ry > radiusSquared) {
                            // Return particle to position before the reflection
                            pState->pos.x -= pState->vel.x * dtvmax.x;
                            pState->pos.y -= pState->vel.y * dtvmax.y;
                            // Do the reflection from walls
                            reflectCylindrical(
                                *pState, *pCell, geoCenter, dtvmax, invDtvmax, invRadius);
                            newCell.pos.x = pCell->pos.x;
                            newCell.pos.y = pCell->pos.y;
                            // Mark new cell traverse (it can happen after reflection)
                            traverseXYCell(*pState, newCell);
                        }
                    }
                    // Now check the z-boundary
                    if (newCell.pos.z == m_pCfgData->cellCount.z) {
                        // Periodic boundary - torus like geometry
                        if (m_pCfgData->periodicBoundary.z) {
                            newCell.pos.z = 0;
                        }
                        // Reflection from z-bound
                        else {
                            pState->pos.z  = 1.0 - pState->pos.z;
                            pState->vel.z *= -1.0;
                            newCell.pos.z = m_pCfgData->cellCount.z - 1;
                        }
                    }
                    else if (newCell.pos.z == 0xFFFF) {
                        // Periodic boundary - torus like geometry
                        if (m_pCfgData->periodicBoundary.z) {
                            newCell.pos.z = m_pCfgData->cellCount.z - 1;
                        }
                        // Reflection from z-bound
                        else {
                            pState->pos.z = 1.0 - pState->pos.z;
                            pState->vel.z *= -1.0;
                            newCell.pos.z = 0;
                        }
                    }
                    // Set states linked list for the old cell and the new cell
                    setNewCell(*pState, stateId,
                        pSpec->headIdOffset + cellId, 
                        pSpec->headIdOffset + newCellId);
                }
                m_pSimData->stateFlagVec[stateId] = StateFlag::PushedState;
                stateId = m_pSimData->stateVec[stateId].next;
            }
        }
        // Go through bound cells (check boundary crossing every time)
        for (cellId_t cellId : m_pSimData->cellIdBVec) {
            // New position for traversing cells
            pCell = &m_pSimData->cellVec[cellId];
            // Get the head state
            stateId = m_pSimData->headIdVec[pSpec->headIdOffset + cellId];
            // Go through all states of the specie in one cell
            while (stateId != Const::noStateId) {
                // If it was pushed previously - just continue
                if (m_pSimData->stateFlagVec[stateId] == StateFlag::PushedState) {
                    stateId = m_pSimData->stateVec[stateId].next;
                    continue;
                }
                pState = &m_pSimData->stateVec[stateId];
                pState->pos.x += pState->vel.x * dtvmax.x;
                pState->pos.y += pState->vel.y * dtvmax.y;
                pState->pos.z += pState->vel.z * dtvmax.z;
                newCell = *pCell;
                traverseCell(*pState, newCell);
                // if state got out of the geometry then reflect/re-emit it, state can't
                // traverse more than one cell so we check if it is in the cell that is the
                // first one outside the geo                    
                if (newCell.pos.x == m_pCfgData->cellCount.x || newCell.pos.x == 0xFFFF ||
                    newCell.pos.y == m_pCfgData->cellCount.y || newCell.pos.y == 0xFFFF) {
                    // Return particles back to position before the reflection
                    pState->pos.x -= pState->vel.x * dtvmax.x;
                    pState->pos.y -= pState->vel.y * dtvmax.y;
                    // Do the reflection from walls
                    reflectCylindrical(
                        *pState, *pCell, geoCenter, dtvmax, invDtvmax, invRadius);
                    newCell.pos.x = pCell->pos.x;
                    newCell.pos.y = pCell->pos.y;
                    traverseXYCell(*pState, newCell);
                }
                else {
                    rx = m_pCfgData->cellSize.x*(pState->pos.x + newCell.pos.x) - geoCenter.x;
                    ry = m_pCfgData->cellSize.y*(pState->pos.y + newCell.pos.y) - geoCenter.y;
                    if (rx * rx + ry * ry > radiusSquared) {
                        // Return particle to position before the reflection
                        pState->pos.x -= pState->vel.x * dtvmax.x;
                        pState->pos.y -= pState->vel.y * dtvmax.y;
                        // Do the reflection from walls
                        reflectCylindrical(
                            *pState, *pCell, geoCenter, dtvmax, invDtvmax, invRadius);
                        newCell.pos.x = pCell->pos.x;
                        newCell.pos.y = pCell->pos.y;
                        // Mark new cell traverse (it can happen after reflection)
                        traverseXYCell(*pState, newCell);
                    }
                }
                // Now check the z-boundary
                if (newCell.pos.z == m_pCfgData->cellCount.z) {
                    // Periodic boundary - torus like geometry
                    if (m_pCfgData->periodicBoundary.z) {
                        newCell.pos.z = 0;
                    }
                    // Reflection from z-bound
                    else {
                        pState->pos.z  = 1.0 - pState->pos.z;
                        pState->vel.z *= -1.0;
                        newCell.pos.z = m_pCfgData->cellCount.z - 1;
                    }
                }
                else if (newCell.pos.z == 0xFFFF) {
                    // Periodic boundary - torus like geometry
                    if (m_pCfgData->periodicBoundary.z) {
                        newCell.pos.z = m_pCfgData->cellCount.z - 1;
                    }
                    // Reflection from z-bound
                    else {
                        pState->pos.z = 1.0 - pState->pos.z;
                        pState->vel.z *= -1.0;
                        newCell.pos.z = 0;
                    }
                }
                if (newCell.pos != pCell->pos) {
                    // newCellId must exist (no Global::noCellId, no id that doesn't exist) 
                    // so if the following line segfaults something has been faulty coded
                    newCellId = m_pSimData->cellIdVec[
                        m_pCfgData->getAbsoluteCellId(newCell.pos)];
                    // Set states linked list for the old cell and the new cell
                    setNewCell(*pState, stateId,
                        pSpec->headIdOffset + cellId, 
                        pSpec->headIdOffset + newCellId);
                }
                m_pSimData->stateFlagVec[stateId] = StateFlag::PushedState;
                stateId = m_pSimData->stateVec[stateId].next;
            }
        }
    }
    return 0;
}

void parfis::Particle::traverseCell(State& state, Cell& newCell)
{
    // Mark crossing of cell boundaries
    if (state.pos.x < 0.0) {
        state.pos.x += 1.0;
        newCell.pos.x -= 1;
    }
    else if (state.pos.x > 1.0) {
        state.pos.x -= 1.0;
        newCell.pos.x += 1;
    }
    if (state.pos.y < 0.0) {
        state.pos.y += 1.0;
        newCell.pos.y -= 1;
    }
    else if (state.pos.y > 1.0) {
        state.pos.y -= 1.0;
        newCell.pos.y += 1;
    }
    if (state.pos.z < 0.0) {
        state.pos.z += 1.0;
        newCell.pos.z -= 1;
    }
    else if (state.pos.z > 1.0) {
        state.pos.z -= 1.0;
        newCell.pos.z += 1;
    }
}

void parfis::Particle::traverseXYCell(State& state, Cell& newCell)
{
    // Mark crossing cell boundaries
    if (state.pos.x < 0.0) {
        state.pos.x += 1.0;
        newCell.pos.x -= 1;
    }
    else if (state.pos.x > 1.0) {
        state.pos.x -= 1.0;
        newCell.pos.x += 1;
    }
    if (state.pos.y < 0.0) {
        state.pos.y += 1.0;
        newCell.pos.y -= 1;
    }
    else if (state.pos.y > 1.0) {
        state.pos.y -= 1.0;
        newCell.pos.y += 1;
    }
}

int parfis::Particle::reflectCylindrical(State& state, Cell& cell, Vec3D<double>& geoCenter, 
    Vec3D<state_t>& dtvmax, Vec3D<state_t>& invDtvmax, double invRadius) 
{
    bool reflect = true;
    int retval = 0;
    while (reflect) {
        // First we find point of reflection
        double rx = m_pCfgData->cellSize.x*(state.pos.x + cell.pos.x) - geoCenter.x;
        double ry = m_pCfgData->cellSize.y*(state.pos.y + cell.pos.y) - geoCenter.y;
        double vx = state.vel.x*dtvmax.x;
        double vy = state.vel.y*dtvmax.y;
        double a = 2.0*(vx * vx + vy * vy);
        double b = 2.0*(rx * vx + ry * vy);
        double c = rx * rx + ry * ry - geoCenter.x*geoCenter.x;
        double delt = (sqrt(b*b - 2.0*a*c) - b) / a;
        // Push particle to point of reflection (rx, ry)
        state.pos.x += vx * delt;
        state.pos.y += vy * delt;
        rx = m_pCfgData->cellSize.x*(state.pos.x + cell.pos.x) - geoCenter.x;
        ry = m_pCfgData->cellSize.y*(state.pos.y + cell.pos.y) - geoCenter.y;
        // Cosine and sine of rotation
        double cr = rx * invRadius;
        double sr = ry * invRadius;
        // Reflected velocity vector in the rotated coord. sys.
        double ux = -vx * cr - vy * sr;
        double uy = vy * cr - vx * sr;
        // Set velocity and position vector of the rotated coord. sys.
        vx = ux * cr - uy * sr;
        vy = uy * cr + ux * sr;
        // Push particle to final position
        delt = 1.0 - delt;
        state.pos.x += vx * delt;
        state.pos.y += vy * delt;
        state.vel.x = vx * invDtvmax.x;
        state.vel.y = vy * invDtvmax.y;
        // If traectory is close to tangent - multiple reflection can happen in a single timestep
        rx = m_pCfgData->cellSize.x*(state.pos.x + cell.pos.x) - geoCenter.x;
        ry = m_pCfgData->cellSize.y*(state.pos.y + cell.pos.y) - geoCenter.y;
        if (rx * rx + ry * ry > geoCenter.x*geoCenter.x) {
            // Reflect back to the boundary and repeat reflection process
            state.pos.x -= vx * delt;
            state.pos.y -= vy * delt;
            retval++;
        }
        else {
            reflect = false;
        }
    }
    return retval;
}

void parfis::Particle::setNewCell(State& state, stateId_t stateId, 
    size_t headIdPos, size_t newHeadIdPos)
{
    // If the state is not the head state (has prev)
    if (state.prev != Const::noStateId)
        // Connect prev and next from the old cell (prev->next)
        m_pSimData->stateVec[state.prev].next = state.next;
    // If the state is a head state (doesn't have prev)
    else
        // Connect head pointer to next from the old cell (head->next)
        m_pSimData->headIdVec[headIdPos] = state.next;

    // If the state is not the last state (has next)
    if (state.next != Const::noStateId)
        // Connect prev and next from the old cell (prev<-next)
        m_pSimData->stateVec[state.next].prev = state.prev;

    // Set new cell values for the state (state becomes head in the new cell)
    state.prev = Const::noStateId;
    state.next = m_pSimData->headIdVec[newHeadIdPos];
    m_pSimData->headIdVec[newHeadIdPos] = stateId;

    // If there was a head before (in the new cell) then set its prev pointer to the new head
    if (state.next != Const::noStateId)
        m_pSimData->stateVec[state.next].prev = stateId;
}