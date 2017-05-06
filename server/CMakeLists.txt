### File Globbing ##############################################################
###############################################################################

# these instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables 
# `sources` and `data`
file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE sources_test test/*.cpp)
file(GLOB_RECURSE data resources/*)
# you can use set(sources src/main.cpp) etc if you don't want to
# use globing to find files automatically

###############################################################################
## target definitions #########################################################
###############################################################################

# add the data to the target, so it becomes visible in some IDE
add_executable(server ${sources})

#libserver will be used for tests mostly. Other than that,
# all configuration should probably be identical to 'server'
add_library(libserver ${sources})

# just for example add some compiler flags
#target_compile_options(example PUBLIC -std=c++1y -Wall -Wfloat-conversion)

# this lets me include files relative to the root src dir with a <> pair
target_include_directories(server PUBLIC include)
target_include_directories(server PUBLIC $ENV{BOOST_DIR})
target_include_directories(server PUBLIC $ENV{CAPNP_INCLUDE_DIR})
target_include_directories(libserver PUBLIC include)
target_include_directories(libserver PUBLIC $ENV{BOOST_DIR})
target_include_directories(libserver PUBLIC $ENV{CAPNP_INCLUDE_DIR})
# this copies all resource files in the build directory
# we need this, because we want to work with paths relative to the executable
#file(COPY ${data} DESTINATION resources)
set_target_properties(server PROPERTIES ENABLE_EXPORTS ON)
set_target_properties(libserver PROPERTIES ENABLE_EXPORTS ON)
###############################################################################
## dependencies ###############################################################
###############################################################################

# this defines the variables Boost_LIBRARIES that contain all library names
# that we need to link to
#find_package(Boost 1.36.0 COMPONENTS filesystem system REQUIRED)
MESSAGE(STATUS "BOOST: $ENV{BOOST_DIR}/stage/lib")
set(BOOST_LIB $ENV{BOOST_DIR}/stage/lib)
find_library(BOOST_FS boost_filesystem HINTS ${BOOST_LIB} )
find_library(BOOST_SYSTEM boost_system HINTS ${BOOST_LIB} )
find_library(BOOST_PO boost_program_options HINTS ${BOOST_LIB} )
link_libraries("$ENV{BOOST_DIR}/stage/lib")
target_link_libraries(server PUBLIC
	  ${BOOST_FS} ${BOOST_SYSTEM} ${BOOST_PO} pthread webserver_capnp kj capnp capnpc)
	   
        # here you can add any library dependencies
target_link_libraries(libserver PUBLIC
	  ${BOOST_FS} ${BOOST_SYSTEM})
 ###############################################################################
 ## testing ####################################################################
 ###############################################################################

 # this is for our testing framework
 # we don't add REQUIRED because it's just for testing
 # set(GTEST_LIBS_DIR /home/yoav/CppProjects/googletest/build/googletest)
 # set(GTEST_INCLUDE_DIR /home/yoav/CppProjects/googletest/googletest/include)
set(GTEST_LIBS_DIR $ENV{GOOGLETEST_LIBS_DIR})
set(GTEST_INCLUDE_DIR $ENV{GOOGLETEST_INCLUDE_DIR})
add_library(gtest STATIC IMPORTED)
add_library(gtest_main STATIC IMPORTED)
set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${GTEST_LIBS_DIR}/libgtest.a)
set_target_properties(gtest_main PROPERTIES IMPORTED_LOCATION ${GTEST_LIBS_DIR}/libgtest_main.a)
foreach(file ${sources_test})
    get_filename_component(TEST_FILE ${file} NAME_WE)   
    add_executable(${TEST_FILE} ${file})
    target_include_directories(${TEST_FILE} PUBLIC ${GTEST_INCLUDE_DIR})
    # Can't add as a library an executable. That's why
    # we created libserver
    target_link_libraries(${TEST_FILE} gtest gtest_main pthread libserver webserver_capnp kj capnp capnpc)
    MESSAGE(STATUS "COMMAND:  ${CMAKE_BINARY_DIR}/server/${TEST_FILE}") 
    add_test(NAME ${TEST_FILE} COMMAND ${CMAKE_BINARY_DIR}/server/${TEST_FILE})
endforeach() 

