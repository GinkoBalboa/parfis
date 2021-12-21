#include "gtest/gtest.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    // Uncomment line to run a speciffic test
    //::testing::GTEST_FLAG(filter) = "System.*";
    //::testing::GTEST_FLAG(filter) = "Space.*";
    //::testing::GTEST_FLAG(filter) = "Particle.*";
    //::testing::GTEST_FLAG(filter) = "System.*:Space.*:Particle.*";
    //::testing::GTEST_FLAG(filter) = "System.*:Space.*:Particle.*:API.*";
    //::testing::GTEST_FLAG(filter) = "API.ParticlePusherCylindrical";
    //::testing::GTEST_FLAG(filter) = "API.Getters";
    //::testing::GTEST_FLAG(filter) = "Algorithm.LUTFunctionTime";
    //::testing::GTEST_FLAG(filter) = "Algorithm.LUTFunction";
    return RUN_ALL_TESTS();
}