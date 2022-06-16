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
    int retVal;
    std::string strTmp;
    std::vector<std::string> strVec;
    getParamToVector("specie", m_pCfgData->specieNameVec);
    m_pCfgData->gasCollisionNameVec.clear();
    for (size_t i = 0; i < m_pCfgData->specieNameVec.size(); i++) {
        getParamToVector("specie." + m_pCfgData->specieNameVec[i] + ".gasCollision", strVec);
        // If there is no parameter given, there is no data so skip the vector creation
        if (strVec.size()) {
            for (auto& colName: strVec) {
                m_pCfgData->gasCollisionNameVec.push_back(
                    m_pCfgData->specieNameVec[i] + "." + colName);
            }
        }

    }
    return 0;
}

/**
 * @brief Loads data into SimData object 
 * @details We access the SimData object through a pointer, and the data object 
 * itself is a member of the Parfis class.
 */
int parfis::Particle::loadSimData()
{
    int retVal;
    std::string strTmp;
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
        retVal = getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".velInitDist", 
            m_pSimData->specieVec[i].velInitDist);
        if (retVal) m_pSimData->specieVec[i].velInitDist = ParamDefault::velInitDist;
        retVal = getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".velInitDistMin", 
            m_pSimData->specieVec[i].velInitDistMin);
        if (retVal) m_pSimData->specieVec[i].velInitDistMin = ParamDefault::velInitDistMin;
        retVal = getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".velInitDistMax", 
            m_pSimData->specieVec[i].velInitDistMax);
        if (retVal) m_pSimData->specieVec[i].velInitDistMax = ParamDefault::velInitDistMax;
        retVal = getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".randomSeed", 
            m_pSimData->specieVec[i].randomSeed);
        if (retVal) m_pSimData->specieVec[i].randomSeed = ParamDefault::randomSeed;
        m_pSimData->randomEngineVec.push_back(randEngine_t());
    }

    // Specie calculated data
    for (auto& spec: m_pSimData->specieVec) {
        spec.mass = spec.amuMass*Const::amuKg;
        spec.charge = spec.eCharge*Const::eCharge;
        spec.dt = double(spec.timestepRatio)*m_pCfgData->timestep;
        spec.idt = 1.0 / spec.dt;
        spec.maxVel = std::min(m_pCfgData->cellSize.z, 
            std::min(m_pCfgData->cellSize.x, m_pCfgData->cellSize.y))*spec.idt;
        spec.qm = spec.charge*Const::eCharge / spec.mass;
        spec.maxEv = 0.5*spec.maxVel*spec.maxVel*spec.mass*Const::JeV;

        std::string msg = 
            "specie " + std::string(spec.name) + " configuration:\n" +
            Const::multilineSeparator + 
            "mass [kg]: " + Global::to_string(spec.mass) + "\n" +
            "dt [s]: " + Global::to_string(spec.dt) + "\n" +
            "max velocity [m/s]: " + Global::to_string(spec.maxVel) + "\n" +
            "max energy [eV]: " + Global::to_string(spec.maxEv) + "\n" +
            "charge/mass ratio [C/kg]: " + Global::to_string(spec.qm) + "\n" +
            Const::multilineSeparator;
        LOG(*m_pLogger, LogMask::Info, msg);
    }

    // Gas collision data
    for (size_t j = 0; j < m_pCfgData->gasCollisionNameVec.size(); j++) {
        std::tuple<std::string, std::string> specGasColl = 
            Global::splitDot(m_pCfgData->gasCollisionNameVec[j]);
        for (size_t i = 0; i < m_pCfgData->specieNameVec.size(); i++) {
            if (std::get<0>(specGasColl) == m_pSimData->specieVec[i].name) {
                // // When adding the first vector element add the total collision - a sum of all 
                // // other collision cross-sections
                // if (m_pSimData->specieVec[i].gasCollisionVecId.size() == 0) {
                //     m_pSimData->gasCollisionTotalVec.push_back({});
                //     uint32_t id = m_pSimData->gasCollisionTotalVec.size() - 1;
                //     m_pSimData->gasCollisionTotalVec.back().id =  id;
                //     m_pSimData->gasCollisionTotalVec.back().type = int(Collision::Total);
                //     m_pSimData->gasCollisionTotalVec.back().specieId = i;
                //     m_pSimData->gasCollisionTotalVec.back().name = nullptr;
                //     m_pSimData->gasCollisionTotalVec.back().fileName = nullptr;
                //     m_pSimData->specieVec[i].gasCollisionTotalVecId = id;
                // }
                m_pSimData->gasCollisionVec.push_back({});
                m_pSimData->gasCollisionVec[j].id = j;
                m_pSimData->specieVec[i].gasCollisionVecId.push_back(j);
                m_pSimData->gasCollisionVec[j].name = m_pCfgData->gasCollisionNameVec[j].c_str();
                // Get type of collision
                getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".gasCollision." + 
                    std::get<1>(specGasColl) + ".type", m_pSimData->gasCollisionVec[j].type);
                // Get gas id from gas name
                getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".gasCollision." + 
                    std::get<1>(specGasColl) + ".gas", strTmp);
                for (size_t k=0; k<m_pSimData->gasVec.size(); k++) {
                    if (m_pSimData->gasVec[k].name == strTmp) {
                        m_pSimData->gasCollisionVec[j].gasId = k;
                        break;
                    }
                }
                // Load data from cross section file
                getParamToValue("specie." + m_pCfgData->specieNameVec[i] + ".gasCollision." + 
                    std::get<1>(specGasColl) + ".crossSectionFile", strTmp);
                m_pCfgData->gasCollisionFileNameVec.push_back(strTmp);
                m_pSimData->gasCollisionVec[j].fileName = 
                    m_pCfgData->gasCollisionFileNameVec[j].c_str();
                // Cross section is nonlinear tabulated data
                m_pSimData->gasCollisionVec[j].xSecFtab.type = 1;
                m_pSimData->gasCollisionVec[j].xSecFtab.loadData(
                    m_pCfgData->gasCollisionFileNameVec[j]);
                m_pSimData->gasCollisionVec[j].calculateColFreq(
                    m_pSimData->specieVec[i],
                    m_pSimData->gasVec[m_pSimData->gasCollisionVec[j].gasId]);
            }
        }
    }

    m_pSimData->calculateColProb(m_pCfgData);

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
            if (m_pCfgData->geometry == 1) {
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
            if (m_pCfgData->geometry == 1) {
                pcom->m_func = [&]()->int { return pushStatesCylindrical(); };
                pcom->m_funcName = "Particle::pushStatesCylindrical";
                std::string msg = "pushStates command defined with " + pcom->m_funcName + "\n";
                LOG(*m_pLogger, LogMask::Info, msg);
            }
            if (m_pSimData->field.typeE == Vec3D<int>{0, 0, 0}) {
                stepState = 
                    [&](Specie* pSpec, State* pState) { return stepStateNoField(pSpec, pState); }; 
                std::string msg = "stepStates function defined with Particle::stepStateNoField\n";
                LOG(*m_pLogger, LogMask::Info, msg);
            }
            if (m_pSimData->field.typeE == Vec3D<int>{0, 0, 1}) {
                stepState = 
                    [&](Specie* pSpec, State* pState) { return stepStateUniformEz(pSpec, pState); }; 
                std::string msg = "stepStates function defined with Particle::stepStateNoField\n";
                LOG(*m_pLogger, LogMask::Info, msg);
            }
        }
    }

    return 0;
}

int parfis::Particle::createStates()
{
    // Initialize random engine
    // Use random_device to generate a seed for Mersenne twister engine.
    // Use Mersenne twister engine to generate pseudo-random numbers.
    for (auto& spec : m_pSimData->specieVec) {
        if (spec.randomSeed == 0) {
            std::random_device rd;
            m_pSimData->randomEngineVec[spec.id].seed(rd());
        }
        else {
            m_pSimData->randomEngineVec[spec.id].seed(spec.randomSeed);
        }
    }
    
    // Reserve space for states
    int stateSum = 0;
    for (auto& spec : m_pSimData->specieVec)
        stateSum += spec.statesPerCell;

    // Reserve space as if all states are used
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
    // Get the random engine
    randEngine_t &engine = m_pSimData->randomEngineVec[spec.id];
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
        0.5 * m_pCfgData->cellCount.x, 
        0.5 * m_pCfgData->cellCount.y,
        0.5 * m_pCfgData->cellCount.z};
    double radiusSquared = geoCenter.x*geoCenter.x; 
    double rx, ry;
    std::string msg;
    for (cellId_t ci = 0; ci < m_pSimData->cellVec.size(); ci++) {
        for (stateId_t si = 0; si < spec.statesPerCell; si++) {
            state.pos.x = dist(engine);
            state.pos.y = dist(engine);
            state.pos.z = dist(engine);
            // 0: uniform distribution
            if (spec.velInitDist == 0) {
                state.vel.x = (spec.velInitDistMax.x - spec.velInitDistMin.x)*dist(engine) + 
                    spec.velInitDistMin.x;
                state.vel.y = (spec.velInitDistMax.y - spec.velInitDistMin.y)*dist(engine) + 
                    spec.velInitDistMin.y;
                state.vel.z = (spec.velInitDistMax.z - spec.velInitDistMin.z)*dist(engine) + 
                    spec.velInitDistMin.z;
            }

            pCell = &m_pSimData->cellVec[ci];
            // If the cell is not whole in the geometry check state position, 
            // and if the state is not in the geometry, don't add it
            if (m_pSimData->nodeFlagVec[ci] != NodeFlag::InsideGeo && 
                m_pCfgData->geometry == 1) {                
                rx = state.pos.x + pCell->pos.x - geoCenter.x;
                ry = state.pos.y + pCell->pos.y - geoCenter.y;
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
                m_pSimData->stateVec[headId].prev = m_pSimData->stateVec.size() - 1;
                pState->next = headId;
            }
            // Set head pointer
            m_pSimData->headIdVec[spec.headIdOffset + ci] = m_pSimData->stateVec.size() - 1;
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
    uint16_t mark;
    // Center of the geometry
    Vec3D<double> geoCenter = {
        0.5 * m_pCfgData->cellCount.x, 
        0.5 * m_pCfgData->cellCount.y,
        0.5 * m_pCfgData->cellCount.z};
    double rx, ry;
    double radiusSquared = geoCenter.x*geoCenter.x; 
    double invRadius = 1.0 / geoCenter.x;
    // Reset pushed state vector
    std::fill(m_pSimData->stateFlagVec.begin(), m_pSimData->stateFlagVec.end(), 0);
    for (size_t specId = 0; specId < m_pSimData->specieVec.size(); specId++) {
        // Define timestep and 1/timestep for specie
        pSpec = &m_pSimData->specieVec[specId];
        // velocity change in computational units:
        // DV = (q*E*dt^2)/(m*CellLength)
        pSpec->dvUniformE.x = m_pSimData->field.strengthE.x*(pSpec->charge*Const::eCharge * 
            pSpec->timestepRatio * pSpec->timestepRatio * pSpec->dt * pSpec->dt)/
            (pSpec->amuMass*Const::amuKg * m_pCfgData->cellSize.x);

        pSpec->dvUniformE.y = m_pSimData->field.strengthE.y*(pSpec->charge*Const::eCharge * 
            pSpec->timestepRatio * pSpec->timestepRatio * pSpec->dt * pSpec->dt)/
            (pSpec->amuMass*Const::amuKg * m_pCfgData->cellSize.y);

        pSpec->dvUniformE.z = m_pSimData->field.strengthE.z*(pSpec->charge*Const::eCharge * 
            pSpec->timestepRatio * pSpec->timestepRatio * pSpec->dt * pSpec->dt)/
            (pSpec->amuMass*Const::amuKg * m_pCfgData->cellSize.z);
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
                stepState(pSpec, pState);
                newCell.pos = pCell->pos;
                traverseCell(*pState, newCell);
                m_pSimData->stateFlagVec[stateId] = StateFlag::PushedState;
                stateId = m_pSimData->stateVec[stateId].next;
                // If cell is traversed
                if (newCell.pos != pCell->pos) {
                    // newCellId must exist (no Global::noCellId, no id that doesn't exist) 
                    // so if the following line segfaults something has been faulty coded
                    newCellId = m_pSimData->cellIdVec[m_pCfgData->getAbsoluteCellId(newCell.pos)];
                    // Set states linked list for the old cell and the new cell
                    setNewCell(*pState,
                        pSpec->headIdOffset + cellId, 
                        pSpec->headIdOffset + newCellId);
                }
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
                stepState(pSpec, pState);                
                rx = pState->pos.x + pCell->pos.x - geoCenter.x;
                ry = pState->pos.y + pCell->pos.y - geoCenter.y;
                if (rx * rx + ry * ry > radiusSquared) {
                    // Return particle to position before the reflection
                    pState->pos.x -= pState->vel.x;
                    pState->pos.y -= pState->vel.y;
                    // Do the reflection from walls
                    reflectCylindrical(*pState, *pCell, geoCenter, invRadius);
                }
                newCell.pos = pCell->pos;
                traverseCell(*pState, newCell);
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
                m_pSimData->stateFlagVec[stateId] = StateFlag::PushedState;
                stateId = m_pSimData->stateVec[stateId].next;
                if (newCell.pos != pCell->pos) {
                    // newCellId must exist (no Global::noCellId, no id that doesn't exist) 
                    // so if the following line segfaults something has been faulty coded
                    newCellId = m_pSimData->cellIdVec[m_pCfgData->getAbsoluteCellId(newCell.pos)];
                    // Set states linked list for the old cell and the new cell
                    setNewCell(*pState,
                        pSpec->headIdOffset + cellId, 
                        pSpec->headIdOffset + newCellId);
                }
            }
        }
    }
    return 0;
}

void parfis::Particle::stepStateNoField(Specie *pSpec, State *pState)
{
    pState->pos.x += pState->vel.x;
    pState->pos.y += pState->vel.y;
    pState->pos.z += pState->vel.z;
}

void parfis::Particle::stepStateUniformEz(Specie *pSpec, State *pState)
{
    pState->vel.z += pSpec->dvUniformE.z;
    pState->pos.x += pState->vel.x;
    pState->pos.y += pState->vel.y;
    pState->pos.z += pState->vel.z;
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

int parfis::Particle::reflectCylindrical(State& state, Cell& cell, Vec3D<double>& geoCenter,
    double invRadius) 
{
    bool reflect = true;
    int retval = 0;
    double timeStep = 1.0, tau;
    state_t rx, ry, vx, vy;
    double a, b, c;
    double ux, uy, cr, sr;
    while (reflect) {
        // First we find point of reflection
        rx = state.pos.x + cell.pos.x - geoCenter.x;
        ry = state.pos.y + cell.pos.y - geoCenter.y;
        vx = state.vel.x;
        vy = state.vel.y;
        a = (vx * vx + vy * vy) * timeStep;
        b = 2.0*(rx * vx + ry * vy);
        c = rx * rx + ry * ry - geoCenter.x*geoCenter.x;
        tau = 0.5*(sqrt(b*b - 4.0*a*c) - b) / a;
        // Push particle to point of reflection (rx, ry)
        state.pos.x += vx * tau;
        state.pos.y += vy * tau;
        rx = state.pos.x + cell.pos.x - geoCenter.x;
        ry = state.pos.y + cell.pos.y - geoCenter.y;
        // Cosine and sine of rotation
        cr = rx * invRadius;
        sr = ry * invRadius;
        // Velocity vector before reflection in the rotated coord. sys.
        ux =  vx * cr + vy * sr;
        uy =  vy * cr - vx * sr;
        // Set velocity and position vector of the original coord. sys.
        vx = -ux * cr - uy * sr;
        vy =  uy * cr - ux * sr;
        // Push particle to final position
        timeStep -= tau;
        state.pos.x += vx * timeStep;
        state.pos.y += vy * timeStep;
        state.vel.x = vx;
        state.vel.y = vy;
        // If traectory is close to tangent - multiple reflection can happen in a single timestep
        rx = state.pos.x + cell.pos.x - geoCenter.x;
        ry = state.pos.y + cell.pos.y - geoCenter.y;
        if (rx * rx + ry * ry > geoCenter.x*geoCenter.x) {
            // Reflect back to the boundary and repeat reflection process
            state.pos.x -= vx * timeStep;
            state.pos.y -= vy * timeStep;
            retval++;
        }
        else {
            reflect = false;
        }
    }
    return retval;
}

void parfis::Particle::setNewCell(State& state, size_t headIdPos, size_t newHeadIdPos)
{
    stateId_t stateId;
    // If the state is not the head state (has prev)
    if (state.prev != Const::noStateId) {
        stateId = m_pSimData->stateVec[state.prev].next;
        // Connect prev and next from the old cell (prev->next)
        m_pSimData->stateVec[state.prev].next = state.next;
    }
    // If the state is a head state (doesn't have prev)
    else {
        stateId = m_pSimData->headIdVec[headIdPos];
        // Connect head pointer to next from the old cell (head->next)
        m_pSimData->headIdVec[headIdPos] = state.next;
    }

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