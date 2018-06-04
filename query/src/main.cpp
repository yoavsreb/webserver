#include <cstdio>
#include <iostream>
#include <string>


#include "AbstractLanguage.hpp"
#include "parser.tab.hh"
#include "Lexer.hpp"


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
