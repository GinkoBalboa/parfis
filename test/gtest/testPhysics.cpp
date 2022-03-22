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
    int error = 0;
    const parfis::Cell *pCell;
    const parfis::SimData *pSimData;
    parfis::stateId_t stateId;
    const parfis::State *pState;
    parfis::state_t rx, ry, rz;
    double radiusSquared = std::pow(parfis::api::getCfgData(id)->cellCount.x * 0.5, 2);
    double zMax = parfis::api::getCfgData(id)->cellCount.z;
    parfis::Vec3D<double> geoCenter = {
        0.5 * parfis::api::getCfgData(id)->cellCount.x, 
        0.5 * parfis::api::getCfgData(id)->cellCount.y,
        0.5 * parfis::api::getCfgData(id)->cellCount.z};
    for (uint32_t i = 0; i<100; i++) {
        if (i%5 == 0) std::cout << "|" << std::flush;
        parfis::api::runCommandChain(id, "evolve");
        pSimData = parfis::api::getSimData(id);
        for (auto& spec : pSimData->specieVec) {
            for (auto cellId : pSimData->cellIdBVec) {
                pCell = &pSimData->cellVec[cellId];
                stateId = pSimData->headIdVec[spec.headIdOffset + cellId];
                while (stateId != 0xFFFFFFFF) {
                    pState = &pSimData->stateVec[stateId];
                    if (pState->pos.x > 1.0 || pState->pos.x < 0.0)
                        error = 1;
                    if (pState->pos.y > 1.0 || pState->pos.y < 0.0)
                        error = 2;
                    if (pState->pos.z > 1.0 || pState->pos.z < 0.0)
                        error = 3;
                    if (pState->pos.y > 1.0 || pState->pos.y < 0.0)
                        error = 4;
                    if (pState->pos.z > 1.0 || pState->pos.z < 0.0)
                        error = 5;
                    if (pState->vel.x*pState->vel.x + 
                        pState->vel.y*pState->vel.y + 
                        pState->vel.z*pState->vel.z > 1.0)
                        error = 6;
                    rx = pState->pos.x + pCell->pos.x - geoCenter.x;
                    ry = pState->pos.y + pCell->pos.y - geoCenter.y;
                    rz = pState->pos.z + pCell->pos.z;
                    if (rx*rx + ry*ry > radiusSquared)
                        error = 7;                    
                    if (rz > zMax || rz < 0.0) 
                        error = 8;
                    if (error) break;
                    stateId = pSimData->stateVec[stateId].next;
                }
                if (error) break;
            }
            if (error) break;
        }
        if (error) break;
    }
    if (error) std::cout << " error : " << error << std::endl;
    else std::cout << " 100%" << std::endl;
    // Check if error occured
    ASSERT_EQ(error, 0);
    parfis::api::deleteParfis(id);
}


/** @} gtestAll*/