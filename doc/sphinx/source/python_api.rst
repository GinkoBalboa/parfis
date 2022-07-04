Python API
==========

The Python API is a wrapper around the C++ API. Structures and functionality
is copied from the C++ API with as similar naming as possible. 

Parfis class
------------

Parfis class is the main class of the API. It provides options to load the 
compiled dynamical library.

.. autoclass:: parfis.Parfis
    :members:

Data structure classes
----------------------

.. automodule:: parfis


.. autoclass:: parfis.Vec3DBase
    :members:


.. autoclass:: parfis.Type
    :members:

.. autoclass:: parfis.PyFuncTable
    :members:

Python tools
============

Tools in python are used for generating data (ex: cross-section tabulated functions)
and data processing.  

.. autofunction:: generateCrossSection

