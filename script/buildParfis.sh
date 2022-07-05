#!/bin/bash
shopt -s extglob
curdir="${PWD##*/}"
if [[ $curdir != "script" ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi

CONFIG=Release
DOUBLE_STATE=ON
BUILD_GTESTALL=OFF
if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
  else
    if [ "$1" == "Debug" ] || [ "$2" == "Debug" ] || [ "$3" == "Debug" ]
      then
        CONFIG=Debug
    fi
    if [ $1 == "float" ] || [ $2 == "float" ] || [ "$3" == "float" ]
      then
        DOUBLE_STATE=OFF
    fi
    if [ $1 == "gtestAll" ] || [ $2 == "gtestAll" ] || [ "$3" == "gtestAll" ]
      then
        BUILD_GTESTALL=ON
    fi
fi

./removeCMakeBuildFiles.sh

cd ../build

if [ $CONFIG == "Release" ]
  then
    echo "Build parfis Release, double=$DOUBLE_STATE, gtestAll=$BUILD_GTESTALL"
    cmake .. -DCOPY_LIB_PYTHON=ON -DPARFIS_STATE_TYPE_DOUBLE=$DOUBLE_STATE -DBUILD_DEBUG=OFF -DBUILD_GTESTALL=$BUILD_GTESTALL
    cmake --build . --config Release
fi

if [ $CONFIG == "Debug" ]
  then
    echo "Build parfis Debug, double=$DOUBLE_STATE, gtestAll=$BUILD_GTESTALL"
    cmake .. -DCOPY_LIB_PYTHON=ON -DPARFIS_STATE_TYPE_DOUBLE=$DOUBLE_STATE -DBUILD_DEBUG=ON -DBUILD_GTESTALL=$BUILD_GTESTALL
    cmake --build . --config Debug
fi