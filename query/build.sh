rm a.out
rm Lexer.*
rm parser.tab.cc
rm parser.tab.hh
flex src/Scanner.l
bison -d src/parser.yy
g++ -g -std=c++11 src/main.cpp parser.tab.cc Lexer.cpp -I /home/yoav/opt/boost_1_61_0/ -I`pwd`/include -I`pwd`

