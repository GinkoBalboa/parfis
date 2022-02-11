#!/bin/bash
shopt -s extglob
curdir="${PWD##*/}"
if [[ $curdir != "script" ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
./removeCMakeBuildFiles.sh
cd ../build
rm -rf ./doc
cmake .. -DBUILD_SPHINX=ON -DBUILD_LIB=OFF -DBUILD_GTESTALL=OFF
cmake --build . --config Release