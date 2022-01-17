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
    std::vector<std::string> strVec;
    getParamToVector("specie", strVec);

    m_pSimData->specieVec.resize(strVec.size());
    for (size_t i = 0; i < strVec.size(); i++) {
        m_pSimData->specieVec[i].id = i;
        m_pSimData->specieVec[i].name = strVec[i];
        getParamToValue("specie." + strVec[i] + ".statesPerCell", 
            m_pSimData->specieVec[i].statesPerCell);

    }

    for (auto& spec: m_pSimData->specieVec) {
        spec.dt = double(spec.timestepRatio*m_pCfgData->timestep);
        spec.idt = 1.0 / spec.dt;
        spec.maxVel = {
            double(m_pCfgData->cellSize.x) * spec.idt,
            double(m_pCfgData->cellSize.y) * spec.idt,
            double(m_pCfgData->cellSize.z) * spec.idt
        };
        spec.qm = spec.charge / spec.mass;
        double maxEnergy = 0.5*spec.maxVel.lenSq()*spec.mass*Const::eVJ;

        std::string msg = 
            std::string("-------------------------------------\n") +
            "specie " + spec.name + "configuration\n"
            "mass [kg]: " + std::to_string(spec.mass) + "\n" +
            "dt [s]: " + std::to_string(spec.dt) + "\n" +
            "max velocity [m/s]: [" + 
                std::to_string(spec.maxVel.x) + ", " + 
                std::to_string(spec.maxVel.y) + ", " + 
                std::to_string(spec.maxVel.z) + "]" + "\n" +
            "max energy [eV]: " + std::to_string(maxEnergy) + "\n" +
            "charge/mass ratio [C/kg]: " + std::to_string(spec.qm);
            "-------------------------------------\n";
        LOG(*m_pLogger, LogMask::Info, msg);
    }

    // Set command for creating cells
    Command *pcom;
    std::string cmdChainName = "create";
    std::string cmdName;
    // Check if creation is defined
    if (m_pCmdChainMap->find(cmdChainName) != m_pCmdChainMap->end()) {
        // Check if cell creation is defined
        cmdName = "createParticles";
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
            if (headId != Const::noStateId) {
                m_pSimData->stateVec[headId].prev =
                    stateId_t(m_pSimData->stateVec.size() - 1);
                m_pSimData->stateVec.back().next = headId;
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
    Specie* spec;
    State* state;
    stateId_t stateId;
    Vec3D<state_t> ds;

    for (size_t specId = 0; specId < m_pSimData->specieVec.size(); specId++) {
        // Define timestep and 1/timestep for particle
        spec = &m_pSimData->specieVec[specId];
        ds = {
            state_t(spec->dt*spec->maxVel.x),
            state_t(spec->dt*spec->maxVel.y),
            state_t(spec->dt*spec->maxVel.z)
        };
        // Go through full cells (no boundary conditions to wory about)
        for (cellId_t cellId = 0; cellId < m_pSimData->fullCellIdVec.size(); cellId++) {
            // Get the head state
            stateId = m_pSimData->headIdVec[specId][cellId];
            // Go through all state of the specie in one cell
            while (stateId != Const::noStateId) {
                state = &m_pSimData->stateVec[stateId];
                state->pos.x += state->pos.x*ds.x;
                state->pos.y += state->pos.y*ds.y;
                state->pos.z += state->pos.z*ds.z;
            }
        }
    }
    return 0;
}