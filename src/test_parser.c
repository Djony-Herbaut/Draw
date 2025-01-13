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

// Fonction pour libérer un nœud et tous ses enfants
void free_ast(ASTNode *node) {
    if (!node) {
        return; // Si le nœud est NULL, rien à libérer
    }

    // Libérer tous les enfants récursivement
    for (int i = 0; i < node->children_count; i++) {
        free_ast(node->children[i]);
    }

    // Libérer les ressources propres du nœud
    free(node->type);        // Libérer la chaîne de caractères pour le type si allouée dynamiquement
    free(node->children);    // Libérer le tableau de pointeurs vers les enfants
    free(node);              // Libérer le nœud lui-même
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

void test_parse_double_condition() {
    Token tokens[] = {
        {TOKEN_D_IF, "if", 1, 1},
        {TOKEN_LPAREN, "(", 1, 4},
        {TOKEN_DRAWV_, "x", 1, 5},
        {TOKEN_SUP, ">", 1, 7},
        {TOKEN_NBR, "10", 1, 9},
        {TOKEN_AND, "&&", 1, 12},
        {TOKEN_DRAWV_, "y", 1, 15},
        {TOKEN_INF, "<", 1, 17},
        {TOKEN_NBR, "20", 1, 19},
        {TOKEN_RPAREN, ")", 1, 21},
        {TOKEN_LBRACE, "{", 1, 23},
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
        printf("Test parse_double_condition failed: incorrect node type\n");
    } else if (node->children_count < 3) {
        printf("Test parse_double_condition failed: insufficient children\n");
    } else {
        printf("Test parse_double_condition passed\n");
    }

}

// Test: Parsing d'une boucle for
void test_parse_for_loop_success() {
    Token tokens[] = {
        {TOKEN_D_FOR, "for", 1, 1},       // "for"
        {TOKEN_LPAREN, "(", 1, 5},       // "("
        {TOKEN_VARNBR, "nbr", 1, 6},     // "varnbr" (type)
        {TOKEN_DRAWV_, "i", 1, 13},      // "i" (nom de variable) - Utiliser TOKEN_DRAWV_ ici
        {TOKEN_ASSIGN, "=", 1, 14},          // "="
        {TOKEN_NBR, "0", 1, 16},         // "0" (valeur)
        {TOKEN_SEMICOLON, ";", 1, 17},   // ";"
        {TOKEN_DRAWV_, "i", 1, 19},      // "i" (nom de variable)
        {TOKEN_SUP, "<", 1, 21},         // "<"
        {TOKEN_NBR, "10", 1, 23},        // "10"
        {TOKEN_SEMICOLON, ";", 1, 25},   // ";"
        {TOKEN_DRAWV_, "i", 1, 27},      // "i" (nom de variable)
        {TOKEN_ASSIGN, "=", 1, 28},          // "="
        {TOKEN_DRAWV_, "i", 1, 29},      // "i"
        {TOKEN_PLUS, "+", 1, 30},        // "+"
        {TOKEN_NBR, "1", 1, 31},         // "1"
        {TOKEN_RPAREN, ")", 1, 32},      // ")"
        {TOKEN_LBRACE, "{", 1, 33},      // "{"
        {TOKEN_DRAWGO, "drawgo", 2, 1},  // "drawgo"
        {TOKEN_LPAREN, "(", 2, 8},       // "("
        {TOKEN_DRAWV_, "i", 2, 9},       // "i"
        {TOKEN_RPAREN, ")", 2, 10},      // ")"
        {TOKEN_SEMICOLON, ";", 2, 11},   // ";"
        {TOKEN_RBRACE, "}", 3, 1},       // "}"
        {TOKEN_EOF, "", 3, 2}            // EOF
    };
    int index = 0;

    ASTNode *node = parse_for_loop(tokens, &index);

    if (!node) {
        printf("Test parse_for_loop failed: node is NULL\n");
        return;
    }

    if (strcmp(node->type, "for_loop") != 0) {
        printf("Test parse_for_loop failed: expected node type 'for_loop', got '%s'\n", node->type);
        free_ast(node);
        return;
    }

    if (node->children_count != 4) {
        printf("Test parse_for_loop failed: expected 4 children, got %d\n", node->children_count);
        free_ast(node);
        return;
    }

    if (strcmp(node->children[0]->type, "variable_declaration") != 0) {
        printf("Test parse_for_loop failed: expected first child 'variable_declaration', got '%s'\n", node->children[0]->type);
    } else if (strcmp(node->children[1]->type, "condition_expr") != 0) {
        printf("Test parse_for_loop failed: expected second child 'condition', got '%s'\n", node->children[1]->type);
    } else if (strcmp(node->children[2]->type, "increment") != 0) {
        printf("Test parse_for_loop failed: expected third child 'increment', got '%s'\n", node->children[2]->type);
    } else if (strcmp(node->children[3]->type, "block") != 0) {
        printf("Test parse_for_loop failed: expected fourth child 'body', got '%s'\n", node->children[3]->type);
    } else {
        printf("Test parse_for_loop passed\n");
    }

    free_ast(node);
}

void test_for_loop_failure() {
    Token tokens[] = {
        {TOKEN_D_FOR, "for", 1, 1},          // "for"
        {TOKEN_LPAREN, "(", 1, 5},          // "("
        {TOKEN_VARNBR, "nbr", 1, 6},        // Déclaration : type
        {TOKEN_DRAWV_, "i", 1, 10},         // Déclaration : nom de variable
        {TOKEN_ASSIGN, "=", 1, 12},         // "="
        {TOKEN_NBR, "0", 1, 14},            // Valeur assignée
        // Manque un point-virgule ici
        {TOKEN_DRAWV_, "i", 1, 16},         // Condition incorrecte (devrait être un opérateur comme "<")
        {TOKEN_ASSIGN, "=", 1, 18},         // Affectation incorrecte dans la condition
        {TOKEN_NBR, "10", 1, 20},           // Valeur incorrecte dans la condition
        {TOKEN_SEMICOLON, ";", 1, 22},      // ";"
        {TOKEN_DRAWV_, "i", 1, 24},         // Incrémentation : variable
        {TOKEN_ASSIGN, "=", 1, 26},         // "="
        {TOKEN_DRAWV_, "i", 1, 28},         // Valeur incrémentée incorrecte (manque opérateur "+")
        {TOKEN_RPAREN, ")", 1, 30},         // ")" manquant
        {TOKEN_LBRACE, "{", 1, 32},         // "{"
        {TOKEN_RBRACE, "}", 1, 34}          // "}"
    };

    int index = 0;
    ASTNode *node = parse_for_loop(tokens, &index);

    if (node == NULL) {
        printf("Test for_loop_fail passed: errors detected successfully.\n");
    } else {
        printf("Test for_loop_fail failed: invalid loop was parsed as valid.\n");
    }
}

void test_parse_while_loop_success() {
    Token tokens[] = {
        {TOKEN_D_WHILE, "while", 1, 1},
        {TOKEN_LPAREN, "(", 1, 7},
        {TOKEN_DRAWV_, "x", 1, 8},
        {TOKEN_SUP, ">", 1, 10},
        {TOKEN_NBR, "0", 1, 12},
        {TOKEN_RPAREN, ")", 1, 14},
        {TOKEN_LBRACE, "{", 1, 16},
        {TOKEN_DRAWGO, "drawgo", 2, 1},
        {TOKEN_LPAREN, "(", 2, 8},
        {TOKEN_NBR, "10", 2, 9},
        {TOKEN_RPAREN, ")", 2, 11},
        {TOKEN_SEMICOLON, ";", 2, 12},
        {TOKEN_RBRACE, "}", 3, 1},
        {TOKEN_EOF, "", 3, 2}
    };
    int index = 0;

    ASTNode *node = parse_while_loop(tokens, &index);

    if (!node) {
        printf("Test parse_while_loop failed: node is NULL\n");
        return;
    }

    if (strcmp(node->type, "while_loop") != 0) {
        printf("Test parse_while_loop failed: expected node type 'while_loop', got '%s'\n", node->type);
        free_ast(node);
        return;
    }

    if (node->children_count != 2) {
        printf("Test parse_while_loop failed: expected 2 children, got %d\n", node->children_count);
        free_ast(node);
        return;
    }

    if (strcmp(node->children[0]->type, "condition_expr") != 0) {
        printf("Test parse_while_loop failed: expected first child 'condition', got '%s'\n", node->children[0]->type);
    } else if (strcmp(node->children[1]->type, "block") != 0) {
        printf("Test parse_while_loop failed: expected second child 'body', got '%s'\n", node->children[1]->type);
    } else {
        printf("Test parse_while_loop passed\n");
    }

    free_ast(node);
}

void test_while_loop_failure() {
    // Cas 1 : Parenthèse ouvrante manquante
    Token tokens1[] = {
        {TOKEN_D_WHILE, "while", 1, 1},
        {TOKEN_VARNBR, "x", 1, 7},
        {TOKEN_INF, "<", 1, 9},
        {TOKEN_NBR, "10", 1, 11},
        {TOKEN_RPAREN, ")", 1, 13},
        {TOKEN_LBRACE, "{", 1, 15},
        {TOKEN_RBRACE, "}", 1, 16}
    };
    int index1 = 0;
    ASTNode *node1 = parse_while_loop(tokens1, &index1);
    if (node1 == NULL) {
        printf("Test while_loop_fail (missing '(') passed\n");
    } else {
        printf("Test while_loop_fail (missing '(') failed\n");
    }

    // Cas 2 : Condition absente
    Token tokens2[] = {
        {TOKEN_D_WHILE, "while", 1, 1},
        {TOKEN_LPAREN, "(", 1, 7},
        {TOKEN_RPAREN, ")", 1, 9},
        {TOKEN_LBRACE, "{", 1, 11},
        {TOKEN_RBRACE, "}", 1, 12}
    };
    int index2 = 0;
    ASTNode *node2 = parse_while_loop(tokens2, &index2);
    if (node2 == NULL) {
        printf("Test while_loop_fail (missing condition) passed\n");
    } else {
        printf("Test while_loop_fail (missing condition) failed\n");
    }
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

}

// Test: Parsing des conditions
void test_parse_condition() {
    Token tokens[] = {
        {TOKEN_D_IF, "if", 1, 1},
        {TOKEN_LPAREN, "(", 1, 4},
        {TOKEN_DRAWV_, "x", 1, 5},
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

/*int main() {

    test_create_node();
    test_add_child();

    test_parse_draw_stmt();

    test_parse_condition();
    test_parse_double_condition();

    test_parse_for_loop_success();
    test_for_loop_failure();

    test_parse_while_loop_success();
    test_while_loop_failure();

    test_error_handling();
        const char *tokens[] = {
        "TOKEN_DRAWSET_POS", "100", "100", "TOKEN_SEMICOLON",
        "TOKEN_DRAWCIRCLE", "100000", "TOKEN_SEMICOLON",
        "TOKEN_DRAWCURSOR_COLOR", "blue", "TOKEN_SEMICOLON",
        "TOKEN_DRAWDOT", "TOKEN_SEMICOLON",
        "TOKEN_EOF"
    };

    int token_count = sizeof(tokens) / sizeof(tokens[0]);
    const char *filename = "../output/tokens.txt";

    // Appel de la fonction pour écrire les tokens dans un fichier
    write_tokens_to_file(tokens, token_count, filename);

    return 0;
}*/
