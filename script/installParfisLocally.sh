#!/bin/bash
shopt -s extglob
curdir="${PWD##*/}"
if [[ $curdir != "script" ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
./removeLogFiles.sh
echo "Install parfis locally"
cd ../python-package
pip install -e . --force-reinstall -v