#!/bin/bash
shopt -s extglob
curdir="${PWD##*/}"
if [[ $curdir != "script" ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
./removeLogFiles.sh
./removeCMakeBuildFiles.sh
cd ../build
echo "Remove library files from /build/lib/parfis and /python-package/parfis/clib"
rm -rf ./lib/parfis/* ../python-package/parfis/clib/*.so ../python-package/parfis/clib/*.dll
echo "Build float release"
cmake .. -DBUILD_DEBUG=OFF -DCOPY_LIB_PYTHON=ON -DPARFIS_STATE_TYPE_DOUBLE=OFF -DBUILD_GTESTALL=ON
cmake --build . --config Release
./bin/gtestAll/gtestAll

cd ../script
./removeCMakeBuildFiles.sh
cd ../build
echo "Build double release"
cmake .. -DBUILD_DEBUG=OFF -DCOPY_LIB_PYTHON=ON -DPARFIS_STATE_TYPE_DOUBLE=ON -DBUILD_GTESTALL=ON
cmake --build . --config Release
./bin/gtestAll/gtestAll