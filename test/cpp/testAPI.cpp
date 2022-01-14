#include <fstream>
#include <iostream>
#include <mutex>
#include "gtest/gtest.h"
#include "global.h"
#include "parfis.h"
#include "define.h"

/**
 * @defgroup gtestAll
 * @brief Tests for the c++ code using Googletest
 * @{
 */

/**
 * @brief Check the type of state_t
 */
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

/**
 * @brief Check timestep value
 */
TEST(api, checkTimestep) {
    uint32_t id = parfis::api::newParfis();
    std::string cfgstr = parfis::api::getConfig(id);
    cfgstr.erase(std::remove_if(cfgstr.begin(), cfgstr.end(), ::isspace), cfgstr.end());
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

/**
 * @brief Creates three new parfis objects and deletes one from the middle
 */
TEST(api, deleteParfis)
{
    int sizeBefore = parfis::api::getParfisIdVec().size();
    std::vector<uint32_t> idVec;
    idVec.push_back(parfis::api::newParfis());
    idVec.push_back(parfis::api::newParfis());
    idVec.push_back(parfis::api::newParfis());
    parfis::api::deleteParfis(1);
    ASSERT_EQ(sizeBefore + 2, parfis::api::getParfisIdVec().size());
}

/**
 * @brief Check if number of cells is calculated correctly.
 * @details Also and checks if error is returned if number of cells is greater than maximum
 */
TEST(api, checkNumberOfCalculatedCellCount) {
    uint32_t id = parfis::api::newParfis();
    int retval = 0;
    // Get number of cells in the x direction
    int cellCount_x = parfis::api::getCfgData(id)->cellCount.x;
    // Calculate number of cells from the config parameters
    double cellSize_x = parfis::api::getCfgData(id)->cellSize.x;
    double geometrySize_x = parfis::api::getCfgData(id)->geometrySize.x;
    int cellCount_x_calc = ceil(geometrySize_x/cellSize_x);
    // Check if number of cells is calculated correctly
    ASSERT_EQ(cellCount_x, cellCount_x_calc);
    // Set normal number of cells
    retval = parfis::api::setConfig(id, "system.cellSize=[1.0e-3, 1.0e-3, 1.0e-3]");
    ASSERT_EQ(retval, 0); 
    // Set over the limit number of cells
    retval = parfis::api::setConfig(id, "system.cellSize=[1.0e-6, 1.0e-6, 1.0e-6]");
    ASSERT_NE(retval, 0);
}

/**
 * @brief Create command chains and run creation chain, then check for cells
 */
TEST(api, checkNumberOfCreatedCells) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::runCommandChain(id, "create");
    ASSERT_EQ(139200, parfis::api::getSimData(id)->cellVec.size());
    ASSERT_EQ(160000, parfis::api::getSimData(id)->cellIdVec.size());
    uint64_t ptr1 = reinterpret_cast<uint64_t>(&parfis::api::getSimData(id)->cellVec[0]);
    uint64_t ptr2 = reinterpret_cast<uint64_t>(&parfis::api::getSimData(id)->cellVec[1]);
    // Check size alignment
    ASSERT_EQ(8, ptr2 - ptr1);
}
/** @} gtestAll*/