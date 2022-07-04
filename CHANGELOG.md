# Change log

## 0.0.7 (unreleased)

Features:
  - **FuncTable** structure - to have nonlinear function tabulation. This is needed 
    for cross-section representation.
  - **GasCollision** structure - to represent particle collisions with gas.

Changes:
  - Changed location of tests:
    * c++ tests are now in cpp_test
    * python tests are now in python_test

Fixes:
  - Fixed multiple PyVec<std::string> objects in PyCfgData. This wasn't
    noticed as a bug since we didn't have this situation. The problem
    was with the concept of keeping pointers to the strings in the data structure.

## 0.0.6 (released 2022-06-06)

Features:

  - Conda environment `.yml` for easier management of the parfis library.
  - Flag for nbexecution `NBSPHINX_EXECUTE` when generating doc.
  - Selection of random seed for the random engine in the configuration file.
  - Possibility to change the random engine by setting `randEngine_t` in `datastruct.h`.
  - Loading configuration from files.
  - Avoid using strings where int can be used (ex: classified entries).
  - Nonlinear function tabulation.
  - Gas collision definitions.

Changes:

  - Turn off log printing to file by default.
  - Actions for debug config are optional.

## 0.0.5 (released 2022-03-25)

Features:

  - Parfis module is ported with two shared libs, one with `float` type for state_t and the other
    with `double` type. Users can choose to load one or another lib, or even switch libs in the 
    same session. Switching libs triggers the unloading and loading of the shared libs.
  - Windows and Linux build systems and wheels for Win and Linux.
  - Github Actions: automatic build, testing C++ and Python, and preparing wheels for PyPI (on demand).
  - Documentation generation for the website.

## < 0.0.5

Info:

  - Versions lower than 0.0.5 are based on different concepts than newer versions.