bison -d parser.yy
flex++ Scanner.l
g++ -std=c++11 parser.yy.tab.c Lexer.cpp -I /home/yoav/opt/boost_1_61_0/

