#!/bin/bash
# set -e
cd ../build
if [[ $? != 0 ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
rm -rf ./* ./.cmake
rm ../*.log
rm ../python-package/parfis/libparfis.so
cmake .. -DCOPY_LIB_PYTHON=ON
cmake --build . --config Release