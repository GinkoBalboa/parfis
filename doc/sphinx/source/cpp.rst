
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
can use one or the other.

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