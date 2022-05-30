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
# and controll characters are optional and are used only for initialization of Param objects.\n\
#\n\
# Predefined domains (have special c++ class representations) are of type <parfis::Domain>\n\
# Complex data types are named as <parfis::Param>\n\
# Simple data types can be <int>, <double> and <std::string>\n\
# Command chain is of type <parfis::CommandChain>\n\
# Commands are of type <parfis::Command>\n\
#\n\
# Data types <std::string> and <parfis::Param> are treated as strings, with the difference\n\
# that <parfis::Param> builds linked list structure for child elements.\n\
# \n\
# Double and single quotes for values of <std::strings> and <parfis::Params> are stripped, \n\
# so strings can be defined with or without them. The convention is that if the string is \n\
# predefined one, it is written without quotes, if it is a name than is written with quotes.\n\
#\n\
# [] - defines list of values\n\
# <> - defines the value type\n\
# #  - starts comment\n\
\n\
#------------ Domains ------------\n\
domain = [system, particle] <parfis::Domain> # Domains are defined as separate classes in the code\n\
\n\
#------------ System ------------\n\
system = [geometry, timestep, geometrySize, cellSize, periodicBoundary, field] <parfis::Param> # System domain  \n\
system.geometry = 1 <int> # Only cylindrical geometry is supported for now (0: cubical, 1: cylindrical)\n\
system.timestep = 1.0 <double> # Timestep is given in seconds \n\
system.geometrySize = [0.02, 0.02, 0.4] <double> # System geometry size in meters \n\
system.cellSize = [1e-3, 1e-3, 1e-3] <double> # Cell size in meters \n\
system.periodicBoundary = [0, 0, 0] <int> # Boundary condition (0: wall, 1: periodic)\n\
# Field\n\
system.field = [typeE, typeB, strengthE, strengthB] <parfis::Param> # Field parameters\n\
system.field.typeE = [0, 0, 0] <int> # Type of electric field 0: none, 1: uniform\n\
system.field.typeB = [0, 0, 0] <int> # Type of magnetic field 0: none, 1: uniform\n\
system.field.strengthE = [0, 0, 0] <double> # Strength of electric field in V/m\n\
system.field.strengthB = [0, 0, 0] <double> # Strength of magnetic field in T\n\
\n\
#------------ Particles ------------\n\
particle = specie <parfis::Param> # Particle domain\n\
particle.specie = \"a\" <parfis::Param> # Species\n\
particle.specie.a = [statesPerCell, timestepRatio, amuMass, eCharge, velInitDist, velInitDistMin, velInitDistMax, randomSeed] <parfis::Param> # Parameters for each specie \n\
particle.specie.a.statesPerCell = 10 <int> # Number of states (particles) per cell\n\
particle.specie.a.timestepRatio = 1 <int> # This many system.timesteps is one timestep for the specie\n\
particle.specie.a.amuMass = 4 <double> # Mass is given in a.m.u. \n\
particle.specie.a.eCharge = 1 <int> # Charge is given in units of elementary charge \n\
particle.specie.a.velInitDist = 0 <int> # Random generator for initial velocity vector (0: uniform)\n\
particle.specie.a.velInitDistMin = [-0.5773502691, -0.5773502691, -0.5773502691] <double> # Array of v initial distribution minimums \n\
particle.specie.a.velInitDistMax = [0.5773502691, 0.5773502691, 0.5773502691] <double> # Array of v initial distribution maximums \n\
particle.specie.a.randomSeed = 0 <int> # Seed for the random engine (0: random_device, <int>: seed number)\n\
\n\
#------------ Command Chain ------------\n\
commandChain = [create, evolve] <parfis::CommandChain> # Command chain\n\
commandChain.create = [createCells, createStates] <parfis::Command> # Commands for creation of data \n\
commandChain.evolve = [pushStates] <parfis::Command> # Commands for evolving the system\n\
"
/** @} configuration */
#endif // PARFIS_CONFIG_H
