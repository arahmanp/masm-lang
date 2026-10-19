#include <iostream>
#include <string>

enum class TokenType {
    Assign,
    RegAddress,
    Number,
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Print,
    Comment,
};

struct Token {
    TokenType type;
    std::string value;
};

int main() {
    std::cout << "Hello, world\n";
}