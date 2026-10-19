#include <cctype>
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

    bool isValidHexChar(char c) {
        return isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f');
    }

public:
    Lexer(std::string code) : source(code) {}

    Token nextToken() {
        skipWhitespaces();

        if(pos >= source.length()) {
            return {TokenType::EndOfFile, ""};
        }

        char current = peek();

        if(current == ',') {
            advance();
            return {TokenType::Comma, ","};
        }

        if(current == ';') {
            while(pos < source.length() && peek() != '\n' && peek() != '\r') {
                advance();
            }
        }

        if(current == '0' && tolower(peekNext()) == 'x') {
            advance();

            std::string regAddrStr = "0";
            regAddrStr += advance();

            while(pos < source.length() && isValidHexChar(peek())) {
                regAddrStr += advance();
            }

            return {TokenType::RegAddress, regAddrStr};
        }

        if(isdigit(current)) {
            std::string numStr = "";
            int countComma = 0;

            while(pos < source.length() && (isdigit(peek()) || peek() == '.')) {
                if(peek() == '.') countComma++;
                numStr += advance();
            }

            if(countComma == 0) numStr += ".0";

            if(countComma <= 1) return {TokenType::Number, numStr};
            else return {TokenType::Unknown, numStr};
        }

        if(isalpha(current)) {
            std::string str = "";

            while(pos < source.length() && isalpha(peek())) {
                str += advance();
            }

            if(str == "assgn") return {TokenType::Assign, str};
            else if(str == "add") return {TokenType::Add, str};
            else if(str == "subtr") return {TokenType::Subtract, str};
            else if(str == "multi") return {TokenType::Multiply, str};
            else if(str == "div") return {TokenType::Divide, str};
            else if(str == "mod") return {TokenType::Modulo, str};
            else if(str == "print") return {TokenType::Print, str};
            else return {TokenType::Unknown, str};
        }

        std::string unk(1, current);
        return {TokenType::Unknown, unk};
    }
};

int main() {
    std::cout << "Hello, world\n";
}