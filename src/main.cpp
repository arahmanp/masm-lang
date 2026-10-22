#include <cctype>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

constexpr size_t MAX_REG = 1 << 16;

using Environment =  std::vector<double>;

enum class TokenType {
    Assign,
    RegAddress,
    Number,
    Add,
    Subtract,
    Multiply,
    Divide,
    Print,
    Comma,
    EndOfFile,
    Unknown,
};

enum class NodeType {
    ProgramNode,
    StatementNode,
    ExpressionNode,
    RegAssignNode,
    MathOpNode,
    PrintNode,
    NumberNode,
    RegAddrNode
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

    void skipComments() {
        if(peek() == ';') {
            while(pos < source.length() && peek() != '\n' && peek() != '\r') {
                advance();
            }
        }
        skipWhitespaces();
    }

    bool isValidHexChar(char c) {
        return isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f');
    }

public:
    Lexer(std::string code) : source(code) {}

    Token nextToken() {
        skipWhitespaces();
        skipComments();

        if(pos >= source.length()) {
            return {TokenType::EndOfFile, ""};
        }

        char current = peek();

        if(current == ',') {
            advance();
            return {TokenType::Comma, ","};
        }

        // ! 0x is still valid here
        if(current == '0' && tolower(peekNext()) == 'x') {
            advance();

            std::string regAddrStr = "0";
            regAddrStr += advance();

            while(pos < source.length() && isValidHexChar(peek())) {
                regAddrStr += advance();
            }

            return {TokenType::RegAddress, regAddrStr};
        }

        // ! 12. is still valid here
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
            else if(str == "print") return {TokenType::Print, str};
            else return {TokenType::Unknown, str};
        }

        std::string unk(1, current);
        return {TokenType::Unknown, unk};
    }
};

class ASTNode {
public:
    NodeType type;

    ASTNode(NodeType t) : type(t) {}

    virtual double eval(Environment &env) = 0;

    virtual ~ASTNode() {}
};

class StatementNode : public ASTNode {
public:
    StatementNode(NodeType t) : ASTNode(t) {}
};

class ExpressionNode : public ASTNode {
public:
    ExpressionNode(NodeType t) : ASTNode(t) {}
};

class NumberNode : public ExpressionNode {
public:
    double value;

    NumberNode(double val) : ExpressionNode(NodeType::NumberNode), value(val) {}

    double eval(Environment &env) override {
        return value;
    }
};

class RegAddrNode : public ExpressionNode {
public:
    std::string address;

    RegAddrNode(std::string addr) : ExpressionNode(NodeType::RegAddrNode), address(addr) {};

    double eval(Environment &env) override {
        size_t index = std::stoi(address, nullptr, 0);
        return index;
    }
};

class RegAssignNode : public StatementNode {
public:
    ASTNode * regAddress;
    ASTNode * value;

    RegAssignNode(ASTNode * regAddr, ASTNode * val) : StatementNode(NodeType::RegAssignNode),
        regAddress(regAddr), value(val) {}

    double eval(Environment &env) override {
        size_t index = regAddress->eval(env);
        double val = value->eval(env);

        if(index >= MAX_REG) {
            throw std::runtime_error("lu akses melebihi jumlah register anjing!\n");
        }

        env[index] = val;

        return 0;
    }

    ~RegAssignNode() override {
        delete regAddress;
        delete value;
    }
};

class MathOpNode : public StatementNode {
public:
    char op;
    ASTNode * regDestination;
    ASTNode * regOperand1;
    ASTNode * regOperand2;

    MathOpNode(char op, ASTNode * regDest, ASTNode * regOpr1, ASTNode * regOpr2) : StatementNode(NodeType::MathOpNode), 
        op(op), regDestination(regDest), regOperand1(regOpr1), regOperand2(regOpr2) {}

    double eval(Environment &env) override {
        size_t idxDestination = regDestination->eval(env);
        size_t idxOperand1 = regOperand1->eval(env);
        size_t idxOperand2 = regOperand2->eval(env);

        if(idxDestination >= MAX_REG || idxOperand1 >= MAX_REG || idxOperand2 >= MAX_REG) {
            throw std::runtime_error("lu akses melebihi jumlah register anjing!\n");
        }

        if(op == '+') env[idxDestination] = env[idxOperand1] + env[idxOperand2];
        else if(op == '-') env[idxDestination] = env[idxOperand1] - env[idxOperand2];
        else if(op == '*') env[idxDestination] = env[idxOperand1] * env[idxOperand2];
        else if(op == '/') env[idxDestination] = env[idxOperand1] / env[idxOperand2];

        return 0;
    }

    ~MathOpNode() override {
        delete regDestination;
        delete regOperand1;
        delete regOperand2;
    }
};

class PrintNode : public StatementNode {
public:
    ASTNode * regAddress;

    PrintNode(ASTNode * regAddr) : StatementNode(NodeType::PrintNode), regAddress(regAddr) {}

    double eval(Environment &env) override {
        size_t index = regAddress->eval(env);

        if(index >= MAX_REG) {
            throw std::runtime_error("lu akses melebihi jumlah register anjing!\n");
        }

        std::cout << env[index] << '\n';

        return 0;
    }

    ~PrintNode() override {
        delete regAddress;
    }
};

int main() {
    std::cout << "Hello, world\n";
}