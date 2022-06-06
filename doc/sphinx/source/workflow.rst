Simulation workflow
===================

Every simulation starts with loading the ``parfis`` library. Loading is 
needed only once during the lifetime of the program, since the loading 
only brings the dynamical library into working memory, but no data is 
created yet.

.. code:: python

  from parfis import Parfis
  Parfis.load_lib()
