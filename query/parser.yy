%{
#include <cstdio>
#include <iostream>
#include <string>

#include "AbstractLanguage.hpp"

extern int yylex();
extern "C" int yyparse();
extern int yyerror(const char *msg);

#define YYSTYPE Token

//#undef yylex
//#define yylex lexer.yylex
%}

/*
%union {
   int iVal;
   char* sVal;
}*/

%token PLUS MULTIPLY LPAREN RPAREN
%token NUMBER 
%token UUID


%%
program:
       LPAREN NUMBER RPAREN {
    std::cout << "Found a sentence: " << $2.value << std::endl;        }
    ;
%%

int main(int, char**) {
    do {
        yyparse();
    } while(!feof(stdin));
}
