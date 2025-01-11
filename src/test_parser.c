#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Fonction utilitaire pour afficher un ASTNode
void print_ast(ASTNode *node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("Node Type: %s, Token: %s\n", node->type, node->token.lexeme);

    for (int i = 0; i < node->children_count; i++) {
        print_ast(node->children[i], depth + 1);
    }
}

// Test: Création de nœuds
void test_create_node() {
    Token token = {TOKEN_DRAWCURSOR_COLOR, "drawcursor.color", 1, 1};
    ASTNode *node = create_node("setcolor_stmt", token);
    if (strcmp(node->type, "setcolor_stmt") != 0) {
        printf("Test create_node failed: type mismatch\n");
    } else if (strcmp(node->token.lexeme, "drawcursor.color") != 0) {
        printf("Test create_node failed: token mismatch\n");
    } else {
        printf("Test create_node passed\n");
    }
    free(node);
}

// Test: Ajout d'enfants
void test_add_child() {
    Token parent_token = {TOKEN_DRAWCURSOR_COLOR, "drawcursor.color", 1, 1};
    Token child_token = {TOKEN_STR, "\"red\"", 1, 14};

    ASTNode *parent = create_node("setcolor_stmt", parent_token);
    ASTNode *child = create_node("color_lexeme", child_token);

    add_child(parent, child);

    if (parent->children_count != 1) {
        printf("Test add_child failed: children count mismatch\n");
    } else if (strcmp(parent->children[0]->token.lexeme, "\"red\"") != 0) {
        printf("Test add_child failed: child token mismatch\n");
    } else {
        printf("Test add_child passed\n");
    }

    free(parent->children);
    free(parent);
    free(child);
}

// Test: Parsing des instructions draw
void test_parse_draw_stmt() {
    Token tokens[] = {
        {TOKEN_DRAWGO, "drawgo", 1, 1},
        {TOKEN_LPAREN, "(", 1, 8},
        {TOKEN_NBR, "50", 1, 9},
        {TOKEN_RPAREN, ")", 1, 11},
        {TOKEN_SEMICOLON, ";", 1, 12},
        {TOKEN_EOF, "", 1, 13}
    };
    int index = 0;

    ASTNode *node = parse_draw_stmt(tokens, &index);

    if (!node || strcmp(node->type, "draw_stmt") != 0) {
        printf("Test parse_draw_stmt failed: incorrect node type\n");
    } else if (node->children_count != 2) {
        printf("Test parse_draw_stmt failed: children count mismatch\n");
    } else if (strcmp(node->children[0]->token.lexeme, "drawgo") != 0) {
        printf("Test parse_draw_stmt failed: command mismatch\n");
    } else {
        printf("Test parse_draw_stmt passed\n");
    }

    free(node->children);
    free(node);
}

// Test: Parsing des conditions
void test_parse_condition() {
    Token tokens[] = {
        {TOKEN_D_IF, "if", 1, 1},
        {TOKEN_LPAREN, "(", 1, 4},
        {TOKEN_VARIABLE, "x", 1, 5},
        {TOKEN_EQ, "==", 1, 7},
        {TOKEN_NBR, "10", 1, 10},
        {TOKEN_RPAREN, ")", 1, 12},
        {TOKEN_LBRACE, "{", 1, 14},
        {TOKEN_DRAWGO, "drawgo", 2, 1},
        {TOKEN_LPAREN, "(", 2, 8},
        {TOKEN_NBR, "50", 2, 9},
        {TOKEN_RPAREN, ")", 2, 11},
        {TOKEN_SEMICOLON, ";", 2, 12},
        {TOKEN_RBRACE, "}", 3, 1},
        {TOKEN_EOF, "", 3, 2}
    };
    int index = 0;

    ASTNode *node = parse_condition(tokens, &index);

    if (!node || strcmp(node->type, "condition") != 0) {
        printf("Test parse_condition failed: incorrect node type\n");
    } else if (node->children_count < 2) {
        printf("Test parse_condition failed: insufficient children\n");
    } else {
        printf("Test parse_condition passed\n");
    }

    free(node->children);
    free(node);
}

// Test: Gestion des erreurs
void test_error_handling() {
    Token tokens[] = {
        {TOKEN_DRAWGO, "drawgo", 1, 1},
        {TOKEN_LPAREN, "(", 1, 8},
        {TOKEN_NBR, "50", 1, 9},
        {TOKEN_RPAREN, ")", 1, 11},
        {TOKEN_EOF, "", 1, 12}
    };
    int index = 0;

    ASTNode *node = parse_draw_stmt(tokens, &index);

    if (node) { 
        printf("Test error_handling failed: error not caught\n");
    } else {
        printf("Test error_handling passed\n");
    }
}

int main() {
    test_create_node();
    test_add_child();
    test_parse_draw_stmt();
    test_parse_condition();
    test_error_handling();
    return 0;
}
