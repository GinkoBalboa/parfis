# Parfis

[![build-and-test Actions Status](https://github.com/GinkoBalboa/parfis/workflows/build-and-test/badge.svg)](https://github.com/GinkoBalboa/parfis/actions)

Parfis is an acronym for PARticles and FIeld Simulator. Parfis is written in 
C++ with Python bindings. The algorithm is based upon the particle-in-cell 
(PIC) method used for interacting many-particle systems. In the PIC method
individual particles (or fluid elements) in a Lagrangian frame are tracked 
in a continuous phase space, whereas moments of the distribution such as 
densities and currents are computed simultaneously on Eulerian (stationary) 
mesh points.


## Build from source

Build configuration is produced with CMake. After obtaining the source run the 
following commands corresponding to your OS.

### Building on Linux and other UNIX like systems

For Linux we use the following commands:

``` bash
$ cd parfis
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Building on Windows

For Windows we have:

``` console
$ cd parfis
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build . --config Release
```
