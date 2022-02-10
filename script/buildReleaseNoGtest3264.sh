#!/bin/bash
shopt -s extglob
curdir="${PWD##*/}"
if [[ $curdir != "script" ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
./removeAllLogFiles.sh
cd ../build
rm -rf ./bin/* ./lib/parfis/* ./CMake* ./Makefile ./cmake* ./.cmake*
cmake .. -DCOPY_LIB_PYTHON=ON -DPARFIS_STATE_TYPE_DOUBLE=OFF
cmake --build . --config Release
rm -rf ./CMake* ./Makefile ./cmake* ./.cmake*
cmake .. -DCOPY_LIB_PYTHON=ON -DPARFIS_STATE_TYPE_DOUBLE=ON
cmake --build . --config Release