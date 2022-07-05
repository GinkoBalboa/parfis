#!/bin/bash
shopt -s extglob
cd ../build
if [[ $? != 0 ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
echo "Remove all binary files"
rm -rf ./lib/* ./bin/* ../python-package/parfis/clib/*.so ../python-package/parfis/clib/*.dll