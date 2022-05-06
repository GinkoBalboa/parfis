# Parfis

![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/GinkoBalboa/parfis)
![GitHub](https://img.shields.io/github/license/GinkoBalboa/parfis)
[![Build and test Actions Status](https://github.com/GinkoBalboa/parfis/actions/workflows/main.yml/badge.svg)](https://github.com/GinkoBalboa/parfis/actions)

Parfis is an acronym for Particles and Field Simulator. Parfis is written in 
C++ with Python bindings. The algorithm is based on the particle-in-cell 
(PIC) method used for interacting many-particle systems. In the PIC method
individual particles (or fluid elements) in a Lagrangian frame are tracked 
in a continuous phase space, whereas moments of the distribution such as 
densities and currents are computed simultaneously on Eulerian (stationary) 
mesh points.

## Quick start

You can quickly install and try parfis by doing the following in your terminal:

``` bash
pip install parfis
```

After installing you can try if the library loads and check the info of 
the istalled build with the following three commands from your python terminal:

``` python
from parfis import Parfis
Parfis.load_lib()
print(Parfis.info())
```

## Explore more possibilities

Explore the possibilities by checking some of the examples given in the 
[demo](https://www.parfis.com/demo.html) section of the [doc web-site](https://www.parfis.com).


## Building from source

The build configuration is generated with CMake. After obtaining the source from the
GitHub repo, run the CMake commands shown here.


### Requirements

For building the dynamic library you will need cmake and cxx compiler. For running
tests you will need to pull and build google test. On Linux the following commands
will install cmake and cxx compiler, and the script will build the parfis lib in 
debug mode and pull, build and run google test and gtest.

``` bash
sudo apt get install cmake
sudo apt get install g++
cd scripts
./buildDebug32_64_buildGoogleTest_and_gtestAll.sh
```

### Building on Linux and Windows

Building on Linux and Windows is the same, presumably you have CMake installed:

``` bash
cd parfis
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

For more info on building from source check [build](https://www.parfis.com/build.html) section 
from the doc web-size.
