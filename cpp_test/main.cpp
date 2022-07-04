#include "gtest/gtest.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    // Uncomment line to run only speciffic test
    // ::testing::GTEST_FLAG(filter) = "api.createCells";
    // ::testing::GTEST_FLAG(filter) = "api.pushStates";
    // ::testing::GTEST_FLAG(filter) = "api.pyCfgData";
    // ::testing::GTEST_FLAG(filter) = "api.pySimData";
    // ::testing::GTEST_FLAG(filter) = "api.configureSpecieFromFile";
    // ::testing::GTEST_FLAG(filter) = "api.multiplePyVecStr";
    // ::testing::GTEST_FLAG(filter) = "api.gasCollisionDefinition";
    // ::testing::GTEST_FLAG(filter) = "api.gasCollisionDefinition:api.structPyFuncTable";
    // ::testing::GTEST_FLAG(filter) = "physics.checkUniformField";
    return RUN_ALL_TESTS();
}