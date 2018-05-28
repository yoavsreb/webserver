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


%token PLUS MULTIPLY LPAREN RPAREN CLPAREN CRPAREN VERT_SEP COMMA
%token NUMBER 
%token UUID
%token IDENTIFIER NODE_KEYWORD EDGE_KEYWORD

// This is to solve shift/reduce ambiguity (a + b + c ) -> ((a+b) + c)
%left PLUS  

%%
program:
    expression { 
        *root = new ParsingNode($1); 
    } |
    CLPAREN decls VERT_SEP expression CRPAREN {
        *root = new ParsingNode(*($2.pExpression), *($4.pExpression));
    };
decls: decl { $$.pExpression = std::make_shared<Expression>(ExpressionType::DECLS, $1.pExpression); } | 
        decls COMMA decl { $$.pExpression = $1.pExpression;
        $$.pExpression->subExpr.push_back($3.pExpression);
        } |
        %empty {};
decl: NODE_KEYWORD IDENTIFIER {
        $$.pExpression = std::make_shared<Expression>(ExpressionType::DECL_NODE, Expression($2.pToken)); 
    }
expression:
    expression PLUS expression {
        $$.pExpression = std::make_shared<Expression>(ExpressionType::ADD_EXPR, $1.pExpression, $3.pExpression);
    } | 
    LPAREN expression RPAREN {
        $$.pExpression = std::make_shared<Expression>(ExpressionType::WRAPPED_EXPR, $2.pExpression);
    } | scalar {
        $$.pExpression = $1.pExpression;
    };
scalar:
        NUMBER {
    $$.pExpression = std::make_shared<Expression>($1.pToken); 
        }
    ;

        
      
%%

int main(int, char**) {
    std::cout << "Starting to parse input" << std::endl;
    yyscan_t scanner;
    yylex_init(&scanner);
    ParsingNode* root = nullptr;

    do {
        yyparse(&root, scanner);
        PrintVisitor visitor;
        if (!root || root->pExpression == nullptr) {
            std::cout << "root pexpression is null " << std::endl; } else {
            root->pExpression->accept(visitor);
            if (root->pDeclarations != nullptr) {
                std::cout << "Printing declarations: " << std::endl;
                root->pDeclarations->accept(visitor);
            }
            delete root;
            root = nullptr;
        }
    } while(!feof(stdin));
    yylex_destroy(scanner);
    return 0;
}
