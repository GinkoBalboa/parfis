
Function tabulation
===================

We often have the need for tabulated functions in the code. Especially for the collision
algorithms. Collision cross-sections can be defined in an extremely broad range, where it can 
be highly nonlinear in a very small part of the range. To solve that problem we have used nonlinear
tabulation. Basic idea is to define manually ranges for tabulation so that certain parts 
of the function's domain can have a controllable resolution.

.. doxygenstruct:: parfis::FuncTable
    :members:

For every collision cross-section, we use a single FuncTable object 
:cpp:member:`parfis::GasCollision::xSecFtab`. We calculate the collision frequencies for every 
cross-section in another FuncTable object :cpp:member:`parfis::GasCollision::freqFtab`. In the end, we combine
all of the collisional frequencies in a single FuncTable object with multiple rows and columns 
:cpp:member:`parfis::SimData::gasCollisionProbVec`.

For the example given in the demo notebook :ref:`/collisional_files/generating_simple_cross_sections.ipynb`, 
we have set the following ranges and bins for one specie with two collisional cross cross-sections:

.. code-block:: python

    ranges = [   1,   10,  100, 1000, 10000, 114000]
    nbins =  [1000, 1000, 1000, 1000,  1000,   1000]

The resulting generated FuncTable objects have the following members defining the data format:

.. code-block:: python

    xSecFtab.type = 1
    xSecFtab.colCnt = 6000
    xSecFtab.rowCnt = 1

    freqFtab.type = 1
    freqFtab.colCnt = 6000
    freqFtab.rowCnt = 1

    gasCollisionProbVec[0].type = 1
    gasCollisionProbVec[0].colCnt = 2
    gasCollisionProbVec[0].rowCnt = 6000

We can see that the unrolling of the matrix format in the probability vector is 
different than the cross-section and collision frequency FuncTable. This vector format
is more convenient for us. Once we have found the particle value on the x-axis (squared
speed) we can easily go through all of the collision probabilities because they are 
located in successive memory addresses.

Wrapper for the python module
-----------------------------

The function tabulation has a wrapper for Python which on the python side is given as:

.. doxygenstruct:: parfis::PyFuncTable
    :members:




