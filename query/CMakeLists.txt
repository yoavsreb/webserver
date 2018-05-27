project(query)

add_custom_target(Lexer.cpp COMMAND flex Scanner.l)
add_custom_target(parser.tab.cc COMMAND bison -d parser.yy)

add_executable(a.out parser.tab.cc Lexer.cpp)
add_library(libquery parser.tab.cc Lexer.cpp)


target_include_directories(a.out PUBLIC $ENV{BOOST_DIR})
target_include_directories(libquery PUBLIC $ENV{BOOST_DIR})


set_target_properties(libquery PROPERTIES ENABLE_EXPORTS ON)
