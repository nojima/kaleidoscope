#include <cctype>
#include <cstdio>
#include "token.hpp"

namespace {

    inline int readChar(
            std::istreambuf_iterator<char>* pFirst,
            std::istreambuf_iterator<char>* pLast,
            size_t* pLine,
            size_t* pColumn)
    {
        if (*pFirst == *pLast)
            return EOF;
        const int ch = **pFirst;
        if (ch == '\n') {
            *pLine += 1;
            *pColumn = 1;
        } else {
            *pColumn += 1;
        }
        ++*pFirst;
        return ch;
    }

}   // anonymous namespace

namespace kaleidoscope {

    std::vector<std::unique_ptr<Token>> tokenize(
            std::istreambuf_iterator<char> first,
            std::istreambuf_iterator<char> last,
            const std::string& filename)
    {
        if (first == last)
            throw TokenizationError(Position(filename, 1, 1), "unexpected end of file");

        size_t line = 1;
        size_t column = 1;
        int ch = *first;
        std::vector<std::unique_ptr<Token>> tokens;

        for (;;) {
            // skip spaces
            while (std::isspace(ch)) {
                ch = readChar(&first, &last, &line, &column);
            }

            if (ch == EOF) {
                break;
            } else if (std::isalpha(ch) || ch == '_') {
                std::string s;
                do {
                    s.push_back(ch);
                    ch = readChar(&first, &last, &line, &column);
                } while (std::isalpha(ch) || ch == '_');

                if (s == "def") {
                    tokens.emplace_back(new DefToken(Position(filename, line, column)));
                } else if (s == "extern") {
                    tokens.emplace_back(new ExternToken(Position(filename, line, column)));
                } else {
                    tokens.emplace_back(new IdentifierToken(Position(filename, line, column), s));
                }
            } else if (std::isdigit(ch) || ch == '.') {
                std::string s;
                do {
                    s.push_back(ch);
                    ch = readChar(&first, &last, &line, &column);
                } while (std::isdigit(ch) || ch == '.');

                double x;
                if (std::sscanf(s.c_str(), "%lf", &x) != 1)
                    throw TokenizationError(Position(filename, line, column), "invalid format of number");
                tokens.emplace_back(new NumberToken(Position(filename, line, column), x));
            } else if (ch == '#') {
                do {
                    ch = readChar(&first, &last, &line, &column);
                } while (ch != '\n' && ch != EOF);
            } else {
                tokens.emplace_back(new CharToken(Position(filename, line, column), static_cast<char>(ch)));
                ch = readChar(&first, &last, &line, &column);
            }
        }

        tokens.emplace_back(new EofToken(Position(filename, line, column)));
        return tokens;
    }

}   // namespace kaleidoscope
