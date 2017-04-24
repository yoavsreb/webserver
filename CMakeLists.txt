# WebServer main CMake file

cmake_minimum_required (VERSION 2.8.11)
project (WebServer)
set(CMAKE_CXX_FLAGS "-g -O0 -std=c++11")
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
enable_testing()

add_library(webserver_capnp capnp/RequestResponse.capnp.c++)
add_subdirectory (server)
add_subdirectory (client)
