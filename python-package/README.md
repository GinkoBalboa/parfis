# Parfis

Parfis is an acronym for Particles and Field Simulator. Parfis is written in 
C++ with Python bindings. The algorithm is based on the particle-in-cell 
(PIC) method used for interacting many-particle systems. In the PIC method
individual particles (or fluid elements) in a Lagrangian frame are tracked 
in a continuous phase space, whereas moments of the distribution such as 
densities and currents are computed simultaneously on Eulerian (stationary) 
mesh points.

## Quick start

You can quickly install and try parfis by doing the following in your terminal:

``` bash
pip install parfis
```

After installing you can try if the library loads and check the info of 
the installed build with the following three commands from your python terminal:

``` python
from parfis import Parfis
Parfis.load_lib()
print(Parfis.info())
```

## Explore more possibilities

Explore the possibilities by checking some of the examples given in the 
[demo](https://www.parfis.com/demo.html) section of the [doc web-site](https://www.parfis.com).
