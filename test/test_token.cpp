#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "token.hpp"

using namespace kaleidoscope;

std::string gFileName("test");

std::vector<std::unique_ptr<Token>>
tokenizeString(const std::string& str)
{
    std::istringstream stream(str);
    std::istreambuf_iterator<char> first(stream);
    std::istreambuf_iterator<char> last;
    return tokenize(first, last, gFileName);
}

void assertNumberToken(const Token* token, double number)
{
    auto t = dynamic_cast<const NumberToken*>(token);
    ASSERT_NE(nullptr, t);
    EXPECT_EQ(number, t->number());
}

void assertCharToken(const Token* token, char ch)
{
    auto t = dynamic_cast<const CharToken*>(token);
    ASSERT_NE(nullptr, t);
    EXPECT_EQ(ch, t->ch());
}

void assertEofToken(const Token* token)
{
    auto t = dynamic_cast<const EofToken*>(token);
    ASSERT_NE(nullptr, t);
}

void assertDefToken(const Token* token)
{
    auto t = dynamic_cast<const DefToken*>(token);
    ASSERT_NE(nullptr, t);
}

void assertExternToken(const Token* token)
{
    auto t = dynamic_cast<const ExternToken*>(token);
    ASSERT_NE(nullptr, t);
}

void assertIdentifierToken(const Token* token, const std::string& name)
{
    auto t = dynamic_cast<const IdentifierToken*>(token);
    ASSERT_NE(nullptr, t);
    EXPECT_EQ(name, t->name());
}

TEST(TokenizeTest, OneNumber) {
    auto tokens = tokenizeString("42");
    assertNumberToken(tokens[0].get(), 42);
    assertEofToken(tokens[1].get());
}

TEST(TokenizeTest, BinaryExpression) {
    auto tokens = tokenizeString("42 + 3.14");
    assertNumberToken(tokens[0].get(), 42);
    assertCharToken(tokens[1].get(), '+');
    assertNumberToken(tokens[2].get(), 3.14);
    assertEofToken(tokens[3].get());
}

TEST(TokenizeTest, DefineFunction) {
    auto tokens = tokenizeString("def foo(x) x + 1");
    assertDefToken(tokens[0].get());
    assertIdentifierToken(tokens[1].get(), "foo");
    assertCharToken(tokens[2].get(), '(');
    assertIdentifierToken(tokens[3].get(), "x");
    assertCharToken(tokens[4].get(), ')');
    assertIdentifierToken(tokens[5].get(), "x");
    assertCharToken(tokens[6].get(), '+');
    assertNumberToken(tokens[7].get(), 1);
}

TEST(TokenizeTest, Prototype) {
    auto tokens = tokenizeString("extern foo(x)");
    assertExternToken(tokens[0].get());
    assertIdentifierToken(tokens[1].get(), "foo");
    assertCharToken(tokens[2].get(), '(');
    assertIdentifierToken(tokens[3].get(), "x");
    assertCharToken(tokens[4].get(), ')');
}
