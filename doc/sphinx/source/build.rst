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

- ``BUILD_DEBUG`` (default ``OFF``). Build all binaries in debug mode. Debug version get the ``d``
  suffix.
- ``BUILD_STATIC_LIB`` (default ``OFF``). Builds static lib and links to produce a single
  executable file. The default builds shared lib (``.so`` under Linux and ``.dll`` under Windows).
  The executable needs the shared library at the time of linking and execution. Library is build 
  in the directory ``/build/lib/parfis``.
- ``BUILD_PARFISAPP`` (default ``ON``). Builds executable file ``parfisApp`` that functions as
  a terminal interface towards the Parfis program. The executable is build in the location
  ``/build/bin/parfisApp``.
- ``PARFIS_STATE_TYPE_DOUBLE`` (default ``OFF``). The parameter defines the type for the state 
  vector used in parfis. The state vector defines the position and velocity of particles. The 
  default type is float. If state type is **double**, the memory needed for particle states is 
  twice the size of the **float** state.
- ``BUILD_DOXYGEN`` (default ``OFF``). Builds doxygen documentation in the folder 
  ``build/doc/doxygen``.
- ``BUILD_SPHINX`` (default ``OFF``). Builds sphinx documentation in the folder  
  ``build/doc/sphinx``.
- ``BUILD_GOOGLE_TEST`` (default ``OFF``). Clones the googletest repo and builds the project needed
  for testing. The build location is ``/build/lib/googletest``.
- ``BUILD_GTESTALL`` (default ``OFF``). Builds executable ``gtestAll`` that runs all google tests.
  The location of the executable is ``/build/bin/gtestAll``.
- ``PARFIS_LOG_LEVEL`` (default 0). Bitmask for the Logger, corresponds to bitmasks defined in 
  :cpp:enum:`parfis::LogMask`.

For example: 
  If we want to build debug version with full logging for the first four levels we can use the 
  following cmake settings:
  
    .. code-block::bash

        parfis/build$ cmake .. -DBUILD_DEBUG=ON -DPARFIS_LOG_LEVEL=15 


Build under Linux
=================

For Linux we use CMake, and we run it from the build directory. The full set of commands, with 
default settings for cmake, is given in the following block of code:

.. code-block:: bash

    cd parfis
    mkdir build
    cd build
    cmake ..
    make

Build under Windows
===================

Under Windows systems we don't have the ``make`` command so we must use ``cmake`` with the`
``--build`` flag and also configuration must be specified (``Debug`` of ``Release``). Here
we give an example for the debug compilation under windows:

.. code-block:: bash

    cd parfis
    mkdir build
    cd build
    cmake .. -DBUILD_DEBUG=ON
    cmake --build . --config Debug