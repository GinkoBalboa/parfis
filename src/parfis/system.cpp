#include <fstream>
#include "config.h"
#include "system.h"
#include "global.h"

parfis::System::System(const std::string& dname, Logger& logger):
    Domain(dname, logger)
{
    LOG(*m_logger, LogMask::Memory, std::string("\n") + __FUNCTION__ + " constructor");
}