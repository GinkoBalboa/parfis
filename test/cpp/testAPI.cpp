#include <fstream>
#include <iostream>
#include <mutex>
#include "gtest/gtest.h"
#include "global.h"
#include "parfis.h"
#include "define.h"
#define _USE_MATH_DEFINES
#include <math.h>
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
    // This is the default timestep
    ASSERT_NE(cfgstr.find("system.timestep=1"), std::string::npos);
    cfgstr = parfis::api::getConfigParam(id, "system.timestep");
    // Check default as single param
    ASSERT_EQ(double(1.0), std::stod(cfgstr, nullptr));
    // This is the new timestep
    parfis::api::setConfig(id, "system.timestep=0.00112e-12");
    cfgstr = parfis::api::getConfigParam(id, "system.timestep");
    // Check as string with function that exposes Global::to_string
    ASSERT_EQ(parfis::api::toStringDouble(1.12e-15), cfgstr);
    // Check as double
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
TEST(api, calculateCellCount) {
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
    // Set over the limit number of cells
    retval = parfis::api::setConfig(id, "system.cellSize=[1.0e-6, 1.0e-6, 1.0e-6]");
    ASSERT_NE(retval, 0);
}

/**
 * @brief Create command chains and run creation chain, then check for cells
 */
TEST(api, createCells) {
    uint32_t id = parfis::api::newParfis();
    std::cout << GTEST_BOX << "log file name: " << parfis::api::getLogFileName(id) << std::endl;
    int retval = parfis::api::setConfig(id, "commandChain.create = [createCells]");
    ASSERT_EQ(retval, 0);
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    ASSERT_EQ(139200, parfis::api::getSimData(id)->cellVec.size());
    ASSERT_EQ(160000, parfis::api::getSimData(id)->cellIdVec.size());
    // No particle creation is performed because of missins createStates command
    ASSERT_EQ(0, parfis::api::getSimData(id)->stateVec.size());
}

/**
 * @brief Create command chains and run creation chain, then check for cells
 */
TEST(api, userDefinedCfgString) {
    // First take the default configuration string
    std::string defaultStr = parfis::api::defaultConfiguration();
    // Find the creation command chain
    auto startPos = defaultStr.find("commandChain.create");
    auto endPos = defaultStr.find("\n", startPos);
    // Replace the line with the create command chain so it will only create cells
    defaultStr.replace(startPos, endPos-startPos, 
        "commandChain.create = [createCells] <parfis::Command>");
    // Proceed as usual with creating the Parfis object
    uint32_t id = parfis::api::newParfis(defaultStr.c_str());
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    ASSERT_EQ(139200, parfis::api::getSimData(id)->cellVec.size());
    ASSERT_EQ(160000, parfis::api::getSimData(id)->cellIdVec.size());
    ASSERT_EQ(0, parfis::api::getSimData(id)->stateVec.size());
    uint64_t ptr1 = reinterpret_cast<uint64_t>(&parfis::api::getSimData(id)->cellVec[0]);
    uint64_t ptr2 = reinterpret_cast<uint64_t>(&parfis::api::getSimData(id)->cellVec[1]);
    // Check size alignment
    ASSERT_EQ(12, ptr2 - ptr1);
}

/**
 * @brief Config specie
 */
TEST(api, configSpecie) {
    uint32_t id = parfis::api::newParfis();
    ASSERT_EQ(1, parfis::api::getCfgData(id)->specieNameVec.size());
    ASSERT_EQ("a", parfis::api::getCfgData(id)->specieNameVec[0]);
}

/**
 * @brief Create cells and states
 */
TEST(api, createCellsAndStates) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    double rSqPi = M_PI * std::pow(parfis::api::getCfgData(id)->geometrySize.x * 0.5, 2);
    double areaCyl = rSqPi * parfis::api::getCfgData(id)->geometrySize.z;
    double areaCub = parfis::api::getCfgData(id)->geometrySize.x * 
        parfis::api::getCfgData(id)->geometrySize.y * parfis::api::getCfgData(id)->geometrySize.z;

    uint32_t numStatesCyl = parfis::api::getSimData(id)->stateVec.size();
    uint32_t numCells = parfis::api::getSimData(id)->cellIdVec.size();
    uint32_t numStatesCub = numCells*parfis::api::getSimData(id)->specieVec[0].statesPerCell;
    double areaRatio = areaCyl/areaCub;
    double stateRatio = double(numStatesCyl)/double(numStatesCub);
    double relativeDifference = abs(areaRatio - stateRatio)/(0.5*(areaRatio + stateRatio));
    double precission = 5.0e-3;
    
    // The ratio between the number of created states in cylindrical and cubical geometry 
    // should resemble the ratio of the volume between the two
    std::cout << GTEST_BOX << "created: " << numStatesCyl << " states" << std::endl;
    std::cout << GTEST_BOX << "volume ratio: " << areaRatio << std::endl;
    std::cout << GTEST_BOX << "states ratio: " << stateRatio << std::endl;
    std::cout << GTEST_BOX << "relative difference: " << 
        relativeDifference*100.0 << "%" << std::endl;
    std::cout << GTEST_BOX << "max. rel. difference: " << precission*100.0 << "%" << std::endl;

    // Relative precission is set to less than 0.5%
    ASSERT_LE(relativeDifference, precission);
}
/** @} gtestAll*/