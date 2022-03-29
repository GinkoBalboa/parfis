# Change log

## 0.0.6

Features:

## 0.0.5

Features:

  - Parfis module is ported with two shared libs, one with `float` type for state_t and the other
    with `double` type. User can choose to load one or another lib, or even switch libs in the 
    same session. Switching libs triggers unloading and loading of the shared libs.
  - Windows and Linux build systems and wheels for win and linux.
  - Github Actions: automatic build, testing C++ and Python and preparing wheels for PyPI (on demand).
  - Documentation generation for web site.

## < 0.0.5

Info:

  - Versions lower than 0.0.5 are based on different concepts than newer versions.