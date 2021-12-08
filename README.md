# Parfis

Particles and field simulator. Simulator written in C++ with Python interface.
Simulator is based on an altered particle-in-cell (PIC) method. In the PIC method
method individual particles (or fluid elements) in a Lagrangian frame are tracked
in a continuous phase space, whereas moments of the distribution such as densities
and currents are computed simultaneously on Eulerian (stationary) mesh points.


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
$ make -j$(nproc)
```
