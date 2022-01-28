# Parfis

[![build-and-test Actions Status](https://github.com/GinkoBalboa/parfis/workflows/main/badge.svg)](https://github.com/GinkoBalboa/parfis/actions)

Parfis is an acronym for PARticles and FIeld Simulator. Parfis is written in 
C++ with Python bindings. The algorithm is based upon the particle-in-cell 
(PIC) method used for interacting many-particle systems. In the PIC method
individual particles (or fluid elements) in a Lagrangian frame are tracked 
in a continuous phase space, whereas moments of the distribution such as 
densities and currents are computed simultaneously on Eulerian (stationary) 
mesh points.


## Building from source

The build configuration is generated with CMake. After obtaining the source from the
GitHub repo, run the CMake commands shown here.

### Building on Linux and Windows

Building on Linux and Windows is the same, presumably you have CMake installed:

``` bash
$ cd parfis
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build . --config Release
```
