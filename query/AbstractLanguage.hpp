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
    SCALAR,
    WRAPPED_EXPR,
    MULT_EXPR,
    ADD_EXPR
};

struct Expression;

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
        if (t.type == TokenType::NUMBER) {
            type = ExpressionType::SCALAR;
        } else {
            throw std::runtime_error("Can't create expression with token");
        }
        pToken = std::shared_ptr<Token>(new Token(t));
    }
    
    Expression(std::shared_ptr<Token> t) : pToken(t) {
        if (t->type == TokenType::NUMBER) {
            type = ExpressionType::SCALAR;
        } else {
            throw std::runtime_error("Can't create expression with token");
        }
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

struct ParsingNode {
    std::shared_ptr<Token> pToken;
    std::shared_ptr<Expression> pExpression;

    ParsingNode(const Token& token) : pExpression(nullptr) {
        pToken = std::shared_ptr<Token>(new Token(token));
    }

    ParsingNode(const Expression& expr) : pToken(nullptr) {
        pExpression = std::shared_ptr<Expression>(new Expression(expr));
    }

    ParsingNode() : pToken(nullptr), pExpression(nullptr) {}
};



class PrintVisitor : public Visitor 
{
public:
    void visit(ExpressionType t,  const std::vector<std::shared_ptr<Expression>>& subs) override {
    switch(t) {
        case ExpressionType::ADD_EXPR: std::cout << "Add Operation" << std::endl; break;
        case ExpressionType::WRAPPED_EXPR:  std::cout << "Wrapped Expression" << std::endl; break;
        default: std::cout << "Not expected" << std::endl;
    }
    for_each(subs.begin(), subs.end(), [this]( std::shared_ptr<Expression> exp) {  exp->accept(*this);
            });
}
    void visit(const Token& t) override {
        std::cout << t.value << std::endl;
    }
};

#endif
