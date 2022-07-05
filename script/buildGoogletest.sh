#!/bin/bash
shopt -s extglob
curdir="${PWD##*/}"
if [[ $curdir != "script" ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
./removeCMakeBuildFiles.sh
cd ../build
echo "Remove googletest files"
rm -rf ./googletest* ./gtest* ./lib/googletest
echo "Build googletest"
cmake .. -DBUILD_LIB=OFF -DBUILD_GTESTALL=OFF -DBUILD_GOOGLE_TEST=ON
cmake --build . --config Release