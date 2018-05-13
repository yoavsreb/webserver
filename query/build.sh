rm a.out
rm Lexer.*
rm parser.tab.cc
rm parser.tab.hh
flex Scanner.l
bison -d parser.yy
g++ -g -std=c++11 parser.tab.cc Lexer.cpp -I /home/yoav/opt/boost_1_61_0/

