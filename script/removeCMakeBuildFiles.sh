#!/bin/bash
shopt -s extglob
cd ../build
if [[ $? != 0 ]]; then
  echo "You must call this script from the folder /parfis/script"
  exit 1
fi
echo "Remove CMake build files from the build folder"
rm -rf ./CMake* 
rm -rf ./Makefile 
rm -rf ./cmake* 
rm -rf ./.cmake*