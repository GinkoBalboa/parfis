#ifndef PARFIS_SYSTEM_H
#define PARFIS_SYSTEM_H

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <map>
#include <random>
#include "parfis.h"
#include "datastruct.h"

#if defined(PARFIS_SYSTEM_INI_FILE)
#define SYSTEM_INI_FILE PARFIS_SYSTEM_INI_FILE
#else
#define SYSTEM_INI_FILE "system.ini"
#endif

namespace parfis
{
    struct System : public Domain
    {
        System() = default;
        System(const std::string& dname, Logger& logger, CfgData& cfgData, SimData& simData,
            std::map<std::string, std::unique_ptr<CommandChain>>& cmdChainMap)
            : Domain(dname, logger, cfgData, simData, cmdChainMap) {};
        System(const System&) = default;
        System& operator=(const System&) = default;
        ~System() = default;

        int loadCfgData() override;
        int createCellsCylindrical();
    };
}

#endif // PARFIS_SYSTEM_H
