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
        int createStates();
        int createStatesOfSpecie(Specie& spec);
        int moveCylindrical();
    };
}

#endif // PARFIS_PARTICLE_H
