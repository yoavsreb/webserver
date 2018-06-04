#ifndef _QUERY_ABSTRACT_LANGUAGE_HPP
#define _QUERY_ABSTRACT_LANGUAGE_HPP

#include <boost/variant.hpp>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

enum class TokenType {
    NONE = 0,
    NUMBER,
    UUID,
    IDENTIFIER,
};

struct Token {
    TokenType type;
    boost::variant<int, std::string> value;

    template<typename T>
    Token(TokenType t, const T& v) : type(t), value(v) {}

    Token() : type(TokenType::NONE), value(-1) {}
};



enum class ExpressionType {
    SCALAR, // holds a terminal value/token - string, uuid, number. This is the type of expression that doesn't contain any sub-expressions.
    WRAPPED_EXPR,
    REF_ATTR_EXPR,
    MULT_EXPR,
    ADD_EXPR,
    DECL_NODE,
    DECLS
};

struct Expression;

/**
 * A visitor interface to visit the tree.
 */
class Visitor {
public:
    virtual void visit(ExpressionType expType, const std::vector<std::shared_ptr<Expression>>& subexprs) = 0;
    virtual void visit(const Token& token) = 0;
};


struct Expression {
    ExpressionType type;
    std::vector<std::shared_ptr<Expression>> subExpr;
    std::shared_ptr<Token> pToken;

    template<typename ...Ts> 
    Expression(ExpressionType t, Ts... subexprs) : 
        type(t), pToken(nullptr) {
            addSubExpr(subexprs...);
        }

    Expression(const Token& t) {
        type = ExpressionType::SCALAR;
        pToken = std::shared_ptr<Token>(new Token(t));
    }
    
    Expression(std::shared_ptr<Token> t) : pToken(t) {
        type = ExpressionType::SCALAR;
    }

    void accept(Visitor& visitor) const {
        if (type == ExpressionType::SCALAR) {
            visitor.visit(*pToken);
        } else {
            visitor.visit(type, subExpr);    
        }
    }


private:
    template<typename ...Ts>
    void addSubExpr(std::shared_ptr<Expression> val, Ts... vals) {
        subExpr.push_back(val);
        addSubExpr(vals...);
    } 
    void addSubExpr(std::shared_ptr<Expression> val) {
        subExpr.push_back(val);
    }

    template<typename ...Ts>
    void addSubExpr(const Expression& val, Ts... vals) {
        subExpr.push_back(std::shared_ptr<Expression>(new Expression(val)));
        addSubExpr(vals...);
    }

    void addSubExpr(const Expression& val) {
        subExpr.push_back(std::shared_ptr<Expression>(new Expression(val)));
    }
};

/**
 * A simple struct used in building the AST
 */
struct ParsingNode {
    std::shared_ptr<Token> pToken;
    std::shared_ptr<Expression> pExpression;
    std::shared_ptr<Expression> pDeclarations;

    ParsingNode(const Token& token) : pExpression(nullptr), pDeclarations(nullptr) {
        pToken = std::shared_ptr<Token>(new Token(token));
    }

    ParsingNode(const Expression& expr) : pToken(nullptr), pDeclarations(nullptr) {
        pExpression = std::shared_ptr<Expression>(new Expression(expr));
    }

    ParsingNode(const Expression& decls, const Expression& expr) : pToken(nullptr) {
        pExpression = std::shared_ptr<Expression>(new Expression(expr));
        pDeclarations = std::shared_ptr<Expression>(new Expression(decls));
    }

    ParsingNode() : pToken(nullptr), pExpression(nullptr) {}
};


/**
 * A simple visitor implemetor for printing trees.
 */
class PrintVisitor : public Visitor 
{
private:
    int mIndent = 0;
public:
    void visit(ExpressionType t,  const std::vector<std::shared_ptr<Expression>>& subs) override {
    indent();
    switch(t) {
        case ExpressionType::ADD_EXPR: std::cout << "Add Operation" << std::endl; break;
        case ExpressionType::WRAPPED_EXPR:  std::cout << "Wrapped Expression" << std::endl; break;
        case ExpressionType::DECL_NODE: std::cout <<"Declare node " << std::endl; break;
        case ExpressionType::DECLS:     std::cout << "Declarations: " << std:: endl; break;
        case ExpressionType::REF_ATTR_EXPR: std::cout << "Reference expression" << std::endl; break;
        default: std::cout << "Not expected" << std::endl;
    }
    push();
    for_each(subs.begin(), subs.end(), [this]( std::shared_ptr<Expression> exp) {  exp->accept(*this);
            });
    pop();
}
    void visit(const Token& t) override {
        indent();
        std::cout << t.value << std::endl;
    }

    void push() {
        mIndent++;
    }

    void pop() {
        mIndent--;
    }

    void indent() const {
        for (auto i = 0; i < mIndent; i++) {
            std::cout <<"  ";
        }
    }
};

#endif
