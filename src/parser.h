#ifndef PARSER_H
#define PARSER_H

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include <stdbool.h>

typedef struct ASTNode {
    char *type;                 // Type de nœud : "program", "draw_stmt", "condition", etc.
    Token token;                // Token associe, utile pour les feuilles
    struct ASTNode **children;  // Pointeurs vers les enfants
    int children_count;         // Nombre d'enfants
} ASTNode;

// Fonction pour les tokens "Draw"
bool is_draw_stmt(TokenType type) {
    return type == TOKEN_DRAWCREATE_CURSOR ||
           type == TOKEN_DRAWGO ||
           type == TOKEN_DRAWSETX ||
           type == TOKEN_DRAWSETY ||
           type == TOKEN_DRAWSHOW_CURSOR ||
           type == TOKEN_DRAWHIDE_CURSOR ||
           type == TOKEN_DRAWCURSOR_COLOR ||
           type == TOKEN_DRAWPEN_SIZE ||
           type == TOKEN_DRAWMOVE_FORWARD ||
           type == TOKEN_DRAWMOVE_BACKWARD ||
           type == TOKEN_DRAWPIVOT_LEFT ||
           type == TOKEN_DRAWPIVOT_RIGHT ||
           type == TOKEN_DRAWCIRCLE ||
           type == TOKEN_DRAWDOT ||
           type == TOKEN_DRAWARC ||
           type == TOKEN_DRAWUPDATE ||
           type == TOKEN_DRAWPENUP ||
           type == TOKEN_DRAWPENDOWN ||
           type == TOKEN_DRAWSHAPE ||
           type == TOKEN_DRAWCLEAR_SCREEN ||
           type == TOKEN_DRAWT_SLEEP;
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

    // Verifier si le token est un nombre ou une variable
    if (current.type == TOKEN_NBR || current.type == TOKEN_VARIABLE) {
        ASTNode *node = create_node("value", current);
        (*index)++;
        return node;
    }

    printf("Erreur: Attendu une valeur (nombre ou variable) ligne %d, colonne %d\n", current.line, current.col);
    return NULL;
}

ASTNode *parse_condition_expr(Token *tokens, int *index) {
    Token current = tokens[*index];

    // Creer un nœud pour l'expression conditionnelle
    ASTNode *node = create_node("condition_expr", current);

    // Verification du "NOT" au debut de l'expression
    if (current.type == TOKEN_NOT) {
        add_child(node, create_node("not", current));
        (*index)++;
        
        // Appeler parse_condition_expr pour analyser l'expression apres "NOT"
        ASTNode *sub_expr = parse_condition_expr(tokens, index);
        if (sub_expr == NULL) {
            printf("Erreur : expression apres 'NOT' invalide ligne %d, colonne %d\n",
                   tokens[*index].line, tokens[*index].col);
            return NULL;
        }
        add_child(node, sub_expr);
        return node; // Retourner le nœud "not" avec l'expression inversee
    }

    // Appeler parse_value pour analyser la premiere operande (si pas de "NOT")
    ASTNode *left_operand = parse_value(tokens, index);
    if (left_operand == NULL) {
        printf("Erreur : operande gauche invalide dans l'expression conditionnelle ligne %d, colonne %d\n",
               tokens[*index].line, tokens[*index].col);
        return NULL;
    }
    add_child(node, left_operand);

    // Verifier l'operateur de comparaison
    current = tokens[*index];
    if (current.type == TOKEN_EQ || current.type == TOKEN_NEQ || 
        current.type == TOKEN_SUP || current.type == TOKEN_INF || 
        current.type == TOKEN_LEQ || current.type == TOKEN_GEQ) {
        add_child(node, create_node("operator", current));
        (*index)++;

        // Appeler parse_value pour analyser la deuxieme operande
        ASTNode *right_operand = parse_value(tokens, index);
        if (right_operand == NULL) {
            printf("Erreur : operande droite invalide dans l'expression conditionnelle ligne %d, colonne %d\n",
                   tokens[*index].line, tokens[*index].col);
            return NULL;
        }
        add_child(node, right_operand);

        // Gestion des operateurs logiques (&&, ||)
        current = tokens[*index];
        while (current.type == TOKEN_AND || current.type == TOKEN_OR) {
            ASTNode *logical_op_node = create_node("logical_operator", current);
            (*index)++;

            // Appeler recursivement parse_condition_expr pour analyser la prochaine condition
            ASTNode *next_condition = parse_condition_expr(tokens, index);
            if (next_condition == NULL) {
                printf("Erreur : expression conditionnelle invalide apres operateur logique ligne %d, colonne %d\n",
                       tokens[*index].line, tokens[*index].col);
                return NULL;
            }

            add_child(logical_op_node, node); // Ajouter l'expression actuelle
            add_child(logical_op_node, next_condition); // Ajouter la nouvelle condition
            node = logical_op_node; // Mettre a jour le nœud courant

            current = tokens[*index];
        }

        return node; // Retourner l'AST complet
    } else {
        printf("Erreur : operateur de comparaison attendu ligne %d, colonne %d\n", 
               tokens[*index].line, tokens[*index].col);
        return NULL;
    }
}

ASTNode *parse_block(Token *tokens, int *index) {
    // Verifie que le bloc commence bien par une accolade ouvrante
    if (tokens[*index].type != TOKEN_LBRACE) {
        printf("Erreur syntaxique ligne %d, colonne %d : '{' attendu\n",
               tokens[*index].line, tokens[*index].col);
        return NULL;
    }

    // Cree un nœud pour le bloc
    ASTNode *block_node = create_node("block", tokens[*index]);
    (*index)++; // Avance apres l'accolade ouvrante

    // Parcourt les instructions a l'interieur du bloc
    while (tokens[*index].type != TOKEN_RBRACE) {
        if (tokens[*index].type == TOKEN_EOF) {
            printf("Erreur syntaxique : '}' manquant avant la fin du fichier\n");
            return NULL;
        }

        // Analyse une instruction
        ASTNode *stmt = parse_statement(tokens, index);
        if (stmt != NULL) {
            add_child(block_node, stmt);
        } else {
            printf("Erreur syntaxique ligne %d, colonne %d\n",
                   tokens[*index].line, tokens[*index].col);
            return NULL;
        }
    }

    // Avance apres l'accolade fermante
    (*index)++;
    return block_node;
}


ASTNode *parse_draw_stmt(Token *tokens, int *index) {
    Token current = tokens[*index];
    ASTNode *node = create_node("draw_stmt", current);

    if (is_draw_stmt(current.type)) {
        add_child(node, create_node("command", current));
        (*index)++;
        //On verifie si le token qui suit une fonction draw est une parenthese gauche
        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;
            //On verifie les parametres
            if (tokens[*index].type == TOKEN_NBR) {
                add_child(node, create_node("param", tokens[*index]));      // A revoir le nombre de parametres possibles
                (*index)++;
            }
            //on verifie la parenthsese droite
            if (tokens[*index].type == TOKEN_RPAREN) {
                (*index)++;
                //On verifie le point virgule
                if (tokens[*index].type == TOKEN_SEMICOLON) {
                    (*index)++;
                    return node;
                } else {
                    printf("Erreur : point-virgule manquant a la fin du draw_stmt ligne %d, colonne %d\n", 
                           tokens[*index].line, tokens[*index].col);
                }
            } else {
                printf("Erreur : parenthese fermante ')' manquante ligne %d, colonne %d\n", 
                       tokens[*index].line, tokens[*index].col);
            }
        } else {
            printf("Erreur : parenthese ouvrante '(' manquante apres la commande ligne %d, colonne %d\n", 
                   tokens[*index].line, tokens[*index].col);
        }
    } else {
        printf("Erreur : commande draw invalide ligne %d, colonne %d\n", 
               current.line, current.col);
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
        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;
            
            // Parsing de l'expression conditionnelle
            ASTNode *cond_expr = parse_condition_expr(tokens, index);
            if (cond_expr != NULL) {
                add_child(node, cond_expr);
                
                // Verification de la parenthese fermante
                if (tokens[*index].type == TOKEN_RPAREN) {
                    (*index)++;
                    
                    // Parsing du bloc
                    ASTNode *block = parse_block(tokens, index);
                    if (block != NULL) {
                        add_child(node, block);
                        
                        // Verification du token "else"
                        if (tokens[*index].type == TOKEN_D_ELSE) {
                            (*index)++;
                            
                            // Parsing du bloc "else"
                            ASTNode *else_block = parse_block(tokens, index);
                            if (else_block != NULL) {
                                add_child(node, else_block);
                            } else {
                                printf("Erreur : bloc 'else' invalide ligne %d, colonne %d\n",
                                       tokens[*index].line, tokens[*index].col);
                            }
                        }
                        return node;
                    } else {
                        printf("Erreur : bloc 'if' invalide ligne %d, colonne %d\n", 
                               tokens[*index].line, tokens[*index].col);
                    }
                } else {
                    printf("Erreur : parenthese fermante ')' manquante ligne %d, colonne %d\n", 
                           tokens[*index].line, tokens[*index].col);
                }
            } else {
                printf("Erreur : expression conditionnelle invalide ligne %d, colonne %d\n", 
                       tokens[*index].line, tokens[*index].col);
            }
        } else {
            printf("Erreur : parenthese ouvrante '(' manquante apres 'if' ligne %d, colonne %d\n", 
                   tokens[*index].line, tokens[*index].col);
        }
    } else {
        printf("Erreur : token 'if' attendu ligne %d, colonne %d\n", 
               current.line, current.col);
    }

    return NULL;
}

ASTNode *parse_setcolor_stmt(Token *tokens, int *index) {
    Token current = tokens[*index];

    // Verifier que le premier token est TOKEN_DRAWCURSOR_COLOR
    if (current.type != TOKEN_DRAWCURSOR_COLOR) {
        printf("Erreur: Attendu 'TOKEN_DRAWCURSOR_COLOR' ligne %d, colonne %d\n", current.line, current.col);
        return NULL;
    }

    // Creer le nœud principal pour cette instruction
    ASTNode *node = create_node("setcolor_stmt", current);
    (*index)++;

    // Verifier et ajouter le token '='
    current = tokens[*index];
    if (current.type != TOKEN_ASSIGN) {
        printf("Erreur: Attendu '=' apres 'TOKEN_DRAWCURSOR_COLOR' ligne %d, colonne %d\n", current.line, current.col);
        return NULL;
    }
    add_child(node, create_node("assign", current));
    (*index)++;

    // Verifier et ajouter la chaîne de caracteres (TOKEN_STR)
    current = tokens[*index];
    if (current.type != TOKEN_STR) {
        printf("Erreur: Attendu une chaîne de caracteres ligne %d, colonne %d\n", current.line, current.col);
        return NULL;
    }
    add_child(node, create_node("color_value", current));
    (*index)++;

    // Verifier et consommer le point-virgule
    current = tokens[*index];
    if (current.type != TOKEN_SEMICOLON) {
        printf("Erreur: Attendu ';' apres la valeur ligne %d, colonne %d\n", current.line, current.col);
        return NULL;
    }
    (*index)++;

    return node;

    // Exemple de code :  drawcursor.color = "red";
}

ASTNode *parse_move_stmt(Token *tokens, int *index) {
    Token current = tokens[*index];

    // Verifier que le premier token est TOKEN_DRAWSET_POS
    if (current.type != TOKEN_DRAWSET_POS) {
        printf("Erreur: Attendu 'TOKEN_DRAWSET_POS' ligne %d, colonne %d\n", current.line, current.col);
        return NULL;
    }

    // Creer le nœud principal pour cette instruction
    ASTNode *node = create_node("move_stmt", current);
    (*index)++;

    // Verifier et consommer le token '('
    current = tokens[*index];
    if (current.type != TOKEN_LPAREN) {
        printf("Erreur: Attendu '(' apres 'TOKEN_DRAWSET_POS' ligne %d, colonne %d\n", current.line, current.col);
        return NULL;
    }
    (*index)++;

    // Analyser la premiere valeur (x)
    ASTNode *xValue = parse_value(tokens, index);
    if (xValue == NULL) {
        printf("Erreur: Valeur invalide pour x ligne %d, colonne %d\n", tokens[*index].line, tokens[*index].col);
        return NULL;
    }
    add_child(node, xValue);

    // Verifier et consommer la virgule
    current = tokens[*index];
    if (current.type != TOKEN_COMMA) {
        printf("Erreur: Attendu ',' apres la premiere valeur ligne %d, colonne %d\n", current.line, current.col);
        return NULL;
    }
    (*index)++;

    // Analyser la deuxieme valeur (y)
    ASTNode *yValue = parse_value(tokens, index);
    if (yValue == NULL) {
        printf("Erreur: Valeur invalide pour y ligne %d, colonne %d\n", tokens[*index].line, tokens[*index].col);
        return NULL;
    }
    add_child(node, yValue);

    // Verifier et consommer le token ')'
    current = tokens[*index];
    if (current.type != TOKEN_RPAREN) {
        printf("Erreur: Attendu ')' apres la deuxieme valeur ligne %d, colonne %d\n", current.line, current.col);
        return NULL;
    }
    (*index)++;

    // Verifier et consommer le point-virgule
    current = tokens[*index];
    if (current.type != TOKEN_SEMICOLON) {
        printf("Erreur: Attendu ';' apres ')' ligne %d, colonne %d\n", current.line, current.col);
        return NULL;
    }
    (*index)++;

    return node;
}


ASTNode *parse_loop(Token *tokens, int *index) {
    Token current = tokens[*index];
    ASTNode *node = create_node("loop", current);

    if (current.type == TOKEN_D_FOR || current.type == TOKEN_D_WHILE) {
        add_child(node, create_node("loop_type", current));
        (*index)++;
        //verif parenthese ouvrante
        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;
            // on utilise parse_stmt car le token qui suit un "loop" peut etre soit une condition(while) ou un statement(for)
            ASTNode *stmt = parse_statement(tokens, index);
            if (stmt != NULL) {
                add_child(node, stmt);
                //verif parenthese fermante
                if (tokens[*index].type == TOKEN_RPAREN) {
                    (*index)++;
                    // traitement du block qui suit
                    ASTNode *block = parse_block(tokens, index);
                    if (block != NULL) {
                        add_child(node, block);
                        return node;
                    }
                }
            }
        }
    }

    printf("Erreur syntaxique dans loop ligne %d, colonne %d\n", 
           current.line, current.col);
    return NULL;
}


// Fait office de dispatcher pour les differents token
ASTNode *parse_statement(Token *tokens, int *index) {
    Token current = tokens[*index];

    if (is_draw_stmt(current.type)) {
        return parse_draw_stmt(tokens, index);
    } 
    // traitement des conditions
    else if (current.type == TOKEN_D_IF) {
        return parse_condition(tokens, index);
    } 
    // Traitement des boucles
    else if (current.type == TOKEN_D_FOR || current.type == TOKEN_D_WHILE) {
        return parse_loop(tokens, index);
    } 
    // Traitement token drawcursor
    else if (current.type == TOKEN_DRAWCURSOR_COLOR) {
        return parse_setcolor_stmt(tokens, index);
    } 
    // traitement drawset_pos
    else if (current.type == TOKEN_DRAWSET_POS) {
        return parse_move_stmt(tokens, index);
    } else {
        printf("Instruction non reconnue ligne %d, colonne %d\n", 
               current.line, current.col);
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
#endif