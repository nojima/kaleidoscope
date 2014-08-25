#include <cassert>

#include "ast.hpp"
#include "token.hpp"

namespace {

    using namespace kaleidoscope;
    typedef std::vector<std::unique_ptr<Token>>::iterator Iter;

    std::unique_ptr<ExprNode> parseNumberExpr(Iter& it)
    {
        NumberToken* token = dynamic_cast<NumberToken*>(it->get());
        assert(token);
        ++it;
        return std::make_unique<NumberExprNode>(token->position(), token->number());
    }

    std::unique_ptr<ExprNode> parseParenExpr(Iter& it)
    {
        CharToken* t1 = dynamic_cast<CharToken*>(it->get());
        assert(t1 && t1->ch() == '(');

        std::unique_ptr<ExprNode> ret = parseExpr(it);

        CharToken* t2 = dynamic_cast<CharToken*>(it->get());
        if (t2 || t2->ch() != ')')
            throw ParseError((*it)->position(), "expected ')'");
        ++it;

        return std::move(ret);
    }

    std::unique_ptr<ExprNode> parseFunctionCall(Iter& it, const Position& position, const std::string& funcName)
    {
        std::vector<std::unique_ptr<ExprNode>> args;
        for (;;) {
            args.push_back(parseExpr(it));

            CharToken* token = dynamic_cast<CharToken*>(it->get());
            if (!token || (token->ch() != ')' && token->ch() != ','))
                throw ParseError((*it)->position(), "expected ')' or ',' in argument list");
            ++it;
            if (token->ch() == ')')
                break;
        }

        return std::make_unique<CallExprNode>(position, funcName, std::move(args));
    }

    std::unique_ptr<ExprNode> parseIdentifierExpr(Iter& it)
    {
        IdentifierToken* t1 = dynamic_cast<IdentifierToken*>(it->get());
        assert(t1);
        ++it;

        if (CharToken* t2 = dynamic_cast<CharToken*>(it->get())) {
            if (t2->ch() == '(') {
                ++it;
                return parseFunctionCall(it, t1->position(), t1->name());
            }
        }

        return std::make_unique<VariableExprNode>(t1->position(), t1->name());
    }

    std::unique_ptr<ExprNode> parsePrimary(Iter& it)
    {
        if (dynamic_cast<IdentifierToken*>(it->get()))
            return parseIdentifierExpr(it);

        if (dynamic_cast<NumberToken*>(it->get()))
            return parseNumberExpr(it);

        if (CharToken* t = dynamic_cast<CharToken*>(it->get()))
            if (t->ch() == '(')
                return parseParenExpr(it);

        throw ParseError((*it)->position(), "unknown token when expecting an expression");
    }

}   // namespace anonymous


namespace kaleidoscope {

    std::unique_ptr<ExprNode> parseExpr(Iter& it)
    {
    }

}   // namespace kaleidoscope
