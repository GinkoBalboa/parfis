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
rm ../python-package/parfis/clib/libparfis64d.so
cmake .. -DCOPY_LIB_PYTHON=ON -DBUILD_DEBUG=ON -DPARFIS_STATE_TYPE_DOUBLE=ON
cmake --build . --config Debug
./bin/gtestAll/gtestAlld