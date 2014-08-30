#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "ast.hpp"
#include "position.hpp"

using namespace kaleidoscope;

std::string gFileName("test");

TEST(ParseTest, OneNumber) {
    std::vector<std::unique_ptr<Token>> v;
    Position p(gFileName, 1, 1);
    v.emplace_back(new NumberToken(p, 42));
    v.emplace_back(new EofToken(p));
    auto it = v.begin();
    std::unique_ptr<ExprNode> node = parseExpr(it);
    auto n = dynamic_cast<NumberExprNode*>(node.get());
    ASSERT_NE(nullptr, n);
    EXPECT_EQ(42, n->value());
}
