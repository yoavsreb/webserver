#!/bin/bash
set -e
rm -rf target/*
export GOOGLETEST_LIBS_DIR=`pwd`/../googletest/build/googletest
export GOOGLETEST_INCLUDE_DIR=`pwd`/../googletest/googletest/include
export BOOST_DIR=~/opt/boost_1_61_0
cmake . -Btarget
cd target
make VERBOSE=1
make test

