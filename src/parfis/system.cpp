#include <fstream>
#include "config.h"
#include "system.h"
#include "global.h"

/**
 * @brief Construct a new System object
 * @param dname Name of the Domain object
 * @param logger Reference of the logger from Parfis object
 */
parfis::System::System(const std::string& dname, Logger& logger):
    Domain(dname, logger)
{
    LOG(*m_logger, LogMask::Memory, std::string(__FUNCTION__) + " constructor\n");
}

/**
 * @brief Loads data into CfgData object 
 * @details We access the CfgData object through a pointer, and the data object itself is a 
 * member of the Parfis class. 
 */
void parfis::System::loadCfgData()
{
    m_cfgData->systemTimestep = getParamValue<double>("timestep");
}