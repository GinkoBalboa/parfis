#!/bin/bash
# set -e
cd ../build
if [[ $? != 0 ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
rm -rf ./* ./.cmake
rm ../*.log
rm ../python-package/parfis/libparfisd.so
cmake .. -DCOPY_LIB_PYTHON=ON -DBUILD_DEBUG=ON
cmake --build . --config Debug
./bin/gtestAll/gtestAlld