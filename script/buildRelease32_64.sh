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
cmake .. -DCOPY_LIB_PYTHON=ON -DPARFIS_STATE_TYPE_DOUBLE=OFF
cmake --build . --config Release
./removeCMakeBuildFiles.sh
echo "Build double release"
cmake .. -DCOPY_LIB_PYTHON=ON -DPARFIS_STATE_TYPE_DOUBLE=ON
cmake --build . --config Release