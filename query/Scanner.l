%{
 // cal analyzer run: "flex Lexer.l"

#include <stdio.h>
#include "AbstractLanguage.hpp"
#include "parser.tab.hh"

%}
%option noyywrap
%option outfile="Lexer.cpp" header-file="Lexer.hpp"
%option warn nodefault
%option reentrant
%option bison-bridge
   
LPAREN      "("
RPAREN      ")"
PLUS        "+"
MULTIPLY    "*"

UUID        [0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}
NUMBER      [0-9]+
WS          [ \r\n\t]*

%%

{WS}            { /* Skip blanks. */ }
{NUMBER}        { int temp; sscanf(yytext, "%d", &temp); yylval_param->pToken = std::shared_ptr<Token>(new Token(TokenType::NUMBER, temp)); return NUMBER;}
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
    return LPAREN; 
    }
{RPAREN}        { 
    return RPAREN; 
    }
.               {  
    printf("Found other\n");
    }
%%

int yyerror(yyscan_t driver, const char *msg) {
    fprintf(stderr,"Error:%s\n",msg); return 0;
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
