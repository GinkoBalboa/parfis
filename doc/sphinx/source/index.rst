.. Parfis documentation master file, created by
   sphinx-quickstart on Mon Dec 13 17:49:07 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Parfis
======

Parfis is an acronym for Particles and Field Simulator. Parfis is written in 
C++ with Python bindings. The algorithm is based on the particle-in-cell 
(PIC) method used for interacting many-particle systems. In the PIC method
individual particles (or fluid elements) in a Lagrangian frame are tracked 
in a continuous phase space, whereas moments of the distribution such as 
densities and currents are computed simultaneously on Eulerian (stationary) 
mesh points.


Quick start
===========

You can quickly install and try parfis by doing the following in your terminal:

.. code-block:: bash

    pip install parfis

After installing you can try if the library loads and check the info of 
the istalled build with the following three commands from your python terminal:

.. code-block:: python

   from parfis import Parfis
   Parfis.load_lib()
   print(Parfis.info())

Explore the possibilities by checking some of the examples given in the 
:doc:`demo` section of this site. The above three commands are given in 
the :doc:`demo_files/simple_demo`.

.. toctree::
   :maxdepth: 2
   :caption: Installation:

   installation
   build

.. toctree::
   :maxdepth: 2
   :caption: Tutorials:

   demo

.. toctree::
   :maxdepth: 2
   :caption: CPP API:

   cpp

.. toctree::
   :maxdepth: 1
   :caption: External links:
   :titlesonly:

   Doxygen docs <https://www.parfis.com/doxygen/index.html>
   GitHub repo <https://github.com/GinkoBalboa/parfis>

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
