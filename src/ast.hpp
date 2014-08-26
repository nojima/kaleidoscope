#pragma once

#include <memory>
#include <vector>

#include "position.hpp"
#include "token.hpp"

namespace kaleidoscope {

    typedef int Operator;

    class Node {
    public:
        explicit Node(const Position& position): position_(position) {}

        virtual ~Node() {}

        const Position& position() const noexcept {
            return position_;
        }

    private:
        Position position_;
    };


    class ExprNode: public Node {
    public:
        explicit ExprNode(const Position& position): Node(position) {}
    };


    class NumberExprNode: public ExprNode {
    public:
        NumberExprNode(const Position& position, double value):
            ExprNode(position), value_(value) {}

        double value() const noexcept {
            return value_;
        }

    private:
        double value_;
    };


    class VariableExprNode: public ExprNode {
    public:
        VariableExprNode(const Position& position, const std::string& name):
            ExprNode(position), name_(name) {}

        const std::string& name() const noexcept {
            return name_;
        }

    private:
        std::string name_;
    };


    class BinaryExprNode: public ExprNode {
    public:
        BinaryExprNode(const Position& position,
                       Operator op,
                       std::unique_ptr<ExprNode>&& lhs,
                       std::unique_ptr<ExprNode>&& rhs):
            ExprNode(position), op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

        Operator op() const noexcept {
            return op_;
        }

        const ExprNode* lhs() const noexcept {
            return lhs_.get();
        }

        const ExprNode* rhs() const noexcept {
            return rhs_.get();
        }

    private:
        Operator op_;
        std::unique_ptr<ExprNode> lhs_;
        std::unique_ptr<ExprNode> rhs_;
    };


    class CallExprNode: public ExprNode {
    public:
        CallExprNode(const Position& position,
                     const std::string& callee,
                     std::vector<std::unique_ptr<ExprNode>>&& args):
            ExprNode(position), callee_(callee), args_(std::move(args)) {}

        const std::string& callee() const noexcept {
            return callee_;
        }

        const ExprNode* argument(size_t i) const {
            return args_[i].get();
        }

        size_t argumentCount() const noexcept {
            return args_.size();
        }

    private:
        std::string callee_;
        std::vector<std::unique_ptr<ExprNode>> args_;
    };


    class PrototypeNode: public Node {
    public:
        PrototypeNode(const Position& position,
                      const std::string& name,
                      const std::vector<std::string>& args):
            Node(position), name_(name), args_(args) {}

        const std::string& name() const noexcept {
            return name_;
        }

        const std::string& argument(size_t i) const {
            return args_[i];
        }

        size_t argumentCount() const noexcept {
            return args_.size();
        }

    private:
        std::string name_;
        std::vector<std::string> args_;
    };


    class FunctionNode: public Node {
    public:
        FunctionNode(std::unique_ptr<PrototypeNode> proto,
                     std::unique_ptr<ExprNode> body):
            Node(proto->position()), proto_(std::move(proto)), body_(std::move(body)) {}

        const PrototypeNode* prototype() const noexcept {
            return proto_.get();
        }

        const ExprNode* body() const noexcept {
            return body_.get();
        }

    private:
        std::unique_ptr<PrototypeNode> proto_;
        std::unique_ptr<ExprNode> body_;
    };


    class ParseError {
    public:
        ParseError(const Position& position, const std::string& message):
            position_(position), message_(message) {}

        const Position& position() const noexcept {
            return position_;
        }

        const std::string& message() const noexcept {
            return message_;
        }

        std::string what() const {
            return position_.toString() + ": " + message_;
        }

    private:
        Position position_;
        std::string message_;
    };


    // Parse a sequence of tokens and returns the root of an abstract syntax tree.
    // This function destructively modifies `it` to point the head of unparsed tokens.
    // If some error occurrs, this function throws ParseError.
    std::unique_ptr<ExprNode> parseExpr(std::vector<std::unique_ptr<Token>>::iterator& it);

    void parseAndPrint(std::vector<std::unique_ptr<Token>>::iterator& it);

}   // namespace kaleidoscope
