#include <cassert>
#include <iostream>

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
        return std::unique_ptr<NumberExprNode>(new NumberExprNode(token->position(), token->number()));
    }

    std::unique_ptr<ExprNode> parseParenExpr(Iter& it)
    {
        CharToken* t1 = dynamic_cast<CharToken*>(it->get());
        if (!t1 || t1->ch() != '(')
            throw ParseError((*it)->position(), "expected '('");

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

        return std::unique_ptr<CallExprNode>(new CallExprNode(position, funcName, std::move(args)));
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

        return std::unique_ptr<VariableExprNode>(new VariableExprNode(t1->position(), t1->name()));
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

    int getPrecedence(CharToken* token)
    {
        if (!token)
            return -1;
        switch (token->ch()) {
        case '<': return 10;
        case '+': return 20;
        case '-': return 20;
        case '*': return 40;
        }
        return -1;
    }

    std::unique_ptr<ExprNode> parseBinaryExprRhs(Iter& it, int minPrecedence, std::unique_ptr<ExprNode> lhs)
    {
        CharToken* t1 = dynamic_cast<CharToken*>(it->get());
        int precedence1 = getPrecedence(t1);
        if (precedence1 < minPrecedence)
            return std::move(lhs);
        Operator op = t1->ch();
        ++it;

        auto rhs1 = parsePrimary(it);

        CharToken* t2 = dynamic_cast<CharToken*>(it->get());
        int precedence2 = getPrecedence(t2);
        if (precedence1 >= precedence2) {
            auto lhs2 = std::unique_ptr<BinaryExprNode>(new BinaryExprNode(t1->position(), op, std::move(lhs), std::move(rhs1)));
            return parseBinaryExprRhs(it, minPrecedence, std::move(lhs2));
        } else {
            auto rhs2 = parseBinaryExprRhs(it, precedence1 + 1, std::move(rhs1));
            auto lhs2 = std::unique_ptr<BinaryExprNode>(new BinaryExprNode(t1->position(), op, std::move(lhs), std::move(rhs2)));
            return parseBinaryExprRhs(it, minPrecedence, std::move(lhs2));
        }
    }

    std::unique_ptr<PrototypeNode> parsePrototype(Iter& it)
    {
        IdentifierToken* t1 = dynamic_cast<IdentifierToken*>(it->get());
        if (!t1)
            throw ParseError((*it)->position(), "expected function name in prototype");
        ++it;

        CharToken* t2 = dynamic_cast<CharToken*>(it->get());
        if (!t2 || t2->ch() != '(')
            throw ParseError((*it)->position(), "expected '(' in prototype");
        ++it;

        std::vector<std::string> args;
        while (IdentifierToken* t3 = dynamic_cast<IdentifierToken*>(it->get())) {
            args.push_back(t3->name());
            ++it;

            CharToken* t4 = dynamic_cast<CharToken*>(it->get());
            if (!t4 || t4->ch() != ',')
                break;
            ++it;
        }

        CharToken* t5 = dynamic_cast<CharToken*>(it->get());
        if (!t5 || t5->ch() != ')')
            throw ParseError((*it)->position(), "expected ')' in prototype");
        ++it;

        return std::unique_ptr<PrototypeNode>(new PrototypeNode(t1->position(), t1->name(), std::move(args)));
    }

    std::unique_ptr<FunctionNode> parseDefinition(Iter& it)
    {
        DefToken* t1 = dynamic_cast<DefToken*>(it->get());
        if (!t1)
            throw ParseError((*it)->position(), "expected 'def'");
        ++it;
        auto proto = parsePrototype(it);
        auto body = parseExpr(it);
        return std::unique_ptr<FunctionNode>(new FunctionNode(std::move(proto), std::move(body)));
    }

    std::unique_ptr<PrototypeNode> parseExtern(Iter& it)
    {
        ExternToken* t1 = dynamic_cast<ExternToken*>(it->get());
        if (!t1)
            throw ParseError((*it)->position(), "expected 'extern'");
        ++it;
        return parsePrototype(it);
    }

    std::unique_ptr<FunctionNode> parseTopLevelExpr(Iter& it)
    {
        const Position& pos = (*it)->position();
        auto expr = parseExpr(it);
        auto proto = std::unique_ptr<PrototypeNode>(new PrototypeNode(pos, "", std::vector<std::string>()));
        return std::unique_ptr<FunctionNode>(new FunctionNode(std::move(proto), std::move(expr)));
    }

    bool parseOneAndPrint(Iter& it)
    {
        if (dynamic_cast<EofToken*>(it->get())) {
            return true;
        }

        if (dynamic_cast<DefToken*>(it->get())) {
            std::unique_ptr<FunctionNode> func = parseDefinition(it);
            std::cout << "definition of " << func->prototype()->name() << std::endl;
            return false;
        }

        if (dynamic_cast<ExternToken*>(it->get())) {
            std::unique_ptr<PrototypeNode> proto = parseExtern(it);
            std::cout << "prototype of " << proto->name() << std::endl;
            return false;
        }

        if (CharToken* t = dynamic_cast<CharToken*>(it->get())) {
            if (t->ch() == ';') {
                ++it;
                return false;
            }
        }

        std::unique_ptr<FunctionNode> func = parseTopLevelExpr(it);
        std::cout << "toplevel" << std::endl;
        return false;
    }

}   // namespace anonymous


namespace kaleidoscope {

    std::unique_ptr<ExprNode> parseExpr(Iter& it)
    {
        std::unique_ptr<ExprNode> lhs = parsePrimary(it);
        return parseBinaryExprRhs(it, 0, std::move(lhs));
    }


    void parseAndPrint(Iter& it)
    {
        for (;;) {
            try {
                bool finish = parseOneAndPrint(it);
                if (finish)
                    return;
            } catch (const ParseError& e) {
                std::cerr << e.what() << std::endl;
                // discard tokens until next ';'
                for (;;) {
                    CharToken* t = dynamic_cast<CharToken*>(it->get());
                    ++it;
                    if (t && t->ch() == ';')
                        break;
                }
            }
        }
    }

}   // namespace kaleidoscope