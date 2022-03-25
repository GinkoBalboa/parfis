
===============
C++ source code
===============

Documentation presented here is from the generated doxygen documentation.

.. contents:: Contents

parfis library
==============

Parfis is the main library of the parfis project. It is compiled as shared library in two
modes. One mode has ``float`` for the ``state_t`` type (with suffix 32), and the other mode 
has ``double`` (with suffix 64). Both libraries are packed with the python module so the user
can use one or the other. Libraries are named:

- libparfis32.so *(Linux version, state_t = float)*
- libparfis64.so *(Linux version, state_t = double)*
- parfis32.dll *(Windows version, state_t = float)*
- parfis64.so *(Windows version, state_t = double)*

By default the **double** version is build and by default the **double**
version is loaded in the parfis Python module.

Logging
-------

Logging is defined in the cmake options, with the logging level. Logging is performed by creating 
log messages and adding them to the string in the Logger object. Every Parfis object has its own 
Logger object. When logging is set to zero, log files are created where only
initial text is written (date and parfis info).

.. doxygenstruct:: parfis::Logger
    :members:

.. doxygenenum:: parfis::LogMask

.. doxygendefine:: LOG

.. doxygendefine:: LOG_LEVEL