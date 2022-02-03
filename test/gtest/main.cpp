#include "gtest/gtest.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    // Uncomment line to run only speciffic test
    // ::testing::GTEST_FLAG(filter) = "api.createCells";
    // ::testing::GTEST_FLAG(filter) = "api.pushStates";
    // ::testing::GTEST_FLAG(filter) = "api.pySimData";
    return RUN_ALL_TESTS();
}