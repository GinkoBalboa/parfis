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
        System(const std::string& dname, Logger& logger);
        System(const System&) = default;
        System& operator=(const System&) = default;
        ~System() = default;

        /// Pointer to Data structs
        CfgData* m_cfgData;

        int loadCfgData() override;
    };
}

#endif // PARFIS_SYSTEM_H
