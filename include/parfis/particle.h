#ifndef PARFIS_PARTICLE_H
#define PARFIS_PARTICLE_H

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <map>
#include <random>
#include "parfis.h"
#include "datastruct.h"

namespace parfis
{
    struct Particle : public Domain
    {
        Particle() = default;
        Particle(const std::string& dname, Logger& logger, CfgData& cfgData, SimData& simData,
            std::map<std::string, std::unique_ptr<CommandChain>>& cmdChainMap)
            : Domain(dname, logger, cfgData, simData, cmdChainMap) {};
        Particle(const Particle&) = default;
        Particle& operator=(const Particle&) = default;
        ~Particle() = default;

        int loadCfgData() override;
        int loadSimData() override;
        int createStates();
        int createStatesOfSpecie(Specie& spec);
        int pushStatesCylindrical();
        void traverseCell(State& state, Cell& cell);
        void traverseXYCell(State& state, Cell& cell);
        int reflectCylindrical(State& state, Cell& cell, Vec3D<double>& geoCenter, 
            Vec3D<state_t>& dtvmax, Vec3D<state_t>& invDtvmax, double invRadius);
        void setNewCell(State& state, stateId_t stateId, size_t headIdPos, size_t newHeadIdPos);
    };
}

#endif // PARFIS_PARTICLE_H
