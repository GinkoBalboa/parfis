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
    std::string cfgstr = parfis::api::configuration(id);
    // This is the default timestep
    ASSERT_NE(cfgstr.find("system.timestep=1"), std::string::npos);
    // This is the new timestep
    parfis::api::configure(id, "system.timestep=2.0");
    cfgstr = parfis::api::configuration(id);
    ASSERT_NE(cfgstr.find("system.timestep=2"), std::string::npos);
}