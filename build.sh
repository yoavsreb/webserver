#!/bin/bash
set -e
rm -rf target/*
export GOOGLETEST_LIBS_DIR=/home/yoav/CppProjects/googletest/build/googletest
export GOOGLETEST_INCLUDE_DIR=/home/yoav/CppProjects/googletest/googletest/include
export BOOST_DIR=/home/yoav/opt/boost_1_61_0
cmake . -Btarget
cd target
make VERBOSE=1
make test

