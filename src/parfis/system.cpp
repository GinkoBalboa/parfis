#include <fstream>
#include "config.h"
#include "system.h"
#include "global.h"

parfis::System::System(const std::string& dname, Logger& logger):
    Domain(dname, logger)
{
    LOG(*m_logger, LogMask::Memory, std::string("\n") + __FUNCTION__ + " constructor");
}

/**
 * @brief Initializes variables from the Data according to the initialization string
 * 
 * @param cstr Initialization string in the format key=value 
 * @return int Zero on success 
 */
int parfis::System::configure(const std::string& cstr) 
{
    std::tuple<std::string, std::string> keyValue = Global::splitKeyValue(cstr);
    if (std::get<1>(keyValue).find("<parfis::Param>") != std::string::npos) {
        auto vec = Global::getVector(std::get<1>(keyValue), '[', ']');
        for (auto& pname: vec) {
            addChild(pname);
        }
    }
    return 0;
}