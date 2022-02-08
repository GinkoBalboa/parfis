====================
Building from source
====================

This page gives instructions on how to build and install Parfis from the source code on 
various systems.

.. contents::

Obtain the code
===============

We describe the whole process, from cloning the repo to running tests.

#. Clone the repo:
    To clone the repo, run the following command from the folder 
    where you want the ``parfis`` folder to be created:

    .. code-block:: bash

        git clone https://github.com/GinkoBalboa/parfis.git


CMake options
=============

You can add build options through command-line arguments with the call to cmake. Build options:

- ``BUILD_LIB`` (default ``ON``). Should library be build at all (usefull when you want to generate
  documentation.
- ``PARFIS_STATE_TYPE_DOUBLE`` (default ``ON``). The parameter defines the type for the state 
  vector used in parfis. The state vector defines the position and velocity of particles. The 
  default type is float. If state type is **double**, the memory needed for particle states is 
  twice the size of the **float** state.
- ``BUILD_DEBUG`` (default ``OFF``). Build all binaries in debug mode. Debug version get the ``d``
  suffix.
- ``BUILD_PARFISAPP`` (default ``OFF``). Builds executable file ``parfisApp`` that functions as
  a terminal interface towards the Parfis program. The executable is build in the location
  ``/build/bin/parfisApp``.
- ``BUILD_DOXYGEN`` (default ``OFF``). Builds doxygen documentation in the folder 
  ``build/doc/doxygen``.
- ``BUILD_SPHINX`` (default ``OFF``). Builds sphinx documentation in the folder  
  ``build/doc/sphinx``.
- ``BUILD_GOOGLE_TEST`` (default ``OFF``). Clones the googletest repo and builds the project needed
  for testing. The build location is ``/build/lib/googletest``.
- ``BUILD_GTESTALL`` (default ``OFF``). Builds executable ``gtestAll`` that runs all google tests.
  The location of the executable is ``/build/bin/gtestAll``.
- ``COPY_LIB_PYTHON`` (default ``OFF``). Should parfis lib files be copied to the python-pacage for 
  generating pypi package.
- ``PARFIS_LOG_LEVEL`` (default 0). Bitmask for the Logger, corresponds to bitmasks defined in 
  :cpp:enum:`parfis::LogMask`.

For example: 
  If we want to build debug version with full logging for the first four levels we can use the 
  following cmake settings:
  
    .. code-block::bash

        parfis/build$ cmake .. -DBUILD_DEBUG=ON -DPARFIS_LOG_LEVEL=15 


Build
=====

After obtaining the code, go into the parfis dir, create build dir and run cmake configuration and 
cmake build.

For Linux/Windows we use CMake, and we run it from the build directory. The full set of 
commands, with default settings for cmake, is given in the following block of code:

.. code-block:: bash

    cd parfis
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release

Other configurations and build scripts are given in the ``parfis/script``
folder.