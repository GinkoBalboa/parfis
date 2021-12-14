====================
Building from source
====================

This page gives instructions on how to build and install Parfis from the source code on 
various systems.

.. contents::

Obtaining the code
==================

Here we describe the whole process from cloning the repo to running tests.

#. Clone the repo:
    To clone the repo, run the following command from the folder 
    where you want the ``parfis`` folder to be created:

    .. code-block:: bash

        git clone https://github.com/GinkoBalboa/parfis.git

#. Clone ``googletest`` repo in ``parfis/cpp/test``:

    .. code-block:: bash

        cd parfis/cpp/test
        git clone https://github.com/google/googletest.git

Build under Linux
=================

For Linux we use CMake and we run it from the build directory. Full set of commands is 
given in the following block of code:

.. code-block:: bash

    cd parfis
    mkdir build
    cd build
    cmake ..
    make -j$(nproc)