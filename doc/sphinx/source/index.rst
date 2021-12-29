.. Parfis documentation master file, created by
   sphinx-quickstart on Mon Dec 13 17:49:07 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to Parfis's documentation!
==================================

Parfis is a particles and field simulator. Written in C++ with Python interface, Parfis is based on 
the altered particle-in-cell (PIC) method. In the PIC method method individual particles 
(or fluid elements) in a Lagrangian frame are tracked in a continuous phase space, whereas moments 
of the distribution such as densities and currents are computed simultaneously on Eulerian 
(stationary) mesh points.

`doxygen <www.google.com>`_

- `Link to GitHub repo <https://github.com/GinkoBalboa/parfis>`_
- Version: |version|

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   installation
   build
   cpp

.. toctree::
   :maxdepth: 1
   :caption: External links:
   :titlesonly:

   Doxygen  docs<https://www.parfis.com/doxygen/index.html>
   GitHub repo <https://github.com/GinkoBalboa/parfis>

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
