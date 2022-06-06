#!/bin/bash
shopt -s extglob
curdir="${PWD##*/}"
if [[ $curdir != "script" ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
./removeLogFiles.sh
echo "Run python tests"
cd ..
python3 ./test/pytest/runAll.py