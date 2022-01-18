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
    enum TraverseMark: uint8_t {
        NegX = 0b0000000000000001,
        PosX = 0b0000000000000010,
        NegY = 0b0000000000000100,
        PosY = 0b0000000000001000,
        NegZ = 0b0000000000010000,
        PosZ = 0b0000000000100000,
    };

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
        int moveCylindrical();
    };
}

#endif // PARFIS_PARTICLE_H
