#ifndef PARFIS_SYSTEM_H
#define PARFIS_SYSTEM_H

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <map>
#include <random>
#include "parfis.h"

namespace parfis
{
    struct System : public Domain
    {
        System() = default;
        System(Logger& logger);
        System(const System&) = default;
        System& operator=(const System&) = default;
        ~System() = default;

        /// Pointer to Data struct
        // std::shared_ptr<Data> m_pData;

        /// Overrided initialize method from Interface::initialize
        int initialize() override;

        /// Overrided initialize method from Interface::initialize
        int configure(const std::string& cstr) override;
    };
}

#endif // PARFIS_SYSTEM_H
