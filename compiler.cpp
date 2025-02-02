#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>
using namespace std;

// Token types
enum TokenType
{
    NUMBER,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LPAREN,
    RPAREN,
    END
};

// Token structure
struct Token
{
    TokenType type;
    string value;
    Token(TokenType t, string v = "") : type(t), value(v) {}
};

// Lexical Analyzer (Tokenizer)
vector<Token> tokenize(const string &input)
{
    vector<Token> tokens;
    int i = 0;
    while (i < input.size())
    {
        if (isspace(input[i]))
        {
            i++;
        }
        else if (isdigit(input[i]))
        {
            string num;
            while (i < input.size() && isdigit(input[i]))
                num += input[i++];
            tokens.push_back(Token(NUMBER, num));
        }
        else if (input[i] == '+')
            tokens.push_back(Token(PLUS, "+")), i++;
        else if (input[i] == '-')
            tokens.push_back(Token(MINUS, "-")), i++;
        else if (input[i] == '*')
            tokens.push_back(Token(MUL, "*")), i++;
        else if (input[i] == '/')
            tokens.push_back(Token(DIV, "/")), i++;
        else if (input[i] == '(')
            tokens.push_back(Token(LPAREN, "(")), i++;
        else if (input[i] == ')')
            tokens.push_back(Token(RPAREN, ")")), i++;
        else
            throw runtime_error("Invalid character in input");
    }
    tokens.push_back(Token(END)); // End of input
    return tokens;
}

// Abstract Syntax Tree (AST) Node
struct ASTNode
{
    TokenType type;
    string value;
    ASTNode *left;
    ASTNode *right;

    ASTNode(TokenType t, string v = "") : type(t), value(v), left(nullptr), right(nullptr) {}
};

// Parser
class Parser
{
    vector<Token> tokens;
    int pos;

    Token current() { return tokens[pos]; }
    void advance() { pos++; }

    ASTNode *factor()
    {
        if (current().type == NUMBER)
        {
            ASTNode *node = new ASTNode(NUMBER, current().value);
            advance();
            return node;
        }
        else if (current().type == LPAREN)
        {
            advance(); // Skip '('
            ASTNode *node = expression();
            if (current().type != RPAREN)
                throw runtime_error("Mismatched parentheses");
            advance(); // Skip ')'
            return node;
        }
        throw runtime_error("Invalid factor");
    }

    ASTNode *term()
    {
        ASTNode *node = factor();
        while (current().type == MUL || current().type == DIV)
        {
            Token op = current();
            advance();
            ASTNode *right = factor();
            ASTNode *newNode = new ASTNode(op.type, op.value);
            newNode->left = node;
            newNode->right = right;
            node = newNode;
        }
        return node;
    }

    ASTNode *expression()
    {
        ASTNode *node = term();
        while (current().type == PLUS || current().type == MINUS)
        {
            Token op = current();
            advance();
            ASTNode *right = term();
            ASTNode *newNode = new ASTNode(op.type, op.value);
            newNode->left = node;
            newNode->right = right;
            node = newNode;
        }
        return node;
    }

public:
    Parser(vector<Token> t) : tokens(t), pos(0) {}
    ASTNode *parse() { return expression(); }
};

// Evaluate the AST
int evaluate(ASTNode *node)
{
    if (!node)
        return 0;
    if (node->type == NUMBER)
        return stoi(node->value);

    int leftVal = evaluate(node->left);
    int rightVal = evaluate(node->right);

    if (node->type == PLUS)
        return leftVal + rightVal;
    if (node->type == MINUS)
        return leftVal - rightVal;
    if (node->type == MUL)
        return leftVal * rightVal;
    if (node->type == DIV)
        return leftVal / rightVal;

    throw runtime_error("Invalid node");
}

// Main Function
int main()
{
    string input;
    cout << "Enter an expression: ";
    cin >> input;

    try
    {
        vector<Token> tokens = tokenize(input);
        Parser parser(tokens);
        ASTNode *root = parser.parse();
        cout << "Result: " << evaluate(root) << endl;
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
