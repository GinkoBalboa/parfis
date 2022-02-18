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
 * @brief Check the if particles go outside boundaries for cylindrical geometry
 */
TEST(physics, checkCylindricalReflection) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::loadCfgData(id);
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    std::cout << GTEST_BOX << "progress: "<< std::flush;
    for (uint32_t i = 0; i<100; i++) {
        if (i%5 == 0) std::cout << "|" << std::flush;
        parfis::api::runCommandChain(id, "evolve");
        // for (specieId_t specId = 0; parfis::api::getSimData(id)->specieVec.size(); specId++) {
        //     for (auto& cell : parfis::api::getSimData(id)->cellVec) {
        //         for (auto& state : parfis::api::getSimData(id)->stateVec) {
        //             // pState->pos.x + pCell->pos.x - geoCenter.x;
        //             state.pos.x + 
        //         }
        //     }
        // }
    }
    std::cout << " 100%" << std::endl;
    parfis::api::deleteParfis(id);
}


/** @} gtestAll*/