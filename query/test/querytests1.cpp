#include "gtest/gtest.h"
#include "AbstractLanguage.hpp"

extern std::unique_ptr<ParsingNode> parseString(const std::string& text);

TEST(QueryTests, QueryTest1) {
    EXPECT_EQ(1, 1);
}

TEST(QueryTests, QueryTest2) {
    auto ptr = parseString("{Node v1 | v1[v2]}");
    EXPECT_TRUE(ptr.get() != nullptr);
    EXPECT_TRUE(ptr->pExpression->type == ExpressionType::REF_ATTR_EXPR);
    EXPECT_TRUE(ptr->pDeclarations->type == ExpressionType::DECLS);
    EXPECT_TRUE(ptr->pDeclarations->subExpr.size() == 1);
    EXPECT_TRUE(ptr->pDeclarations->subExpr[0]->type == ExpressionType::DECL_NODE);
    ptr.reset();
}
