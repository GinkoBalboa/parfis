Simulation data structure
=========================

The data structure is used for keeping track of the simulation data, particles, fields 
and data that changes throughout the simulation. The simulation data is kept in a member 
variable inside the `Parfis` class. There is a complete structure defined that holds 
vectors of data concerning different aspects of the system.

.. doxygenstruct:: parfis::SimData
    :members:

.. doxygenstruct:: parfis::State
    :members:

.. doxygentypedef:: parfis::stateId_t


Particle data
-------------

The particle data is organized in the following manner:

- The vector with raw states :cpp:member:`parfis::SimData::stateVec` holds
  the state in the phase space for every particle in the system regardless of 
  its specie. This is the largest memory structure in the 
  program when initialized. The elements of the vector are of type 
  :cpp:class:`parfis::State`. Every state structure beside the phase space vector
  holds data of other state ids from the same cell, thus forming a doubly connected
  list (per cell). The size of the State structure depends on the library version,
  i.e. the size of the ``state_t`` type.

    .. image:: img/particle_memory.png
        :width: 600

- Vector of head ids :cpp:member:`parfis::SimData::headIdVec` holds id of the first 
  state, of each specie, of each cell.


Function tabulation
-------------------

We often have the need for tabulated functions in the code. Especially for the collision
algorithms. Collision cross-sections can be defined in an extremely broad range, where it can 
be highly nonlinear in a very small part of the range. To solve that problem we have used nonlinear
tabulation.