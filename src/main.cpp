#include <cstddef>
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
    Comma,
    Comment,
    EndOfFile,
    Unknown,
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
private:
    std::string source;
    size_t pos = 0;

    char peek() const {
        if(pos >= source.length()) return '\0';
        return source[pos];
    }

    char peekNext() const {
        if(pos + 1 >= source.length()) return '\0';
        return source[pos + 1];
    }

    char advance() {
        if(pos >= source.length()) return '\0';
        return source[pos++];
    }

    void skipWhitespaces() {
        while(pos < source.length() && (peek() == ' ' || peek() == '\t' || peek() == '\n' || peek() == '\r')) {
            advance();
        }
    }

public:
    Lexer(std::string code) : source(code) {}
};

int main() {
    std::cout << "Hello, world\n";
}