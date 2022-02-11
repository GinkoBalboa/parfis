#!/bin/bash
shopt -s extglob
cd ../build
if [[ $? != 0 ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
echo "Removing *.log files from the usual locations"
rm ./*.log
rm ../*.log
rm ../test/pytest/*.log
rm ../doc/sphinx/source/demo_files/*.log
rm ../script/*.log