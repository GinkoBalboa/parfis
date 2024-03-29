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
 * @defgroup gtestAll Testing
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
 * @brief Creates three new parfis objects and deletes one from the middle, then delete all
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
    parfis::api::deleteAll();
    ASSERT_EQ(0, parfis::api::getParfisIdVec().size());
}

/**
 * @brief Check if number of cells is calculated correctly.
 * @details Also and checks if error is returned if number of cells is greater than maximum
 */
TEST(api, calculateCellCount) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::loadCfgData(id);
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
    retval = parfis::api::loadCfgData(id);
    ASSERT_EQ(retval, 0);
    // Set over the limit number of cells
    retval = parfis::api::setConfig(id, "system.cellSize=[1.0e-6, 1.0e-6, 1.0e-6]");
    retval = parfis::api::loadCfgData(id);
    ASSERT_NE(retval, 0);
    parfis::api::deleteParfis(id);
}

/**
 * @brief Create command chains and run creation chain, then check for cells
 */
TEST(api, createCells) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::loadCfgData(id);
    std::cout << GTEST_BOX << "log file name: " << parfis::api::getLogFileName(id) << std::endl;
    int retval = parfis::api::setConfig(id, "commandChain.create = [createCells]");
    ASSERT_EQ(retval, 0);
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    ASSERT_EQ(139200, parfis::api::getSimData(id)->cellVec.size());
    ASSERT_EQ(160000, parfis::api::getSimData(id)->cellIdVec.size());
    // No particle creation is performed because of missins createStates command
    ASSERT_EQ(0, parfis::api::getSimData(id)->stateVec.size());
    parfis::api::deleteParfis(id);
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
    parfis::api::loadCfgData(id);
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    ASSERT_EQ(139200, parfis::api::getSimData(id)->cellVec.size());
    ASSERT_EQ(160000, parfis::api::getSimData(id)->cellIdVec.size());
    ASSERT_EQ(0, parfis::api::getSimData(id)->stateVec.size());
    uint64_t ptr1 = reinterpret_cast<uint64_t>(&parfis::api::getSimData(id)->cellVec[0]);
    uint64_t ptr2 = reinterpret_cast<uint64_t>(&parfis::api::getSimData(id)->cellVec[1]);
    // Check size alignment
    ASSERT_EQ(6, ptr2 - ptr1);
    parfis::api::deleteParfis(id);
}

/**
 * @brief Config specie
 */
TEST(api, configSpecie) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::loadCfgData(id);
    ASSERT_EQ(1, parfis::api::getCfgData(id)->specieNameVec.size());
    ASSERT_EQ("a", parfis::api::getCfgData(id)->specieNameVec[0]);
    parfis::api::deleteParfis(id);
}

/**
 * @brief Reconfigure and add specie
 */
TEST(api, reconfigSpecie) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::loadCfgData(id);
    parfis::api::setPyCfgData(id);
    ASSERT_EQ(1, parfis::api::getCfgData(id)->specieNameVec.size());
    ASSERT_EQ("a", parfis::api::getCfgData(id)->specieNameVec[0]);
    // Set two species
    parfis::api::setConfig(id, "particle.specie = [electron, atom] <parfis::Param>");
    parfis::api::setConfig(id, "particle.specie.electron = [statesPerCell, timestepRatio, amuMass, eCharge] <parfis::Param>");
    parfis::api::setConfig(id, "particle.specie.electron.statesPerCell = 7 <int>");
    parfis::api::setConfig(id, "particle.specie.electron.timestepRatio = 1 <int>");
    parfis::api::setConfig(id, "particle.specie.electron.amuMass = 0.00054858 <double>");
    parfis::api::setConfig(id, "particle.specie.electron.eCharge = -1 <int>");
    parfis::api::setConfig(id, "particle.specie.atom = [statesPerCell, timestepRatio, amuMass, eCharge] <parfis::Param>");
    parfis::api::setConfig(id, "particle.specie.atom.statesPerCell = 5 <int>");
    parfis::api::setConfig(id, "particle.specie.atom.timestepRatio = 1 <int>");
    parfis::api::setConfig(id, "particle.specie.atom.amuMass = 4 <double>");
    parfis::api::setConfig(id, "particle.specie.atom.eCharge = 0 <int>");
    parfis::api::loadCfgData(id);
    parfis::api::loadSimData(id);
    parfis::api::setPyCfgData(id);

    ASSERT_EQ(2, parfis::api::getCfgData(id)->specieNameVec.size());

    ASSERT_EQ("electron", parfis::api::getCfgData(id)->specieNameVec[0]);
    ASSERT_EQ(7, parfis::api::getSimData(id)->specieVec[0].statesPerCell);

    ASSERT_EQ("atom", parfis::api::getCfgData(id)->specieNameVec[1]);
    ASSERT_EQ(5, parfis::api::getSimData(id)->specieVec[1].statesPerCell);

    // Check as double
    ASSERT_EQ(double(4), std::stod(parfis::api::getConfigParam(id, "particle.specie.atom.amuMass"), nullptr));
    parfis::api::deleteParfis(id);
}

/**
 * @brief Reconfigure from file (root dir is parfis)
 */
TEST(api, configureSpecieFromFile) {
    uint32_t id = parfis::api::newParfis();
    int retval = parfis::api::setConfigFromFile(id, "./data/config_files/test_api_configureSpecieFromFile.ini");
    ASSERT_EQ(0, retval);
    parfis::api::loadCfgData(id);
    parfis::api::setPyCfgData(id);
    ASSERT_EQ(2, parfis::api::getCfgData(id)->specieNameVec.size());
    ASSERT_EQ("electron", parfis::api::getCfgData(id)->specieNameVec[0]);
    ASSERT_EQ("atom", parfis::api::getCfgData(id)->specieNameVec[1]);
    // Check as double
    ASSERT_EQ(double(4), std::stod(parfis::api::getConfigParam(id, "particle.specie.atom.amuMass"), nullptr));
    parfis::api::deleteParfis(id);
}

/**
 * @brief Test PyCfgData
 */
TEST(api, pyCfgData) {
    uint32_t id = parfis::api::newParfis();
    // Background gas and two species
    parfis::api::setConfig(id, "system.gas = bck <parfis::Param>");
    parfis::api::setConfig(id, "system.gas.bck = [amuMass, volumeFraction, temperature, molDensity] <parfis::Param>");
    parfis::api::setConfig(id, "system.gas.bck.amuMass = 4.0 <double>");
    parfis::api::setConfig(id, "system.gas.bck.volumeFraction = 1.0 <double>");
    parfis::api::setConfig(id, "system.gas.bck.temperature = 0.0 <double>");
    parfis::api::setConfig(id, "system.gas.bck.molDensity = 0.1660539067173 <double>");
    parfis::api::setConfig(id, "particle.specie = [electron, atom] <parfis::Param>");
    parfis::api::setConfig(id, "particle.specie.electron = [statesPerCell, timestepRatio, amuMass, eCharge] <parfis::Param>");
    parfis::api::setConfig(id, "particle.specie.electron.statesPerCell = 10 <int>");
    parfis::api::setConfig(id, "particle.specie.electron.timestepRatio = 1 <int>");
    parfis::api::setConfig(id, "particle.specie.electron.amuMass = 0.00054858 <double>");
    parfis::api::setConfig(id, "particle.specie.electron.eCharge = -1 <int>");
    parfis::api::setConfig(id, "particle.specie.atom = [statesPerCell, timestepRatio, amuMass, eCharge] <parfis::Param>");
    parfis::api::setConfig(id, "particle.specie.atom.statesPerCell = 10 <int>");
    parfis::api::setConfig(id, "particle.specie.atom.timestepRatio = 1 <int>");
    parfis::api::setConfig(id, "particle.specie.atom.amuMass = 4 <double>");
    parfis::api::setConfig(id, "particle.specie.atom.eCharge = 0 <int>");
    int retval;
    retval = parfis::api::loadCfgData(id);
    ASSERT_EQ(0, retval);
    parfis::api::setPyCfgData(id);
    ASSERT_EQ(1, parfis::api::getPyCfgData(id)->gasNameVec.size);
    ASSERT_EQ(std::string("bck"), std::string(parfis::api::getPyCfgData(id)->gasNameVec.ptr[0]));
    ASSERT_EQ(2, parfis::api::getPyCfgData(id)->specieNameVec.size);
    ASSERT_EQ(std::string("electron"), std::string(parfis::api::getPyCfgData(id)->specieNameVec.ptr[0]));
    ASSERT_EQ(std::string("atom"), std::string(parfis::api::getPyCfgData(id)->specieNameVec.ptr[1]));
    parfis::api::deleteParfis(id);
}

/**
 * @brief Create cells and states
 */
TEST(api, createCellsAndStates) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::loadCfgData(id);
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    double rSqPi = M_PI * std::pow(parfis::api::getCfgData(id)->geometrySize.x * 0.5, 2);
    double volCyl = rSqPi * parfis::api::getCfgData(id)->geometrySize.z;
    double volCub = parfis::api::getCfgData(id)->geometrySize.x * 
        parfis::api::getCfgData(id)->geometrySize.y * parfis::api::getCfgData(id)->geometrySize.z;

    uint32_t numStatesCyl = parfis::api::getSimData(id)->stateVec.size();
    uint32_t numCells = parfis::api::getSimData(id)->cellIdVec.size();
    uint32_t numStatesCub = numCells*parfis::api::getSimData(id)->specieVec[0].statesPerCell;
    double volRatio = volCyl/volCub;
    double stateRatio = double(numStatesCyl)/double(numStatesCub);
    double relativeDifference = abs(volRatio - stateRatio)/(0.5*(volRatio + stateRatio));
    double precission = 5.0e-3;
    
    // The ratio between the number of created states in cylindrical and cubical geometry 
    // should resemble the ratio of the volume between the two
    std::cout << GTEST_BOX << "created: " << numStatesCyl << " states" << std::endl;
    std::cout << GTEST_BOX << "volume ratio: " << volRatio << std::endl;
    std::cout << GTEST_BOX << "states ratio: " << stateRatio << std::endl;
    std::cout << GTEST_BOX << "relative difference: " << 
        relativeDifference*100.0 << "%" << std::endl;
    std::cout << GTEST_BOX << "max. rel. difference: " << precission*100.0 << "%" << std::endl;

    // Relative precission is set to less than 0.5%
    ASSERT_LE(relativeDifference, precission);
    parfis::api::deleteParfis(id);
}

/**
 * @brief Test PySimData
 */
TEST(api, pySimData) {
    uint32_t id = parfis::api::newParfis();
    // Set two species
    std::string cfgStr = "\n\n\
        \n\n\
        particle.specie = electron <parfis::Param>\n\
        particle.specie.electron = [statesPerCell, timestepRatio, amuMass, eCharge, velInitDist, velInitDistMin, velInitDistMax, randomSeed] <parfis::Param>\n\
        particle.specie.electron.statesPerCell = 10 <int>\n\
        particle.specie.electron.timestepRatio = 1 <int>\n\
        particle.specie.electron.amuMass = 0.00054858 <double>\n\
        particle.specie.electron.eCharge = -1 <int>\n\
        particle.specie.electron.velInitDist = 0 <int> \n\
        particle.specie.electron.velInitDistMin = [-0.5773502691, -0.5773502691, -0.5773502691] <double> \n\
        particle.specie.electron.velInitDistMax = [0.5773502691, 0.5773502691, 0.5773502691] <double> \n\
        particle.specie.electron.randomSeed = 0 <int>\n\
    ";
    parfis::api::setConfig(id, cfgStr.c_str());
    parfis::api::loadCfgData(id);
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    parfis::api::setPyCfgData(id);
    ASSERT_EQ(1, parfis::api::getPyCfgData(id)->specieNameVec.size);
    ASSERT_EQ(std::string("electron"), 
        std::string(parfis::api::getPyCfgData(id)->specieNameVec.ptr[0]));
    parfis::api::setPySimData(id);
    ASSERT_EQ(std::string("electron"), 
        std::string(parfis::api::getPySimData(id)->specieVec.ptr[0].name));
    ASSERT_EQ(0, parfis::api::getPySimData(id)->specieVec.ptr[0].velInitDist);
    ASSERT_EQ(parfis::api::getSimData(id)->stateVec.size(), 
        parfis::api::getPySimData(id)->stateVec.size);
    ASSERT_EQ(parfis::api::getSimData(id)->cellIdVec.size(), 
        parfis::api::getPySimData(id)->cellIdVec.size);
    ASSERT_EQ(parfis::api::getSimData(id)->specieVec[0].statesPerCell, 
        parfis::api::getPySimData(id)->specieVec.ptr[0].statesPerCell);
    parfis::api::deleteParfis(id);
}

/**
 * @brief Check multiple PyVec<std::strings> in a structure
 */
TEST(api, multiplePyVecStr) {
    std::string fileName = "./data/config_files/test_physics_gasCollisionDefinition.ini";
    uint32_t id = parfis::api::newParfis();
    parfis::api::setConfigFromFile(id, fileName.c_str());
    parfis::api::loadCfgData(id);
    parfis::api::loadSimData(id);
    parfis::api::setPyCfgData(id);
    const parfis::CfgData *pCfgData = parfis::api::getCfgData(id);
    const parfis::SimData *pSimData = parfis::api::getSimData(id);
    ASSERT_EQ("a", pCfgData->specieNameVec[0]);
    ASSERT_EQ("bck", pCfgData->gasNameVec[0]);
    ASSERT_EQ("a.elastic", pCfgData->gasCollisionNameVec[0]);
    ASSERT_EQ("a.inelastic", pCfgData->gasCollisionNameVec[1]);
    ASSERT_EQ("./data/cross_sections/simple_e.csv", pCfgData->gasCollisionFileNameVec[0]);
    ASSERT_EQ("./data/cross_sections/simple_i.csv", pCfgData->gasCollisionFileNameVec[1]);
    ASSERT_EQ(1, parfis::api::getPyCfgData(id)->specieNameVec.size);
    ASSERT_EQ("a", std::string(parfis::api::getPyCfgData(id)->specieNameVec.ptr[0]));
    ASSERT_EQ(1, parfis::api::getPyCfgData(id)->gasNameVec.size);
    ASSERT_EQ("bck", std::string(parfis::api::getPyCfgData(id)->gasNameVec.ptr[0]));
    ASSERT_EQ(2, parfis::api::getPyCfgData(id)->gasCollisionNameVec.size);
    ASSERT_EQ("a.elastic", std::string(parfis::api::getPyCfgData(id)->gasCollisionNameVec.ptr[0]));
    ASSERT_EQ("a.inelastic", std::string(parfis::api::getPyCfgData(id)->gasCollisionNameVec.ptr[1]));
    ASSERT_EQ(2, parfis::api::getPyCfgData(id)->gasCollisionFileNameVec.size);
    ASSERT_EQ("./data/cross_sections/simple_e.csv", 
        std::string(parfis::api::getPyCfgData(id)->gasCollisionFileNameVec.ptr[0]));
    ASSERT_EQ("./data/cross_sections/simple_i.csv", 
        std::string(parfis::api::getPyCfgData(id)->gasCollisionFileNameVec.ptr[1]));
}


/**
 * @brief Check gas definitions and particle-gas collision definitions
 */
TEST(api, gasCollisionDefinition) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::setConfigFromFile(id, "./data/config_files/test_physics_gasCollisionDefinition.ini");
    parfis::api::loadCfgData(id);
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    const parfis::CfgData *pCfgData = parfis::api::getCfgData(id);
    const parfis::SimData *pSimData = parfis::api::getSimData(id);
    ASSERT_EQ("bck", pCfgData->gasNameVec[0]);
    ASSERT_EQ(pSimData->specieVec[0].gasCollisionVecId.size(), 2);
    uint32_t id0, id1, idp;
    id0 = pSimData->specieVec[0].gasCollisionVecId[0];
    id1 = pSimData->specieVec[0].gasCollisionVecId[1];
    idp = pSimData->specieVec[0].gasCollisionProbId;
    ASSERT_EQ(pSimData->gasCollisionVec[id0].type, 0);
    ASSERT_EQ(pSimData->gasCollisionVec[id1].type, 1);
    ASSERT_EQ(std::string(pSimData->gasCollisionVec[id0].fileName), "./data/cross_sections/simple_e.csv");
    ASSERT_EQ(std::string(pSimData->gasCollisionVec[id1].fileName), "./data/cross_sections/simple_i.csv");
    std::vector<double> ranges = {1, 10, 100, 1000, 10000, 342000};
    std::vector<int> nbins = {1000, 1000, 1000, 1000, 1000, 3000};
    ASSERT_EQ(true, nbins == pSimData->gasCollisionVec[id0].xSecFtab.nbins);
    ASSERT_EQ(true, ranges == pSimData->gasCollisionVec[id0].xSecFtab.ranges);
    ASSERT_EQ(true, nbins == pSimData->gasCollisionVec[id1].xSecFtab.nbins);
    ASSERT_EQ(true, ranges == pSimData->gasCollisionVec[id1].xSecFtab.ranges);
    ASSERT_EQ(8000, pSimData->gasCollisionVec[id0].xSecFtab.xVec.size());
    ASSERT_EQ(8000, pSimData->gasCollisionVec[id0].xSecFtab.yVec.size());
    ASSERT_EQ(8000, pSimData->gasCollisionVec[id1].xSecFtab.xVec.size());
    ASSERT_EQ(8000, pSimData->gasCollisionVec[id1].xSecFtab.yVec.size());
    ASSERT_EQ(8000, pSimData->gasCollisionVec[id0].xSecFtab.colCnt);
    ASSERT_EQ(8000, pSimData->gasCollisionVec[id1].xSecFtab.colCnt);
    ASSERT_EQ(1, pSimData->gasCollisionVec[id0].xSecFtab.rowCnt);
    ASSERT_EQ(1, pSimData->gasCollisionVec[id1].xSecFtab.rowCnt);
    ASSERT_EQ(8000, pSimData->gasCollisionVec[id0].xSecFtab.colCnt);
    ASSERT_EQ(8000, pSimData->gasCollisionVec[id1].xSecFtab.colCnt);
    ASSERT_EQ(2, pSimData->gasCollisionProbVec[id0].colCnt);
    ASSERT_EQ(8000, pSimData->gasCollisionProbVec[idp].rowCnt);    
    ASSERT_EQ(8000, pSimData->gasCollisionProbVec[idp].xVec.size());
    ASSERT_EQ(2*8000, pSimData->gasCollisionProbVec[idp].yVec.size());
    parfis::api::deleteParfis(id);
}

/**
 * @brief Check PyGasCollision and PyFuncTable in a structure
 */
TEST(api, structPyFuncTable) {
    std::string fileName = "./data/config_files/test_physics_gasCollisionDefinition.ini";
    uint32_t id = parfis::api::newParfis();
    parfis::api::setConfigFromFile(id, fileName.c_str());
    parfis::api::loadCfgData(id);
    parfis::api::loadSimData(id);
    parfis::api::setPyCfgData(id);
    parfis::api::setPySimData(id);
    const parfis::CfgData *pCfgData = parfis::api::getCfgData(id);
    const parfis::SimData *pSimData = parfis::api::getSimData(id);
    ASSERT_EQ(1, parfis::api::getPyCfgData(id)->specieNameVec.size);
    ASSERT_EQ("a", std::string(parfis::api::getPyCfgData(id)->specieNameVec.ptr[0]));
    ASSERT_EQ(1, parfis::api::getPyCfgData(id)->gasNameVec.size);
    ASSERT_EQ("bck", std::string(parfis::api::getPyCfgData(id)->gasNameVec.ptr[0]));
    ASSERT_EQ(2, parfis::api::getPySimData(id)->pyGasCollisionVec.size);
    ASSERT_EQ("a.elastic", std::string(parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].name));
    ASSERT_EQ("a.inelastic", std::string(parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].name));
    ASSERT_EQ(1, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].xSecFtab.type);
    ASSERT_EQ("./data/cross_sections/simple_e.csv", 
        std::string(parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].fileName));
    ASSERT_EQ("./data/cross_sections/simple_i.csv", 
        std::string(parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].fileName));
    ASSERT_EQ(6, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].xSecFtab.nbins.size);
    ASSERT_EQ(6, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].xSecFtab.nbins.size);
    ASSERT_EQ(6, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].xSecFtab.ranges.size);
    ASSERT_EQ(6, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].xSecFtab.ranges.size);
    ASSERT_EQ(6, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].freqFtab.nbins.size);
    ASSERT_EQ(6, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].freqFtab.nbins.size);
    ASSERT_EQ(6, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].freqFtab.ranges.size);
    ASSERT_EQ(6, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].freqFtab.ranges.size);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].xSecFtab.xVec.size);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].xSecFtab.xVec.size);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].xSecFtab.yVec.size);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].xSecFtab.yVec.size);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].freqFtab.xVec.size);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].freqFtab.xVec.size);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].freqFtab.yVec.size);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].freqFtab.yVec.size);
    ASSERT_EQ(1, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].freqFtab.rowCnt);
    ASSERT_EQ(1, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].freqFtab.rowCnt);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].freqFtab.colCnt);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].freqFtab.colCnt);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].freqFtab.colCnt);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].freqFtab.colCnt);
    ASSERT_EQ(1, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].freqFtab.rowCnt);
    ASSERT_EQ(1, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].freqFtab.rowCnt);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[0].freqFtab.colCnt);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionVec.ptr[1].freqFtab.colCnt);
    ASSERT_EQ(2, parfis::api::getPySimData(id)->pyGasCollisionProbVec.ptr[0].colCnt);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionProbVec.ptr[0].rowCnt);
    ASSERT_EQ(8000, parfis::api::getPySimData(id)->pyGasCollisionProbVec.ptr[0].xVec.size);
    ASSERT_EQ(2*8000, parfis::api::getPySimData(id)->pyGasCollisionProbVec.ptr[0].yVec.size);
}
/** @} gtestAll*/
