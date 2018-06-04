#include "gtest/gtest.h"
#include "AbstractLanguage.hpp"

extern std::unique_ptr<ParsingNode> parseString(const std::string& text);

TEST(QueryTests, QueryTest1) {
    EXPECT_EQ(1, 1);
}

TEST(QueryTests, QueryTest2) {
    auto ptr = parseString("{Node v1 | v1[v2]}");
    EXPECT_TRUE(ptr.get() != nullptr);
    ptr.reset();
}
