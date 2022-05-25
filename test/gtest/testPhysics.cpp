#include <fstream>
#include <iostream>
#include <mutex>
#include "gtest/gtest.h"
#include "global.h"
#include "parfis.h"
#include "define.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <bitset>
/**
 * @addtogroup gtestAll
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
    double rx, ry, rz;
    double zMax = parfis::api::getCfgData(id)->cellCount.z;
    parfis::Vec3D<double> geoCenter = {
        0.5 * parfis::api::getCfgData(id)->cellCount.x, 
        0.5 * parfis::api::getCfgData(id)->cellCount.y,
        0.5 * parfis::api::getCfgData(id)->cellCount.z};
    double radiusSquared = geoCenter.x*geoCenter.x;
    double overRatio = 1.00001;
    uint32_t i;
    for (i = 0; i<1000; i++) {
        if (i%50 == 0) std::cout << "|" << std::flush;
        parfis::api::runCommandChain(id, "evolve");
        pSimData = parfis::api::getSimData(id);
        for (auto& spec : pSimData->specieVec) {
            for (auto cellId : pSimData->cellIdBVec) {
                pCell = &pSimData->cellVec[cellId];
                stateId = pSimData->headIdVec[spec.headIdOffset + cellId];
                while (stateId != 0xFFFFFFFF) {
                    pState = &pSimData->stateVec[stateId];
                    if (pState->pos.x > 1.0 || pState->pos.x < 0.0)
                        error += 0b1;
                    if (pState->pos.y > 1.0 || pState->pos.y < 0.0)
                        error += 0b10 ;
                    if (pState->pos.z > 1.0 || pState->pos.z < 0.0)
                        error += 0b100;
                    if (pState->pos.y > 1.0 || pState->pos.y < 0.0)
                        error += 0b1000;
                    if (pState->pos.z > 1.0 || pState->pos.z < 0.0)
                        error += 0b10000;
                    if (pState->vel.x*pState->vel.x + 
                        pState->vel.y*pState->vel.y + 
                        pState->vel.z*pState->vel.z > 1.0)
                        error += 0b100000;
                    rx = pState->pos.x + pCell->pos.x - geoCenter.x;
                    ry = pState->pos.y + pCell->pos.y - geoCenter.y;
                    rz = pState->pos.z + pCell->pos.z;
                    if (rx*rx + ry*ry > radiusSquared*overRatio) {
                        std::cout << "\nrx = " << rx << std::endl;
                        std::cout << "\nry = " << ry << std::endl;
                        std::cout << "\nrx*rx + ry*ry = " << rx*rx + ry*ry << std::endl;
                        error += 0b1000000;             
                    }       
                    if (rz > zMax || rz < 0.0) 
                        error += 0b10000000;
                    if (error) break;
                    stateId = pSimData->stateVec[stateId].next;
                }
                if (error) break;
            }
            if (error) break;
        }
        if (error) break;
    }
    if (error) {
        std::cout << "\nerror: " << std::bitset<16>(error) << ", on iteration i: " << i << std::endl;
        std::cout << "***** start log *****\n" << parfis::api::getLogStr(id) << "***** end log *****" << std::endl;
    }
    else std::cout << " 100%" << std::endl;
    // Check if error occured
    ASSERT_EQ(error, 0);
    parfis::api::deleteParfis(id);
}

/**
 * @brief Check the uniform field definition and velocity change in the force field
 */
TEST(physics, checkUniformField) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::setConfig(id, "system.field.typeE = [0, 0, 1]");
    parfis::api::setConfig(id, "system.field.strengthE = [0, 0, 10000.0]");
    parfis::api::loadCfgData(id);
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    const parfis::CfgData *pCfgData = parfis::api::getCfgData(id);
    const parfis::SimData *pSimData = parfis::api::getSimData(id);
    ASSERT_EQ(pCfgData->field.typeE.x, 0);
    ASSERT_EQ(pCfgData->field.typeE.y, 0);
    ASSERT_EQ(pCfgData->field.typeE.z, 1);
    double prevVz = pSimData->stateVec[100].vel.z;
    for (uint32_t i = 0; i<10; i++) {
        parfis::api::runCommandChain(id, "evolve");
        ASSERT_NE(prevVz, pSimData->stateVec[100].vel.z);
        prevVz = pSimData->stateVec[100].vel.z;
    }
    parfis::api::deleteParfis(id);
}

/**
 * @brief Check gas definitions and particle-gas collision definitions
 */
TEST(physics, gasCollisionDefinition) {
    uint32_t id = parfis::api::newParfis();
    parfis::api::setConfigFromFile(id, "./data/config_files/test_physics_gasCollisionDefinition.ini");
    parfis::api::loadCfgData(id);
    parfis::api::loadSimData(id);
    parfis::api::runCommandChain(id, "create");
    const parfis::CfgData *pCfgData = parfis::api::getCfgData(id);
    const parfis::SimData *pSimData = parfis::api::getSimData(id);
    parfis::api::deleteParfis(id);
}

/** @} gtestAll*/