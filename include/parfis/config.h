#ifndef PARFIS_CONFIG_H
#define PARFIS_CONFIG_H
#define DEFAULT_CONFIGURATION "\
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
system = [geometry, timestep] <parfis::Param> (geometry, timestep) # The system configuration domain  \n\
system.geometry = cylindrical <std::string> (cylindrical) # Only cylindrical geometry is supported for now \n\
system.timestep = 0.001  <double> (0, 1.0e-12) # Timestep is given in seconds \n\
"
#endif // PARFIS_CONFIG_H