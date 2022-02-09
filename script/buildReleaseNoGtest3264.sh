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
rm ../test/pytest/*.log
rm ../doc/source/demo/*.log
rm ../python-package/parfis/clib/*.so
cmake .. -DCOPY_LIB_PYTHON=ON -DPARFIS_STATE_TYPE_DOUBLE=OFF
cmake --build . --config Release
rm -rf ./CMake* ./Makefile ./cmake* ./.cmake*
cmake .. -DCOPY_LIB_PYTHON=ON -DPARFIS_STATE_TYPE_DOUBLE=ON
cmake --build . --config Release