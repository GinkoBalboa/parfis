#include <fstream>
#include <iostream>
#include <mutex>
#include "gtest/gtest.h"
#include "global.h"
#include "parfis.h"
#include "define.h"

TEST(api, checkStateType) {
    std::string infostr = parfis::api::info();
    std::string str1 = infostr.substr(0, infostr.find('\n'));
    std::string str2;
    if (sizeof(parfis::state_t) == 4)
        str2 = "parfis::state_t = float";
    else if (sizeof(parfis::state_t) == 8)
        str2 = "parfis::state_t = double";
    else 
        str2 = "parfis::state_t = unknown";
    ASSERT_EQ(str1, str2);
}

TEST(api, newParfis_checkTimestep) {
    uint32_t id = parfis::api::newParfis();
    std::string cfgstr = parfis::api::getConfig(id);
    // This is the default timestep from full configuration
    ASSERT_NE(cfgstr.find("system.timestep=1"), std::string::npos);
    cfgstr = parfis::api::getConfigParam(id, "system.timestep");
    // Check default as single param
    ASSERT_EQ(double(1.0), std::stod(cfgstr, nullptr));
    // This is the new timestep
    parfis::api::setConfig(id, "system.timestep=0.00112e-12");
    cfgstr = parfis::api::getConfigParam(id, "system.timestep");
    ASSERT_EQ(double(1.12e-15), std::stod(cfgstr, nullptr));
}

TEST(api, newParfis_numberOfCells) {
    uint32_t id = parfis::api::newParfis();
    std::string cfgstr = parfis::api::getConfig(id);
    int retval = 0;
    // Set normal number of cells
    retval = parfis::api::setConfig(id, "system.cellSize=[1.0e-3, 1.0e-3, 1.0e-3]");
    ASSERT_EQ(retval, 0); 
    // Set over the limit number of cells
    retval = parfis::api::setConfig(id, "system.cellSize=[1.0e-6, 1.0e-6, 1.0e-6]");
    ASSERT_NE(retval, 0);
}