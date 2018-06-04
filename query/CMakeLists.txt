project(query)

file(GLOB_RECURSE sources_test test/*.cpp)


add_custom_target(Lexer.cpp ALL COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/src/Scanner.l)
add_custom_target(parser.tab.cc ALL COMMAND bison -d ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.yy)

add_executable(a.out src/main.cpp parser.tab.cc Lexer.cpp)
add_dependencies(a.out Lexer.cpp)
add_dependencies(a.out parser.tab.cc)
add_library(query SHARED parser.tab.cc Lexer.cpp)

target_include_directories(query PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} include)
target_include_directories(a.out PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} include)
target_include_directories(a.out PUBLIC $ENV{BOOST_DIR})
target_include_directories(query PUBLIC $ENV{BOOST_DIR})


set_target_properties(query PROPERTIES ENABLE_EXPORTS ON)

set(GTEST_LIBS_DIR $ENV{GOOGLETEST_LIBS_DIR})
set(GTEST_INCLUDE_DIR $ENV{GOOGLETEST_INCLUDE_DIR})
add_library(gtest STATIC IMPORTED)
add_library(gtest_main STATIC IMPORTED)
set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${GTEST_LIBS_DIR}/libgtest.a)
set_target_properties(gtest_main PROPERTIES IMPORTED_LOCATION ${GTEST_LIBS_DIR}/libgtest_main.a)
foreach(file ${sources_test})
    get_filename_component(TEST_FILE ${file} NAME_WE)   
    add_executable(${TEST_FILE} ${file})
    target_include_directories(${TEST_FILE} PUBLIC ${GTEST_INCLUDE_DIR} include)
    # Can't add as a library an executable. That's why
    # we created libserver
    target_link_libraries(${TEST_FILE} gtest gtest_main pthread query)
    add_test(NAME ${TEST_FILE} COMMAND ${CMAKE_BINARY_DIR}/query/${TEST_FILE})
endforeach() 
