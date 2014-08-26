#include <iostream>
#include <iterator>
#include <vector>
#include "token.hpp"
#include "ast.hpp"

using namespace kaleidoscope;

int main()
{
    std::istreambuf_iterator<char> first(std::cin);
    std::istreambuf_iterator<char> last;

    std::string filename = "stdin";
    auto tokens = tokenize(first, last, filename);
    auto it = std::begin(tokens);

    parseAndPrint(it);
    return 0;
}
