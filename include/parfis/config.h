#ifndef PARFIS_CONFIG_H
#define PARFIS_CONFIG_H
/**
 * @file config.h
 *
 * @brief Configuration and initialization strings.
 */
/** 
 * @addtogroup configuration 
 * @{
 */
/**
 * @brief The default initialization string
 */
#define DEFAULT_INITIALIZATION_STRING "\
# Default configuration for Parfis objects\n\
# Settings are defined in key=value or key=[value1, value2, ... ] pairs, all other strings\n\
# and controll characters are optional and are used only for initialization of Param objects. \n\
# [] - defines list of values\n\
# <> - defines the value type\n\
# () - defines the range of possible values or min,max\n\
# #  - starts comment\n\
\n\
domain = [system] <parfis::Domain> (system, particle) # Domains are defined as separate classes in the code  \n\
\n\
system = [geometry, timestep] <parfis::Param> (geometry, timestep)  # The system configuration domain  \n\
system.geometry = cylindrical <std::string>   (cylindrical)         # Only cylindrical geometry is supported for now \n\
system.timestep = 1.0         <double>        (0, 1.0e-12)          # Timestep is given in seconds \n\
system.geometrySize = [0.02, 0.02, 0.4]    <double>   (0.0, 1.0)    # System geometry size in meters \n\
system.unitCellSize = [2e-3, 2e-3, 2e-3]   <double>   (0.0, 1.0)    # Cell size in meters \n\
system.periodicBoundary = [0, 0, 0]        <int>      (0, 1)        # Boundary condition \n\
"
/** @} configuration */
#endif // PARFIS_CONFIG_H