#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LEN 100

// Token types
typedef enum {
    TOKEN_INT,        // "int" keyword
    TOKEN_IF,         // "if" keyword
    TOKEN_IDENTIFIER, // Variable names
    TOKEN_NUMBER,     // Numeric literals
    TOKEN_ASSIGN,     // "="
    TOKEN_PLUS,       // "+"
    TOKEN_MINUS,      // "-"
    TOKEN_SEMICOLON,  // ";"
    TOKEN_LBRACE,     // "{"
    TOKEN_RBRACE,     // "}"
    TOKEN_LPAREN,     // "("
    TOKEN_RPAREN,     // ")"
    TOKEN_EOF,        // End of file
    TOKEN_UNKNOWN     // Unknown token
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char text[MAX_TOKEN_LEN];
} Token;

// AST Node Types
typedef enum {
    AST_VAR_DECL,
    AST_ASSIGNMENT,
    AST_BINARY_EXPR,
    AST_IF_STATEMENT,
    AST_LITERAL,
    AST_IDENTIFIER
} ASTNodeType;

// AST Node Structure
typedef struct ASTNode {
    ASTNodeType type;
    char value[MAX_TOKEN_LEN]; // For literals and identifiers
    struct ASTNode *left;      // Left child (for binary expressions)
    struct ASTNode *right;     // Right child (for binary expressions)
    struct ASTNode *body;      // Body (for if statements)
} ASTNode;

// Global current token
Token current_token;

// Function prototypes
void getNextToken(FILE *file, Token *token);
ASTNode* parseProgram(FILE *file);
ASTNode* parseVarDecl(FILE *file);
ASTNode* parseAssignment(FILE *file);
ASTNode* parseExpression(FILE *file);
ASTNode* parseIfStatement(FILE *file);
void printAST(ASTNode *node, int indent);
void error(const char *message);

// Tokenizer
void getNextToken(FILE *file, Token *token) {
    int c = fgetc(file);

    while (isspace(c)) {
        c = fgetc(file);
    }

    if (c == EOF) {
        token->type = TOKEN_EOF;
        strcpy(token->text, "EOF");
        return;
    }

    if (isalpha(c)) {
        int len = 0;
        token->text[len++] = c;
        while (isalnum(c = fgetc(file))) {
            if (len < MAX_TOKEN_LEN - 1) {
                token->text[len++] = c;
            }
        }
        ungetc(c, file);
        token->text[len] = '\0';

        if (strcmp(token->text, "int") == 0) {
            token->type = TOKEN_INT;
        } else if (strcmp(token->text, "if") == 0) {
            token->type = TOKEN_IF;
        } else {
            token->type = TOKEN_IDENTIFIER;
        }
        return;
    }

    if (isdigit(c)) {
        int len = 0;
        token->text[len++] = c;
        while (isdigit(c = fgetc(file))) {
            if (len < MAX_TOKEN_LEN - 1) {
                token->text[len++] = c;
            }
        }
        ungetc(c, file);
        token->text[len] = '\0';
        token->type = TOKEN_NUMBER;
        return;
    }

    switch (c) {
        case '=': token->type = TOKEN_ASSIGN; strcpy(token->text, "="); break;
        case '+': token->type = TOKEN_PLUS; strcpy(token->text, "+"); break;
        case '-': token->type = TOKEN_MINUS; strcpy(token->text, "-"); break;
        case '{': token->type = TOKEN_LBRACE; strcpy(token->text, "{"); break;
        case '}': token->type = TOKEN_RBRACE; strcpy(token->text, "}"); break;
        case ';': token->type = TOKEN_SEMICOLON; strcpy(token->text, ";"); break;
        case '(': token->type = TOKEN_LPAREN; strcpy(token->text, "("); break;
        case ')': token->type = TOKEN_RPAREN; strcpy(token->text, ")"); break;
        default: token->type = TOKEN_UNKNOWN; sprintf(token->text, "%c", c); break;
    }
}

// Error Handling
void error(const char *message) {
    printf("Syntax Error: %s\n", message);
    exit(1);
}

// AST Node Creation
ASTNode* createASTNode(ASTNodeType type, const char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = type;
    strcpy(node->value, value ? value : "");
    node->left = node->right = node->body = NULL;
    return node;
}

// Parsing Functions
ASTNode* parseProgram(FILE *file) {
    getNextToken(file, &current_token);
    ASTNode *root = createASTNode(AST_LITERAL, "Program");

    while (current_token.type != TOKEN_EOF) {
        if (current_token.type == TOKEN_INT) {
            ASTNode *varDecl = parseVarDecl(file);
            varDecl->left = root->body;
            root->body = varDecl;
        } else if (current_token.type == TOKEN_IDENTIFIER) {
            ASTNode *assign = parseAssignment(file);
            assign->left = root->body;
            root->body = assign;
        } else if (current_token.type == TOKEN_IF) {
            ASTNode *ifStmt = parseIfStatement(file);
            ifStmt->left = root->body;
            root->body = ifStmt;
        } else {
            error("Unexpected token");
        }
    }

    return root;
}

ASTNode* parseVarDecl(FILE *file) {
    getNextToken(file, &current_token); // Consume 'int'

    if (current_token.type != TOKEN_IDENTIFIER) {
        error("Expected identifier after 'int'");
    }
    ASTNode *varDecl = createASTNode(AST_VAR_DECL, current_token.text);
    getNextToken(file, &current_token); // Consume identifier

    if (current_token.type != TOKEN_SEMICOLON) {
        error("Expected ';' after variable declaration");
    }
    getNextToken(file, &current_token); // Consume ';'

    return varDecl;
}

ASTNode* parseAssignment(FILE *file) {
    ASTNode *assign = createASTNode(AST_ASSIGNMENT, current_token.text); // Save identifier
    getNextToken(file, &current_token); // Consume identifier

    if (current_token.type != TOKEN_ASSIGN) {
        error("Expected '=' in assignment");
    }
    getNextToken(file, &current_token); // Consume '='
    assign->right = parseExpression(file);

    if (current_token.type != TOKEN_SEMICOLON) {
        error("Expected ';' after assignment");
    }
    getNextToken(file, &current_token); // Consume ';'

    return assign;
}

ASTNode* parseExpression(FILE *file) {
    ASTNode *expr = NULL;

    if (current_token.type == TOKEN_NUMBER || current_token.type == TOKEN_IDENTIFIER) {
        expr = createASTNode(
            current_token.type == TOKEN_NUMBER ? AST_LITERAL : AST_IDENTIFIER,
            current_token.text
        );
        getNextToken(file, &current_token);
    } else {
        error("Expected literal or identifier in expression");
    }

    return expr;
}

ASTNode* parseIfStatement(FILE *file) {
    getNextToken(file, &current_token); // Consume 'if'

    if (current_token.type != TOKEN_LPAREN) {
        error("Expected '(' after 'if'");
    }
    getNextToken(file, &current_token); // Consume '('

    ASTNode *condition = parseExpression(file);

    if (current_token.type != TOKEN_RPAREN) {
        error("Expected ')' after condition");
    }
    getNextToken(file, &current_token); // Consume ')'

    if (current_token.type != TOKEN_LBRACE) {
        error("Expected '{' after if condition");
    }
    getNextToken(file, &current_token); // Consume '{'

    ASTNode *ifStmt = createASTNode(AST_IF_STATEMENT, "if");
    ifStmt->left = condition;
    ifStmt->body = parseProgram(file);

    if (current_token.type != TOKEN_RBRACE) {
        error("Expected '}' after if body");
    }
    getNextToken(file, &current_token); // Consume '}'

    return ifStmt;
}

// AST Printing
void printAST(ASTNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    printf("%s\n", node->value);
    printAST(node->left, indent + 1);
    printAST(node->right, indent + 1);
    printAST(node->body, indent + 1);
}

// Main Function
int main() {
    const char *filename = "input.txt";
    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    ASTNode *ast = parseProgram(file);
    printAST(ast, 0);

    fclose(file);
    return 0;
}
