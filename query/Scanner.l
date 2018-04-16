%{
 // cal analyzer run: "flex Lexer.l"

#include <stdio.h>

#include "AbstractLanguage.hpp"

#define YYSTYPE Token

#include "parser.yy.tab.h"

//extern "C" YYSTYPE yylval;

int yyerror(const char* msg);

//YYSTYPE yylval;
%}
%option noyywrap
%option outfile="Lexer.cpp" header-file="Lexer.hpp"
%option warn nodefault
   
LPAREN      "("
RPAREN      ")"
PLUS        "+"
MULTIPLY    "*"

UUID        [0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}
NUMBER      [0-9]+
WS          [ \r\n\t]*

%%

{WS}            { /* Skip blanks. */ }
{NUMBER}        { int temp; sscanf(yytext, "%d", &temp); yylval.value = temp; return NUMBER;}
{UUID}          {
    printf("Found UUID\n");
    }
{MULTIPLY}      { 
    printf("Found multiply\n");
    return MULTIPLY; 
    }
{PLUS}          { 
    printf("Found plus\n");
    return PLUS; 
    }
{LPAREN}        { 
    printf("Found lparen\n");
    return LPAREN; 
    }
{RPAREN}        { 
    printf("Found rparen\n");
    return RPAREN; 
    }
.               {  
    printf("Found other\n");
    }
%%

int yyerror(const char *msg) {
    fprintf(stderr,"Error:%s\n",msg); return 0;
}

yyFlexLexer lexer;

int yylex() {
    return lexer.yylex();
}

/*
    This is only used to check tokenizations
 */
/*
int main() {
    yyFlexLexer lexer;
    lexer.yylex();
    return 0;
}*/
