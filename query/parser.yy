%{
#include <cstdio>
#include <iostream>
#include <string>


#include "AbstractLanguage.hpp"

#include "parser.tab.hh"
#include "Lexer.hpp"

int yyerror( ParsingNode **root, yyscan_t scanner, const char *msg) {
    std::cout << "Error in yyparse" << std::endl;
}

%}
%code requires {
#define YYSTYPE ParsingNode

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
}

%define api.pure true

%parse-param {ParsingNode **root}
%parse-param {yyscan_t scanner}
%lex-param {yyscan_t scanner}


%token PLUS MULTIPLY LPAREN RPAREN
%token NUMBER 
%token UUID


%%
program:
    wrapped_scalar { 
        *root = new ParsingNode($1);
    }  | scalar { 
        *root = new ParsingNode($1); 
    };
wrapped_scalar:
       LPAREN scalar RPAREN {
    $$.pExpression = std::make_shared<Expression>(ExpressionType::WRAPPED_EXPR, $2.pExpression);
     }
    ;
scalar:
        NUMBER {
    $$.pExpression = std::make_shared<Expression>($1.pToken); 
        }
    ;

        
      
%%





int main(int, char**) {
    /**
        Change to c'tor with shared_ptr
        Change YYSTYPE to be the ParsingNode 
        the Lexer only uses the tokens, The parser builds tree of of Expression.
        ParsingNode contains only 2 shared_ptr therefore is easily copyable without significant penality.
    */
     
    /*
    Token token1{TokenType::NUMBER, 5};
    Token token2{TokenType::NUMBER, 6};
    Token token3{TokenType::NUMBER, 7};   
     
    Expression exp1(token1);
    Expression exp2(token2);
    Expression exp3(token3);
    
    Expression exp4{ExpressionType::ADD_EXPR, exp1, exp2};
    Expression exp5{ExpressionType::ADD_EXPR, exp4, exp3};
    PrintVisitor visitor;
    exp5.accept(visitor);
    */
    
    std::cout << "Starting to parse input" << std::endl;
    yyscan_t scanner;
    yylex_init(&scanner);
    ParsingNode* root;

    do {
        yyparse(&root, scanner);
        PrintVisitor visitor;
        if (root->pExpression == nullptr) {
            std::cout << "root pexpression is null " << std::endl; } else {
            root->pExpression->accept(visitor);
            delete root;
            root = nullptr;
        }
    } while(!feof(stdin));
    yylex_destroy(scanner);
    return 0;
}
