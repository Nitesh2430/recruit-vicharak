#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define Token Types
typedef enum {
    TOKEN_INT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_SEMICOLON,
    TOKEN_IF,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

// Token Structure
typedef struct {
    TokenType type;
    char text[100];
} Token;

// Define AST Node Types
typedef enum {
    AST_VAR_DECL,
    AST_ASSIGN,
    AST_BINARY_OP,
    AST_IF,
    AST_LITERAL,
    AST_IDENTIFIER
} ASTNodeType;

// AST Node Structure
typedef struct ASTNode {
    ASTNodeType type;
    char value[100];
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *condition;
    struct ASTNode *body;
} ASTNode;

// Global Variables
Token current_token;
FILE *input_file;

// Function to Create AST Nodes
ASTNode *createASTNode(ASTNodeType type, const char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = type;
    if (value) strcpy(node->value, value);
    node->left = node->right = node->condition = node->body = NULL;
    return node;
}

// Lexer Function to Tokenize Input
void getNextToken(FILE *file, Token *token) {
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (isspace(c)) continue;

        if (isalpha(c)) {  // Keywords or Identifiers
            int len = 0;
            token->text[len++] = c;
            while (isalnum(c = fgetc(file))) token->text[len++] = c;
            ungetc(c, file);
            token->text[len] = '\0';

            if (strcmp(token->text, "int") == 0) token->type = TOKEN_INT;
            else if (strcmp(token->text, "if") == 0) token->type = TOKEN_IF;
            else token->type = TOKEN_IDENTIFIER;
            return;
        }

        if (isdigit(c)) {  // Numbers
            int len = 0;
            token->text[len++] = c;
            while (isdigit(c = fgetc(file))) token->text[len++] = c;
            ungetc(c, file);
            token->text[len] = '\0';
            token->type = TOKEN_NUMBER;
            return;
        }

        // Single-character tokens
        switch (c) {
            case '=': token->type = TOKEN_ASSIGN; strcpy(token->text, "="); return;
            case '+': token->type = TOKEN_PLUS; strcpy(token->text, "+"); return;
            case '-': token->type = TOKEN_MINUS; strcpy(token->text, "-"); return;
            case '{': token->type = TOKEN_LBRACE; strcpy(token->text, "{"); return;
            case '}': token->type = TOKEN_RBRACE; strcpy(token->text, "}"); return;
            case '(': token->type = TOKEN_LPAREN; strcpy(token->text, "("); return;
            case ')': token->type = TOKEN_RPAREN; strcpy(token->text, ")"); return;
            case ';': token->type = TOKEN_SEMICOLON; strcpy(token->text, ";"); return;
            default: token->type = TOKEN_UNKNOWN; token->text[0] = c; token->text[1] = '\0'; return;
        }
    }
    token->type = TOKEN_EOF;
}

// Parser Functions
ASTNode *parseExpression();
ASTNode *parseVarDecl();
ASTNode *parseIf();
ASTNode *parseAssignment();

ASTNode *parseStatement() {
    if (current_token.type == TOKEN_INT) return parseVarDecl();
    else if (current_token.type == TOKEN_IF) return parseIf();
    else if (current_token.type == TOKEN_IDENTIFIER) return parseAssignment();
    else {
        printf("Syntax error: unexpected token '%s'\n", current_token.text);
        exit(1);
    }
}

ASTNode *parseVarDecl() {
    getNextToken(input_file, &current_token); // Consume 'int'
    if (current_token.type != TOKEN_IDENTIFIER) {
        printf("Syntax error: expected identifier\n");
        exit(1);
    }

    ASTNode *node = createASTNode(AST_VAR_DECL, current_token.text);
    getNextToken(input_file, &current_token); // Consume identifier

    if (current_token.type != TOKEN_SEMICOLON) {
        printf("Syntax error: expected ';'\n");
        exit(1);
    }
    getNextToken(input_file, &current_token); // Consume ';'

    return node;
}

ASTNode *parseAssignment() {
    ASTNode *node = createASTNode(AST_ASSIGN, current_token.text);
    getNextToken(input_file, &current_token); // Consume identifier

    if (current_token.type != TOKEN_ASSIGN) {
        printf("Syntax error: expected '='\n");
        exit(1);
    }
    getNextToken(input_file, &current_token); // Consume '='

    node->left = parseExpression();
    if (current_token.type != TOKEN_SEMICOLON) {
        printf("Syntax error: expected ';'\n");
        exit(1);
    }
    getNextToken(input_file, &current_token); // Consume ';'
    return node;
}

ASTNode *parseExpression() {
    ASTNode *node = createASTNode(AST_LITERAL, current_token.text);
    getNextToken(input_file, &current_token); // Consume literal or identifier

    if (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS) {
        ASTNode *opNode = createASTNode(AST_BINARY_OP, current_token.text);
        getNextToken(input_file, &current_token); // Consume operator
        opNode->left = node;
        opNode->right = parseExpression();
        return opNode;
    }

    return node;
}

ASTNode *parseIf() {
    getNextToken(input_file, &current_token); // Consume 'if'
    if (current_token.type != TOKEN_LPAREN) {
        printf("Syntax error: expected '('\n");
        exit(1);
    }
    getNextToken(input_file, &current_token); // Consume '('

    ASTNode *condition = parseExpression();
    if (current_token.type != TOKEN_RPAREN) {
        printf("Syntax error: expected ')'\n");
        exit(1);
    }
    getNextToken(input_file, &current_token); // Consume ')'

    if (current_token.type != TOKEN_LBRACE) {
        printf("Syntax error: expected '{'\n");
        exit(1);
    }
    getNextToken(input_file, &current_token); // Consume '{'

    ASTNode *body = parseStatement();
    if (current_token.type != TOKEN_RBRACE) {
        printf("Syntax error: expected '}'\n");
        exit(1);
    }
    getNextToken(input_file, &current_token); // Consume '}'

    ASTNode *node = createASTNode(AST_IF, NULL);
    node->condition = condition;
    node->body = body;
    return node;
}

// Code Generator
void generateCode(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_VAR_DECL:
            printf("DECLARE %s\n", node->value);
            break;
        case AST_ASSIGN:
            generateCode(node->left);
            printf("STORE %s\n", node->value);
            break;
        case AST_BINARY_OP:
            generateCode(node->left);
            generateCode(node->right);
            if (strcmp(node->value, "+") == 0) printf("ADD\n");
            else if (strcmp(node->value, "-") == 0) printf("SUB\n");
            break;
        case AST_LITERAL:
            printf("LOAD %s\n", node->value);
            break;
        case AST_IF:
            generateCode(node->condition);
            printf("JUMP_IF_ZERO else_label\n");
            generateCode(node->body);
            printf("else_label:\n");
            break;
        default:
            printf("Unknown AST node type\n");
    }
}

// Main Function
int main() {
    input_file = fopen("input.txt", "r");
    if (!input_file) {
        perror("Failed to open input file");
        return 1;
    }

    getNextToken(input_file, &current_token);

    ASTNode *program = parseStatement();
    generateCode(program);

    fclose(input_file);
    return 0;
}
