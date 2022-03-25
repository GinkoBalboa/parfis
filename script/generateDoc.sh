#!/bin/bash
shopt -s extglob
curdir="${PWD##*/}"
if [[ $curdir != "script" ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
./removeCMakeBuildFiles.sh
./buildRelease32_64.sh
cd ../python-package
python -m pip install -e . --force-reinstall
cd ../build
rm -rf ./doc
cmake .. -DBUILD_SPHINX=ON -DBUILD_LIB=OFF
cmake --build . --config Release