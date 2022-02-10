#!/bin/bash
# set -e
shopt -s extglob
cd ../build
if [[ $? != 0 ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
rm ./*.log
rm ../*.log
rm ../test/pytest/*.log
rm ../doc/source/demo/*.log
rm ../script/*.log
rm ../python-package/parfis/clib/*.so