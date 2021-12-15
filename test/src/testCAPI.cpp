#include <fstream>
#include <iostream>
#include <mutex>
#include "gtest/gtest.h"
#include "parfis.h"
#include "define.h"

TEST(cAPI, GetPtrData) {
    std::cout << parfis::cAPI::info() << std::endl;
}