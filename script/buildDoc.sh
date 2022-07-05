#!/bin/bash
shopt -s extglob
curdir="${PWD##*/}"
if [[ $curdir != "script" ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi

RUN_NBSPHINX=OFF
if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
  else
    if [ "$1" == "nbsphinx" ]
      then
        RUN_NBSPHINX=ON
    fi
fi

./removeCMakeBuildFiles.sh

cd ../build
rm -rf ./doc
cmake .. -DBUILD_SPHINX=ON -DNBSPHINX_EXECUTE=$RUN_NBSPHINX -DBUILD_LIB=OFF -DBUILD_GTESTALL=OFF
cmake --build . --config Release