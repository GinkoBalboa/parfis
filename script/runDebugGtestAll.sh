#!/bin/bash
shopt -s extglob
curdir="${PWD##*/}"
if [[ $curdir != "script" ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
echo "Run debug cpp tests"
cd ..
./build/bin/gtestAll/gtestAlld