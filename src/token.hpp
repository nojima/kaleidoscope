#pragma once

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "position.hpp"

namespace kaleidoscope {

    // Base class for all tokens.
    class Token {
    public:
        explicit Token(const Position& position):
            position_(position) {}

        virtual ~Token() {}

        const Position& position() const noexcept {
            return position_;
        }

        virtual std::string toString() const = 0;

    private:
        Position position_;
    };


    // End of file
    class EofToken: public Token {
    public:
        explicit EofToken(const Position& position):
            Token(position) {}

        virtual std::string toString() const;
    };


    // "def" keyword
    class DefToken: public Token {
    public:
        explicit DefToken(const Position& position):
            Token(position) {}

        virtual std::string toString() const;
    };


    // "extern" keyword
    class ExternToken: public Token {
    public:
        explicit ExternToken(const Position& position):
            Token(position) {}

        virtual std::string toString() const;
    };


    // Identifiers
    class IdentifierToken: public Token {
    public:
        IdentifierToken(const Position& position, const std::string& name):
            Token(position), name_(name) {}

        const std::string& name() const noexcept {
            return name_;
        }

        virtual std::string toString() const;

    private:
        std::string name_;
    };


    // Real numbers
    class NumberToken: public Token {
    public:
        NumberToken(const Position& position, double number):
            Token(position), number_(number) {}

        double number() const noexcept {
            return number_;
        }

        virtual std::string toString() const;

    private:
        double number_;
    };


    // Char tokens
    class CharToken: public Token {
    public:
        CharToken(const Position& position, char ch):
            Token(position), ch_(ch) {}

        char ch() const noexcept {
            return ch_;
        }

        virtual std::string toString() const;

    private:
        char ch_;
    };


    // An error throwed when tokenization fails.
    class TokenizationError {
    public:
        TokenizationError(const Position& position, const std::string& message):
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


    // Tokenize the given stream specified by [first, last) into a sequence of tokens.
    // filename is the name of the stream used for Position stored in returned tokens.
    // Throws a TokenizationError when something fails.
    std::vector<std::unique_ptr<Token>> tokenize(
            std::istreambuf_iterator<char> first,
            std::istreambuf_iterator<char> last,
            const std::string& filename);

}   // namespace kaleidoscope
