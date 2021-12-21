#include "system.h"
#include "global.h"

parfis::System::System(parfis::Logger& logger):
    Domain(logger, "system")
{
    log(*m_logger, LogMask::Memory, std::string("\n") + __FUNCTION__ + " constructor");
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
    return 0;
}

int parfis::System::initialize()
{
    return 0;
}
