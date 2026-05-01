#!/bin/bash
rm -rf CUnit-install
mkdir -p CUnit-install
cd CUnit-2-1efb971e0d
rm -rf build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$PWD/../../CUnit-install ../
make
make install
cd ../../
rm -rf CUnit-2-1efb971e0d/build
