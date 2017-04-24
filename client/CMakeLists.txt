file(GLOB_RECURSE sources *.cpp)
add_executable(client ${sources})
target_include_directories(client PUBLIC $ENV{BOOST_DIR})
target_include_directories(client PUBLIC $ENV{CAPNP_INCLUDE_DIR})
set_target_properties(client PROPERTIES ENABLE_EXPORTS ON)

MESSAGE(STATUS "BOOST: $ENV{BOOST_DIR}/stage/lib")
set(BOOST_LIB $ENV{BOOST_DIR}/stage/lib)
find_library(BOOST_PO boost_program_options HINTS ${BOOST_LIB} )
link_libraries("$ENV{BOOST_DIR}/stage/lib")
target_link_libraries(client PUBLIC
	  ${BOOST_PO} pthread webserver_capnp kj capnp capnpc)
	    # here you can add any library dependencies
	    #	    )


