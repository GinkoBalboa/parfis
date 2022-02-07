#!/bin/bash
# set -e
shopt -s extglob
cd ../build
if [[ $? != 0 ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
rm -rf ./CMake* ./Makefile ./cmake* ./.cmake* ./doc
cmake .. -DBUILD_SPHINX=ON -DBUILD_LIB=OFF -DBUILD_GTESTALL=OFF -DSETUP_PYTHON_MODULE=OFF
cmake --build . --config Release