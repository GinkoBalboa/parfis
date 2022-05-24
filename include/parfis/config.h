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
#------------ Domains ------------\n\
domain = [system, particle] <parfis::Domain> (system, particle) # Domains are defined as separate classes in the code  \n\
\n\
#------------ System ------------\n\
system = [geometry, timestep, geometrySize, cellSize, periodicBoundary, fieldTypeE, fieldTypeB, fieldStrengthE, fieldStrengthB] <parfis::Param> (geometry, timestep, geometrySize, cellSize, periodicBoundary, fieldTypeE, fieldTypeB, fieldStrengthE, fieldStrengthB)  # The system configuration domain  \n\
system.geometry = cylindrical <std::string> (cylindrical) # Only cylindrical geometry is supported for now \n\
system.timestep = 1.0 <double> (0.0, 1.0) # Timestep is given in seconds \n\
system.geometrySize = [0.02, 0.02, 0.4] <double> (0.0, 1.0) # System geometry size in meters \n\
system.cellSize = [1e-3, 1e-3, 1e-3] <double> (0.0, 1.0) # Cell size in meters \n\
system.periodicBoundary = [0, 0, 0] <int> (0, 1)  # Boundary condition \n\
system.fieldTypeE = [0, 0, 1] <int> (0, 1) # Type of electric field 0: none, 1: uniform\n\
system.fieldTypeB = [0, 0, 0] <int> (0, 1) # Type of magnetic field 0: none, 1: uniform\n\
system.fieldStrengthE = [0, 0, 1] <double> (0, 1) # Strength of electric field V/m\n\
system.fieldStrengthB = [0, 0, 0] <double> (0, 1) # Strength of magnetic field T\n\
\n\
particle = specie <parfis::Param> (specie) # Particle parameters\n\
particle.specie = a <parfis::Param> (*) # You can define specie name anyway you like \n\
particle.specie.a = [statesPerCell, timestepRatio, amuMass, eCharge, velInitRandom, velInitDistMin, velInitDistMax] <parfis::Param> (statesPerCell, timestepRatio, amuMass, eCharge, velInitRandom, velInitDistMin, velInitDistMax) # Parameters for each specie \n\
particle.specie.a.statesPerCell = 10 <int> # Number of states (particles) per cell\n\
particle.specie.a.timestepRatio = 1 <int> # This many system.timesteps is one timestep for the specie\n\
particle.specie.a.amuMass = 4 <double> # Mass is given in a.m.u. \n\
particle.specie.a.eCharge = 1 <int> # Charge is given in units of elementary charge \n\
particle.specie.a.velInitRandom = uniform <std::string> # Random generator for initial velocity vector \n\
particle.specie.a.velInitDistMin = [-0.5773502691, -0.5773502691, -0.5773502691] <double> # Array of v initial distribution minimums \n\
particle.specie.a.velInitDistMax = [0.5773502691, 0.5773502691, 0.5773502691] <double> # Array of v initial distribution maximums \n\
\n\
commandChain = [create, evolve] <parfis::CommandChain> (create, evolve) # Command chains for the program \n\
commandChain.create = [createCells, createStates] (createCells, createStates) <parfis::Command> # Command chain for creation of data \n\
commandChain.evolve = [pushStates] <parfis::Command> # Command chain for evolving the system\n\
"
/** @} configuration */
#endif // PARFIS_CONFIG_H