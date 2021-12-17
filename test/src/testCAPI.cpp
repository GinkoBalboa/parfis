#include <fstream>
#include <iostream>
#include <mutex>
#include "gtest/gtest.h"
#include "parfis.h"
#include "define.h"

TEST(cAPI, checkInfo) {
    std::string infostr = parfis::cAPI::info();
    std::string str1 = infostr.substr(0, infostr.find('\n'));
    std::string str2 = std::string("state_t size = ") + std::to_string(sizeof(parfis::state_t));
    ASSERT_EQ(str1, str2);
}