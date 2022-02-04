#!/bin/bash
# set -e
shopt -s extglob
cd ../build
if [[ $? != 0 ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
rm -rf ./bin/* ./lib/parfis/* ./CMake* ./Makefile ./cmake* ./.cmake*
rm ./*.log
rm ../*.log
rm ../python-package/parfis/clib/libparfis32.so
cmake .. -DCOPY_LIB_PYTHON=ON
cmake --build . --config Release
./bin/gtestAll/gtestAll