#!/bin/bash
set -e
rm -rf target/*
export GOOGLETEST_LIBS_DIR=`pwd`/../googletest/build/googletest
export GOOGLETEST_INCLUDE_DIR=`pwd`/../googletest/googletest/include
export BOOST_DIR=~/opt/boost_1_61_0
cd capnp
capnp compile -oc++ RequestResponse.capnp
export CAPNP_INCLUDE_DIR=`pwd`
cd ..
cmake . -Btarget
cd target
make VERBOSE=1
make test

