
===============
C++ source code
===============

Documentation presented here is from the generated doxygen documentation.

.. contents:: Contents

parfis
======

Parfis is the main library of the parfis project. It can be compiled as shared or static library.

Logging
-------

Logging is defined in the cmake options, with the logging level. Logging is performed by creating 
log messages and adding them to the string in the Logger object. Every Parfis object has its own 
Logger object.

.. doxygenstruct:: parfis::Logger
    :members:

.. doxygenenum:: parfis::LogMask

.. doxygendefine:: LOG

.. doxygendefine:: LOG_LEVEL