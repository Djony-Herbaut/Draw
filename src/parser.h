#ifndef PARSER_H
#define PARSER_H

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "ide.h"
#include <stdbool.h>
#include <string.h>

typedef struct ASTNode {
    char *type;                 // Type de nœud : "program", "draw_stmt", "condition", etc.
    Token token;                // Token associe, utile pour les feuilles
    struct ASTNode **children;  // Pointeurs vers les enfants
    int children_count;         // Nombre d'enfants
} ASTNode;

// Fonction pour les tokens "Draw"
bool is_draw_stmt_param(TokenType type) {
    return type == TOKEN_DRAWGO ||
           type == TOKEN_DRAWSETX ||
           type == TOKEN_DRAWSETY ||
           type == TOKEN_DRAWCURSOR_COLOR ||
           type == TOKEN_DRAWPEN_SIZE ||
           type == TOKEN_DRAWMOVE_FORWARD ||
           type == TOKEN_DRAWMOVE_BACKWARD ||
           type == TOKEN_DRAWPIVOT_LEFT ||
           type == TOKEN_DRAWPIVOT_RIGHT ||
           type == TOKEN_DRAWCIRCLE ||
           type == TOKEN_DRAWDOT ||
           type == TOKEN_DRAWARC ||
           type == TOKEN_DRAWSHAPE ||
           type == TOKEN_DRAWT_SLEEP;
}

bool is_draw_stmt_noParam(TokenType type) {
    return type == TOKEN_DRAWCREATE_CURSOR ||
    type == TOKEN_DRAWSHOW_CURSOR ||
    type == TOKEN_DRAWHIDE_CURSOR ||
    type == TOKEN_DRAWCLEAR_SCREEN ||
    type == TOKEN_DRAWPENUP ||
    type == TOKEN_DRAWPENDOWN ||
    type == TOKEN_DRAWUPDATE ;
}

//Construction d'un noeud a partir d'un token
ASTNode *create_node(const char *type, Token token) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = strdup(type);
    node->token = token;
    node->children = NULL;
    node->children_count = 0;
    return node;
}

void add_child(ASTNode *parent, ASTNode *child) {
    //Reallouer de la memoire pour le noeud
    parent->children = (ASTNode **)realloc(parent->children, (size_t)(parent->children_count + 1) * sizeof(ASTNode *));
    //Incrementer le compteur de noeud fils
    parent->children[parent->children_count++] = child;
}

ASTNode *parse_statement(Token *tokens, int *index);

ASTNode *parse_value(Token *tokens, int *index) {
    Token current = tokens[*index];

    // Vérifier si le token est un nombre ou une variable
    if (current.type == TOKEN_NBR || current.type == TOKEN_STR || current.type == TOKEN_DRAWV_) {
        ASTNode *node = create_node("value", current);
        (*index)++;
        return node;
    }

    char error_message[256];
    snprintf(error_message, sizeof(error_message), "Erreur: Attendu une valeur (nombre ou variable) ligne %d, colonne %d", current.line, current.col);
    log_to_console(error_message);
    return NULL;
}

ASTNode *parse_condition_expr(Token *tokens, int *index) {
    Token current = tokens[*index];

    ASTNode *node = create_node("condition_expr", current);

    if (current.type == TOKEN_NOT) {
        add_child(node, create_node("not", current));
        (*index)++;
        ASTNode *sub_expr = parse_condition_expr(tokens, index);
        if (sub_expr == NULL) {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Erreur : expression après 'NOT' invalide ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
            log_to_console(error_message);
            return NULL;
        }
        add_child(node, sub_expr);
        return node;
    }

    ASTNode *left_operand = parse_value(tokens, index);
    if (left_operand == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur : opérande gauche invalide dans l'expression conditionnelle ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
        log_to_console(error_message);
        return NULL;
    }
    add_child(node, left_operand);

    current = tokens[*index];
    if (current.type == TOKEN_EQ || current.type == TOKEN_NEQ || 
        current.type == TOKEN_SUP || current.type == TOKEN_INF || 
        current.type == TOKEN_LEQ || current.type == TOKEN_GEQ) {
        add_child(node, create_node("operator", current));
        (*index)++;

        ASTNode *right_operand = parse_value(tokens, index);
        if (right_operand == NULL) {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Erreur : opérande droite invalide dans l'expression conditionnelle ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
            log_to_console(error_message);
            return NULL;
        }
        add_child(node, right_operand);

        current = tokens[*index];
        while (current.type == TOKEN_AND || current.type == TOKEN_OR) {
            ASTNode *logical_op_node = create_node("logical_operator", current);
            (*index)++;
            ASTNode *next_condition = parse_condition_expr(tokens, index);
            if (next_condition == NULL) {
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Erreur : expression conditionnelle invalide après opérateur logique ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
                log_to_console(error_message);
                return NULL;
            }

            add_child(logical_op_node, node);
            add_child(logical_op_node, next_condition);
            node = logical_op_node;

            current = tokens[*index];
        }

        return node;
    } else {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur : opérateur de comparaison attendu ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
        log_to_console(error_message);
        return NULL;
    }
}

ASTNode *parse_block(Token *tokens, int *index) {
    if (tokens[*index].type != TOKEN_LBRACE) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur syntaxique ligne %d, colonne %d : '{' attendu", tokens[*index].line, tokens[*index].col);
        log_to_console(error_message);
        return NULL;
    }

    ASTNode *block_node = create_node("block", tokens[*index]);
    (*index)++;

    while (tokens[*index].type != TOKEN_RBRACE) {
        if (tokens[*index].type == TOKEN_EOF) {
            log_to_console("Erreur syntaxique : '}' manquant avant la fin du fichier");
            return NULL;
        }

        ASTNode *stmt = parse_statement(tokens, index);
        if (stmt != NULL) {
            add_child(block_node, stmt);
        } else {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Erreur syntaxique ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
            log_to_console(error_message);
            return NULL;
        }
    }

    (*index)++;
    return block_node;
}

ASTNode *parse_draw_stmt(Token *tokens, int *index) {
    Token current = tokens[*index];
    ASTNode *node = create_node("draw_stmt", current);

    if (is_draw_stmt_param(current.type)) {
        add_child(node, create_node("command", current));
        (*index)++;
        
        
        // Vérification de la parenthèse gauche

        // Vérification de la parenthèse gauche
        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;
            
            
            // Vérification des paramètres

            // Vérification des paramètres
            if (tokens[*index].type == TOKEN_NBR || tokens[*index].type == TOKEN_DRAWV_ || tokens[*index].type == TOKEN_STR) {
                add_child(node, create_node("param", tokens[*index]));
                (*index)++;
            } else {
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Erreur : paramètre manquant ou invalide ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
                log_to_console(error_message);
                return NULL;
            }
            
            
            // Vérification de la parenthèse droite

            // Vérification de la parenthèse droite
            if (tokens[*index].type == TOKEN_RPAREN) {
                (*index)++;
                
                
                // Vérification du point-virgule

                // Vérification du point-virgule
                if (tokens[*index].type == TOKEN_SEMICOLON) {
                    (*index)++;
                    return node;
                } else {
                    char error_message[256];
                    snprintf(error_message, sizeof(error_message), "Erreur : point-virgule manquant à la fin de la commande ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
                    log_to_console(error_message);
                    return NULL;
                }
            } else {
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Erreur : parenthèse fermante ')' manquante ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
                log_to_console(error_message);
                return NULL;
            }
        } else {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Erreur : parenthèse ouvrante '(' manquante après la commande ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
            log_to_console(error_message);
            return NULL;
        }
    } else if (is_draw_stmt_noParam(current.type)) {
        add_child(node, create_node("command", current));
        (*index)++;
        
        
        // Vérification de la parenthèse gauche

        // Vérification de la parenthèse gauche
        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;
            
            
            // Vérification de la parenthèse droite

            // Vérification de la parenthèse droite
            if (tokens[*index].type == TOKEN_RPAREN) {
                (*index)++;
                
                
                // Vérification du point-virgule

                // Vérification du point-virgule
                if (tokens[*index].type == TOKEN_SEMICOLON) {
                    (*index)++;
                    return node;
                } else {
                    char error_message[256];
                    snprintf(error_message, sizeof(error_message), "Erreur : point-virgule manquant à la fin de la commande ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
                    log_to_console(error_message);
                    return NULL;
                }
            } else {
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Erreur : parenthèse fermante ')' manquante ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
                log_to_console(error_message);
                return NULL;
            }
        } else {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Erreur : parenthèse ouvrante '(' manquante après la commande ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
            log_to_console(error_message);
            return NULL;
        }
    } else {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur : commande draw invalide ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
    }

    return NULL;
}

ASTNode *parse_condition(Token *tokens, int *index) {
    Token current = tokens[*index];
    ASTNode *node = create_node("condition", current);

    if (current.type == TOKEN_D_IF) {
        add_child(node, create_node("if", current));
        (*index)++;
        
        
        // Verification de la parenthese ouvrante

        // Verification de la parenthese ouvrante
        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;
            
            
            // Parsing de l'expression conditionnelle

            // Parsing de l'expression conditionnelle
            ASTNode *cond_expr = parse_condition_expr(tokens, index);
            if (cond_expr != NULL) {
                add_child(node, cond_expr);
                
                
                // Verification de la parenthese fermante

                // Verification de la parenthese fermante
                if (tokens[*index].type == TOKEN_RPAREN) {
                    (*index)++;
                    
                    
                    // Parsing du bloc

                    // Parsing du bloc
                    ASTNode *block = parse_block(tokens, index);
                    if (block != NULL) {
                        add_child(node, block);
                        
                        
                        // Verification du token "else"

                        // Verification du token "else"
                        if (tokens[*index].type == TOKEN_D_ELSE) {
                            (*index)++;
                            
                            
                            // Parsing du bloc "else"

                            // Parsing du bloc "else"
                            ASTNode *else_block = parse_block(tokens, index);
                            if (else_block != NULL) {
                                add_child(node, else_block);
                            } else {
                                char error_message[256];
                                snprintf(error_message, sizeof(error_message), "Erreur : bloc 'else' invalide ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
                                log_to_console(error_message);
                            }
                        }
                        return node;
                    } else {
                        char error_message[256];
                        snprintf(error_message, sizeof(error_message), "Erreur : bloc 'if' invalide ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
                        log_to_console(error_message);
                    }
                } else {
                    char error_message[256];
                    snprintf(error_message, sizeof(error_message), "Erreur : parenthese fermante ')' manquante ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
                    log_to_console(error_message);
                }
            } else {
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Erreur : expression conditionnelle invalide ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
                log_to_console(error_message);
            }
        } else {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Erreur : parenthese ouvrante '(' manquante apres 'if' ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
            log_to_console(error_message);
        }
    } else {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur : token 'if' attendu ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
    }

    return NULL;
}

ASTNode *parse_setcolor_stmt(Token *tokens, int *index) {
    Token current = tokens[*index];

    if (current.type != TOKEN_DRAWCURSOR_COLOR) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Attendu 'TOKEN_DRAWCURSOR_COLOR' ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
        return NULL;
    }

    ASTNode *node = create_node("setcolor_stmt", current);
    (*index)++;

    current = tokens[*index];
    if (current.type != TOKEN_ASSIGN) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Attendu '=' apres 'TOKEN_DRAWCURSOR_COLOR' ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
        return NULL;
    }
    add_child(node, create_node("assign", current));
    (*index)++;

    current = tokens[*index];
    if (current.type != TOKEN_STR) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Attendu une chaîne de caracteres ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
        return NULL;
    }
    add_child(node, create_node("color_value", current));
    (*index)++;

    current = tokens[*index];
    if (current.type != TOKEN_SEMICOLON) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Attendu ';' apres la valeur ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
        return NULL;
    }
    (*index)++;

    return node;
}

ASTNode *parse_move_stmt(Token *tokens, int *index) {
    Token current = tokens[*index];

    if (current.type != TOKEN_DRAWSET_POS) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Attendu 'TOKEN_DRAWSET_POS' ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
        return NULL;
    }

    ASTNode *node = create_node("move_stmt", current);
    (*index)++;

    current = tokens[*index];
    if (current.type != TOKEN_LPAREN) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Attendu '(' apres 'TOKEN_DRAWSET_POS' ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
        return NULL;
    }
    (*index)++;

    ASTNode *xValue = parse_value(tokens, index);
    if (xValue == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Valeur invalide pour x ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
        log_to_console(error_message);
        return NULL;
    }
    add_child(node, xValue);

    current = tokens[*index];
    if (current.type != TOKEN_COMMA) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Attendu ',' apres la premiere valeur ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
        return NULL;
    }
    (*index)++;

    ASTNode *yValue = parse_value(tokens, index);
    if (yValue == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Valeur invalide pour y ligne %d, colonne %d", tokens[*index].line, tokens[*index].col);
        log_to_console(error_message);
        return NULL;
    }
    add_child(node, yValue);

    current = tokens[*index];
    if (current.type != TOKEN_RPAREN) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Attendu ')' apres la deuxieme valeur ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
        return NULL;
    }
    (*index)++;

    current = tokens[*index];
    if (current.type != TOKEN_SEMICOLON) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur: Attendu ';' apres ')' ligne %d, colonne %d", current.line, current.col);
        log_to_console(error_message);
        return NULL;
    }
    (*index)++;

    return node;
}

ASTNode *parse_variable_declaration(Token *tokens, int *index) {
    Token current = tokens[*index];
    ASTNode *node = create_node("variable_declaration", current);

    if (current.type == TOKEN_VARNBR || current.type == TOKEN_VARSTR) {
        add_child(node, create_node("type", current));
        (*index)++;

        if (tokens[*index].type == TOKEN_DRAWV_) {
            Token var_name = tokens[*index];
            add_child(node, create_node("variable_name", var_name));
            (*index)++;

            if (tokens[*index].type == TOKEN_ASSIGN) {
                (*index)++;

                Token value_token = tokens[*index];
                if ((current.type == TOKEN_VARNBR && value_token.type == TOKEN_NBR) ||
                    (current.type == TOKEN_VARSTR && value_token.type == TOKEN_STR)) {
                    add_child(node, create_node("initial_value", value_token));
                    (*index)++;
                } else {
                    char error_message[256];
                    snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans déclaration de variable: valeur initiale incorrecte ligne %d, colonne %d", 
                             value_token.line, value_token.col);
                    log_to_console(error_message);
                    free(node);
                    return NULL;
                }

                if (tokens[*index].type == TOKEN_SEMICOLON) {
                    (*index)++;
                } else {
                    char error_message[256];
                    snprintf(error_message, sizeof(error_message), "Erreur syntaxique: ';' attendu ligne %d, colonne %d", 
                             tokens[*index].line, tokens[*index].col);
                    log_to_console(error_message);
                    free(node);
                    return NULL;
                }
            } else {
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans déclaration de variable: '=' attendu ligne %d, colonne %d", 
                         tokens[*index].line, tokens[*index].col);
                log_to_console(error_message);
                free(node);
                return NULL;
            }
        } else {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans déclaration de variable: nom de variable attendu ligne %d, colonne %d", 
                     tokens[*index].line, tokens[*index].col);
            log_to_console(error_message);
            free(node);
            return NULL;
        }
    } else {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans déclaration de variable: type attendu (varnbr ou varstr) ligne %d, colonne %d", 
                 current.line, current.col);
        log_to_console(error_message);
        free(node);
        return NULL;
    }

    return node;
}

ASTNode *parse_increment(Token *tokens, int *index) {
    Token current = tokens[*index];

    if (tokens[*index].type == TOKEN_DRAWV_) {
        Token var_token = tokens[*index];
        (*index)++;

        if (tokens[*index].type == TOKEN_ASSIGN) {
            (*index)++;

            if (tokens[*index].type == TOKEN_DRAWV_) {
                Token var_assignment_token = tokens[*index];
                (*index)++;

                if (tokens[*index].type == TOKEN_PLUS) {
                    (*index)++;

                    if (tokens[*index].type == TOKEN_NBR) {
                        Token number_token = tokens[*index];
                        (*index)++;

                        ASTNode *node = create_node("increment", current);

                        ASTNode *var_node = create_node("variable", var_token);
                        ASTNode *var_assignment_node = create_node("variable", var_assignment_token);
                        ASTNode *plus_node = create_node("operator", tokens[*index - 1]);
                        ASTNode *number_node = create_node("number", number_token);

                        add_child(node, var_node);
                        add_child(node, var_assignment_node);
                        add_child(node, plus_node);
                        add_child(node, number_node);

                        return node;
                    } else {
                        char error_message[256];
                        snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans l'incrémentation : un nombre est attendu après '+' ligne %d, colonne %d", 
                                 tokens[*index].line, tokens[*index].col);
                        log_to_console(error_message);
                        return NULL;
                    }
                } else {
                    char error_message[256];
                    snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans l'incrémentation : un signe '+' est attendu après '=' ligne %d, colonne %d", 
                             tokens[*index].line, tokens[*index].col);
                    log_to_console(error_message);
                    return NULL;
                }
            } else {
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans l'incrémentation : variable attendue après '=' ligne %d, colonne %d", 
                         tokens[*index].line, tokens[*index].col);
                log_to_console(error_message);
                return NULL;
            }
        } else {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans l'incrémentation : signe '=' attendu après la variable ligne %d, colonne %d", 
                     tokens[*index].line, tokens[*index].col);
            log_to_console(error_message);
            return NULL;
        }
    } else {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans l'incrémentation : une variable est attendue au début de l'incrément ligne %d, colonne %d", 
                 current.line, current.col);
        log_to_console(error_message);
    }

    return NULL;
}

ASTNode *parse_for_loop(Token *tokens, int *index) {
    Token current = tokens[*index];
    ASTNode *for_node = create_node("for_loop", current);

    if (current.type == TOKEN_D_FOR) {
        (*index)++;

        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;

            ASTNode *var_decl = parse_variable_declaration(tokens, index);
            if (var_decl != NULL) {
                add_child(for_node, var_decl);

                ASTNode *condition = parse_condition_expr(tokens, index);
                if (condition != NULL) {
                    add_child(for_node, condition);
                    if (tokens[*index].type == TOKEN_SEMICOLON) {
                        (*index)++;
                    } else {
                        char error_message[256];
                        snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: ';' manquant après la déclaration de variable", 
                                 current.line, current.col);
                        log_to_console(error_message);
                        return NULL;
                    }

                    ASTNode *increment = parse_increment(tokens, index);
                    if (increment != NULL) {
                        add_child(for_node, increment);

                        if (tokens[*index].type == TOKEN_RPAREN) {
                            (*index)++;

                            ASTNode *block = parse_block(tokens, index);
                            if (block != NULL) {
                                add_child(for_node, block);

                                if (for_node->children_count != 4) {
                                    char error_message[256];
                                    snprintf(error_message, sizeof(error_message), "Test parse_for_loop failed: expected 4 children, got %d", for_node->children_count);
                                    log_to_console(error_message);
                                    return NULL;
                                }
                                return for_node;
                            } else {
                                char error_message[256];
                                snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: Bloc manquant", 
                                         current.line, current.col);
                                log_to_console(error_message);
                            }
                        } else {
                            char error_message[256];
                            snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: Parenthèse fermante manquante", 
                                     current.line, current.col);
                            log_to_console(error_message);
                        }
                    } else {
                        char error_message[256];
                        snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: Incrémentation incorrecte", 
                                 current.line, current.col);
                        log_to_console(error_message);
                    }
                } else {
                    char error_message[256];
                    snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: Condition incorrecte", 
                             current.line, current.col);
                    log_to_console(error_message);
                }
            } else {
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: Déclaration de variable incorrecte", 
                         current.line, current.col);
                log_to_console(error_message);
            }
        } else {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: Parenthèse ouvrante manquante", 
                     current.line, current.col);
            log_to_console(error_message);
        }
    }
    char error_message[256];
    snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d", 
             current.line, current.col);
    log_to_console(error_message);
    return NULL;
}

ASTNode *parse_while_loop(Token *tokens, int *index) {
    Token current = tokens[*index];
    ASTNode *while_node = create_node("while_loop", current);

    if (current.type == TOKEN_D_WHILE) {
        (*index)++;

        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;

            ASTNode *condition = parse_condition_expr(tokens, index);
            if (condition != NULL) {
                add_child(while_node, condition);

                if (tokens[*index].type == TOKEN_RPAREN) {
                    (*index)++;

                    ASTNode *block = parse_block(tokens, index);
                    if (block != NULL) {
                        add_child(while_node, block);

                        if (while_node->children_count != 2) {
                            char error_message[256];
                            snprintf(error_message, sizeof(error_message), "Test parse_while_loop failed: expected 2 children, got %d", while_node->children_count);
                            log_to_console(error_message);
                            return NULL;
                        }
                        return while_node;
                    } else {
                        char error_message[256];
                        snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: Bloc manquant", 
                                 current.line, current.col);
                        log_to_console(error_message);
                    }
                } else {
                    char error_message[256];
                    snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: Parenthèse fermante manquante", 
                             current.line, current.col);
                    log_to_console(error_message);
                }
            } else {
                char error_message[256];
                snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: Condition incorrecte", 
                         current.line, current.col);
                log_to_console(error_message);
            }
        } else {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d: Parenthèse ouvrante manquante", 
                     current.line, current.col);
            log_to_console(error_message);
        }
    }
    char error_message[256];
    snprintf(error_message, sizeof(error_message), "Erreur syntaxique dans loop ligne %d, colonne %d", 
             current.line, current.col);
    log_to_console(error_message);
    return NULL;
}

// Fait office de dispatcher pour les differents token
ASTNode *parse_statement(Token *tokens, int *index) {
    Token current = tokens[*index];

    if (is_draw_stmt_param(current.type) || is_draw_stmt_noParam(current.type)) {
        return parse_draw_stmt(tokens, index);
    }
    // traitement des conditions
    else if (current.type == TOKEN_D_IF) {
        return parse_condition(tokens, index);
    } 
    // Traitement des boucles
    else if (current.type == TOKEN_D_FOR ) {
        return parse_for_loop(tokens, index);
    } 
    else if (current.type == TOKEN_D_WHILE){
        return parse_while_loop(tokens,index);
    }
    // Traitement token drawcursor
    else if (current.type == TOKEN_DRAWCURSOR_COLOR) {
        return parse_setcolor_stmt(tokens, index);
    } 
    // traitement drawset_pos
    else if (current.type == TOKEN_DRAWSET_POS) {
        return parse_move_stmt(tokens, index);
    } else {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Instruction non reconnue ligne %d, colonne %d", 
                 current.line, current.col);
        log_to_console(error_message);
        return NULL;
    }
}

ASTNode *parse_program(Token *tokens, int *index) {

    //Creation du premier noeud "program"
    ASTNode *program_node = create_node("program", tokens[*index]);

    // Parcours des instructions jusqu'a EOF
    while (tokens[*index].type != TOKEN_EOF) {
        // Appel du parseur d'instructions
        ASTNode *stmt = parse_statement(tokens, index);
        if (stmt != NULL) {
            add_child(program_node, stmt);
        } else {
            printf("Erreur syntaxique ligne %d, colonne %d\n", 
                   tokens[*index].line, tokens[*index].col);
            return NULL;
        }
    }

    return program_node;
}

void write_tokens_to_file(const Token tokens[], int token_count, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    for (int i = 0; i < token_count; i++) {
        // Ajouter un saut de ligne après chaque TOKEN_SEMICOLON
        if (tokens[i].type == TOKEN_SEMICOLON) {
            fprintf(file, "\n");
        }
        // Ne pas écrire le token COMMA
        if (tokens[i].type == TOKEN_COMMA || tokens[i].type == TOKEN_LPAREN || tokens[i].type == TOKEN_RPAREN || 
            tokens[i].type == TOKEN_SEMICOLON || tokens[i].type == TOKEN_EOF || tokens[i].type == TOKEN_LBRACKET || 
            tokens[i].type == TOKEN_RBRACKET || tokens[i].type == TOKEN_LBRACE || tokens[i].type == TOKEN_RBRACE) {
            continue; // Passer au token suivant
        }

        if (tokens[i].type == TOKEN_NBR) {
            // Si le token est un nombre, écrit uniquement son lexeme
            fprintf(file, "%s ", tokens[i].lexeme);
        } else {
            // Sinon, écrit le nom lisible du type du token
            const char *token_name = token_type_to_string(tokens[i].type);
            fprintf(file, "%s ", token_name);
        }
    }

    fclose(file);
    printf("Tokens écrits dans le fichier %s avec succès.\n", filename);
}

#endif