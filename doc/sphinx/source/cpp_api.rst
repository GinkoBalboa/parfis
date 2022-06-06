
C++ API
=======

Here we give just brief info of the C++ API since detailed description is in 
the Doxy pages.

The Parfis library
------------------

Parfis is the main library of the parfis project. It is compiled as a shared library in two
modes. One mode has ``float`` for the ``state_t`` type (with suffix 32), and the other mode 
has ``double`` (with suffix 64). Both libraries are packed with the python module so the user
can use one or the other. Libraries are named:

- libparfis32.so *(Linux version, state_t = float)*
- libparfis64.so *(Linux version, state_t = double)*
- parfis32.dll *(Windows version, state_t = float)*
- parfis64.so *(Windows version, state_t = double)*

By default, the **double** version is going to be built and by default the **double**
version is loaded in the parfis Python module.


Using C++ functionality
-----------------------

C++ supports function overloading. This means there can be more than one function with the 
same name but different parameters. We use the `extern "C"` to turn off name-mangling. This
way we expose the C++ functions as standard C functions. Functions that can be called from outside
the parfis library are exclusively under the `extern "C"` and defined as an API.

`Doxy page for the api <https://www.parfis.com/doxygen/group__api.html>`_


Logging
-------

Logging level is defined in the cmake options. Logging is performed by creating 
log messages and adding them to the string in the Logger object. Every Parfis object has its own 
Logger object. When logging is set to zero, log files are created where only
initial text is written (date and parfis info).

`Doxy page for the logging <https://www.parfis.com/doxygen/group__logging.html>`_