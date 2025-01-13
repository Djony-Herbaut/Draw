#ifndef PARSER_H
#define PARSER_H

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "token.h"
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

    // Verifier si le token est un nombre ou une variable
    if (current.type == TOKEN_NBR || current.type == TOKEN_STR || current.type == TOKEN_DRAWV_) {
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

    if (is_draw_stmt_param(current.type)) {  // Cas où la commande nécessite un paramètre
        add_child(node, create_node("command", current));
        (*index)++;
        
        // Vérification de la parenthèse gauche
        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;
            
            // Vérification des paramètres
            if (tokens[*index].type == TOKEN_NBR || tokens[*index].type == TOKEN_DRAWV_ || tokens[*index].type == TOKEN_STR) {
                add_child(node, create_node("param", tokens[*index]));
                (*index)++;
            } else {
                printf("Erreur : paramètre manquant ou invalide ligne %d, colonne %d\n",
                       tokens[*index].line, tokens[*index].col);
                return NULL;
            }
            
            // Vérification de la parenthèse droite
            if (tokens[*index].type == TOKEN_RPAREN) {
                (*index)++;
                
                // Vérification du point-virgule
                if (tokens[*index].type == TOKEN_SEMICOLON) {
                    (*index)++;
                    return node;
                } else {
                    printf("Erreur : point-virgule manquant à la fin de la commande ligne %d, colonne %d\n",
                           tokens[*index].line, tokens[*index].col);
                    return NULL;
                }
            } else {
                printf("Erreur : parenthèse fermante ')' manquante ligne %d, colonne %d\n",
                       tokens[*index].line, tokens[*index].col);
                return NULL;
            }
        } else {
            printf("Erreur : parenthèse ouvrante '(' manquante après la commande ligne %d, colonne %d\n",
                   tokens[*index].line, tokens[*index].col);
            return NULL;
        }
    } else if (is_draw_stmt_noParam(current.type)) {  // Cas où la commande ne nécessite pas de paramètre
        add_child(node, create_node("command", current));
        (*index)++;
        
        // Vérification de la parenthèse gauche
        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;
            
            // Vérification de la parenthèse droite
            if (tokens[*index].type == TOKEN_RPAREN) {
                (*index)++;
                
                // Vérification du point-virgule
                if (tokens[*index].type == TOKEN_SEMICOLON) {
                    (*index)++;
                    return node;
                } else {
                    printf("Erreur : point-virgule manquant à la fin de la commande ligne %d, colonne %d\n",
                           tokens[*index].line, tokens[*index].col);
                    return NULL;
                }
            } else {
                printf("Erreur : parenthèse fermante ')' manquante ligne %d, colonne %d\n",
                       tokens[*index].line, tokens[*index].col);
                return NULL;
            }
        } else {
            printf("Erreur : parenthèse ouvrante '(' manquante après la commande ligne %d, colonne %d\n",
                   tokens[*index].line, tokens[*index].col);
            return NULL;
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

ASTNode *parse_variable_declaration(Token *tokens, int *index) {
    Token current = tokens[*index];
    ASTNode *node = create_node("variable_declaration", current);

    // Vérifier que la déclaration commence avec un type valide (TOKEN_VARNBR ou TOKEN_VARSTR)
    if (current.type == TOKEN_VARNBR || current.type == TOKEN_VARSTR) {
        add_child(node, create_node("type", current)); // Ajouter le type comme enfant
        (*index)++; // Consommer le type

        // Vérifier le nom de la variable
        if (tokens[*index].type == TOKEN_DRAWV_) {  // Utiliser TOKEN_DRAWV_ pour le nom de la variable
            Token var_name = tokens[*index];
            add_child(node, create_node("variable_name", var_name)); // Ajouter le nom de la variable comme enfant
            (*index)++; // Consommer le nom de la variable

            // Vérifier l'assignation avec '='
            if (tokens[*index].type == TOKEN_ASSIGN) {
                (*index)++; // Consommer '='

                // Vérifier la valeur assignée
                Token value_token = tokens[*index];
                if ((current.type == TOKEN_VARNBR && value_token.type == TOKEN_NBR) ||
                    (current.type == TOKEN_VARSTR && value_token.type == TOKEN_STR)) {
                    add_child(node, create_node("initial_value", value_token)); // Ajouter la valeur initiale comme enfant
                    (*index)++; // Consommer la valeur assignée
                } else {
                    printf("Erreur syntaxique dans déclaration de variable: valeur initiale incorrecte ligne %d, colonne %d\n", 
                           value_token.line, value_token.col);
                    free(node);
                    return NULL;
                }

                // Vérifier la présence du point-virgule
                if (tokens[*index].type == TOKEN_SEMICOLON) {
                    (*index)++; // Consommer le point-virgule
                } else {
                    printf("Erreur syntaxique: ';' attendu ligne %d, colonne %d\n", 
                           tokens[*index].line, tokens[*index].col);
                    free(node);
                    return NULL;
                }
            } else {
                printf("Erreur syntaxique dans déclaration de variable: '=' attendu ligne %d, colonne %d\n", 
                       tokens[*index].line, tokens[*index].col);
                free(node);
                return NULL;
            }
        } else {
            printf("Erreur syntaxique dans déclaration de variable: nom de variable attendu ligne %d, colonne %d\n", 
                   tokens[*index].line, tokens[*index].col);
            free(node);
            return NULL;
        }
    } else {
        printf("Erreur syntaxique dans déclaration de variable: type attendu (varnbr ou varstr) ligne %d, colonne %d\n", 
               current.line, current.col);
        free(node);
        return NULL;
    }

    return node;
}

ASTNode *parse_increment(Token *tokens, int *index) {
    Token current = tokens[*index];

    // Vérifier si l'expression commence par une variable (par exemple "i")
    if (tokens[*index].type == TOKEN_DRAWV_) {
        Token var_token = tokens[*index];
        (*index)++;

        // Vérifier le signe d'assignation "="
        if (tokens[*index].type == TOKEN_ASSIGN) {
            (*index)++;  // Consommer "="

            // Vérifier la réassignation "i = i"
            if (tokens[*index].type == TOKEN_DRAWV_) {
                Token var_assignment_token = tokens[*index];
                (*index)++;

                // Vérifier si l'opérateur "+" est présent
                if (tokens[*index].type == TOKEN_PLUS) {
                    (*index)++;  // Consommer "+"

                    // Vérifier que le deuxième terme est un nombre (par exemple "i = i + 1")
                    if (tokens[*index].type == TOKEN_NBR) {
                        Token number_token = tokens[*index];
                        (*index)++;

                        // Créer un nœud pour l'incrémentation
                        ASTNode *node = create_node("increment", current);

                        // Créer des nœuds pour chaque partie de l'expression
                        ASTNode *var_node = create_node("variable", var_token);
                        ASTNode *var_assignment_node = create_node("variable", var_assignment_token);
                        ASTNode *plus_node = create_node("operator", tokens[*index - 1]);  // "+"
                        ASTNode *number_node = create_node("number", number_token);

                        // Ajouter les enfants au nœud d'incrément
                        add_child(node, var_node);
                        add_child(node, var_assignment_node);
                        add_child(node, plus_node);
                        add_child(node, number_node);

                        return node;  // Retourner l'AST pour l'incrément
                    } else {
                        printf("Erreur syntaxique dans l'incrémentation : un nombre est attendu après '+' ligne %d, colonne %d\n",
                               tokens[*index].line, tokens[*index].col);
                        return NULL;
                    }
                } else {
                    printf("Erreur syntaxique dans l'incrémentation : un signe '+' est attendu après '=' ligne %d, colonne %d\n",
                           tokens[*index].line, tokens[*index].col);
                    return NULL;
                }
            } else {
                printf("Erreur syntaxique dans l'incrémentation : variable attendue après '=' ligne %d, colonne %d\n",
                       tokens[*index].line, tokens[*index].col);
                return NULL;
            }
        } else {
            printf("Erreur syntaxique dans l'incrémentation : signe '=' attendu après la variable ligne %d, colonne %d\n",
                   tokens[*index].line, tokens[*index].col);
            return NULL;
        }
    } else {
        printf("Erreur syntaxique dans l'incrémentation : une variable est attendue au début de l'incrément ligne %d, colonne %d\n",
               current.line, current.col);
    }

    return NULL;  // Retourner NULL en cas d'erreur
}

ASTNode *parse_for_loop(Token *tokens, int *index) {
    
    Token current = tokens[*index];
    ASTNode *for_node = create_node("for_loop", current);


    if (current.type == TOKEN_D_FOR) {
        (*index)++;

        // Vérification de la parenthèse ouvrante
        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;

            // 1. Analyse de la déclaration de la variable (par exemple "int i = 0")
            ASTNode *var_decl = parse_variable_declaration(tokens, index);
            if (var_decl != NULL) {
                add_child(for_node, var_decl);  // Ajout de la déclaration de variable

                // 2. Analyse de la condition (par exemple "i < 10")
                ASTNode *condition = parse_condition_expr(tokens, index);
                if (condition != NULL) {
                    add_child(for_node, condition);  // Ajout de la condition
                    if (tokens[*index].type == TOKEN_SEMICOLON) {
                        (*index)++;  // Consommer ";"
                    } else {
                        printf("Erreur syntaxique dans loop ligne %d, colonne %d: ';' manquant après la déclaration de variable\n", 
                            current.line, current.col);
                        return NULL;
                    }

                    // 3. Analyse de l'incrémentation (par exemple "i = i + 1")
                    ASTNode *increment = parse_increment(tokens, index);
                    if (increment != NULL) {
                        add_child(for_node, increment);  // Ajout de l'incrémentation

                        // Vérification de la parenthèse fermante
                        if (tokens[*index].type == TOKEN_RPAREN) {
                            (*index)++;

                            // 4. Traitement du bloc qui suit
                            ASTNode *block = parse_block(tokens, index);
                            if (block != NULL) {
                                add_child(for_node, block);  // Ajout du bloc
                                // Vérification du nombre d'enfants
                                if (for_node->children_count != 4) {  // Attendre 5 enfants pour la boucle for
                                    printf("Test parse_for_loop failed: expected 4 children, got %d\n", for_node->children_count);
                                    return NULL;
                                }
                                return for_node;
                            } else {
                                printf("Erreur syntaxique dans loop ligne %d, colonne %d: Bloc manquant\n",
                                       current.line, current.col);
                            }
                        } else {
                            printf("Erreur syntaxique dans loop ligne %d, colonne %d: Parenthèse fermante manquante\n", 
                                   current.line, current.col);
                        }
                    } else {
                        printf("Erreur syntaxique dans loop ligne %d, colonne %d: Incrémentation incorrecte\n", 
                               current.line, current.col);
                    }
                } else {
                    printf("Erreur syntaxique dans loop ligne %d, colonne %d: Condition incorrecte\n", 
                           current.line, current.col);
                }
            } else {
                printf("Erreur syntaxique dans loop ligne %d, colonne %d: Déclaration de variable incorrecte\n", 
                       current.line, current.col);
            }
        } else {
            printf("Erreur syntaxique dans loop ligne %d, colonne %d: Parenthèse ouvrante manquante\n", 
                   current.line, current.col);
        }
    }
    printf("Erreur syntaxique dans loop ligne %d, colonne %d\n", 
           current.line, current.col);
    return NULL;  // Retourne NULL en cas d'erreur
}

ASTNode *parse_while_loop(Token *tokens, int *index){
    Token current = tokens[*index];
    ASTNode *while_node = create_node("while_loop", current);

     if (current.type == TOKEN_D_WHILE) {
        (*index)++;

        // Vérification de la parenthèse ouvrante
        if (tokens[*index].type == TOKEN_LPAREN) {
            (*index)++;

            // 1. Analyse de la condition (par exemple "i < 10")
            ASTNode *condition = parse_condition_expr(tokens, index);
            if (condition != NULL) {
                add_child(while_node, condition);  // Ajout de la condition

                // Vérification de la parenthèse fermante
                if (tokens[*index].type == TOKEN_RPAREN) {
                    (*index)++;

                    // 2. Traitement du bloc qui suit
                    ASTNode *block = parse_block(tokens, index);
                    if (block != NULL) {
                        add_child(while_node, block);  // Ajout du bloc
                        // Vérification du nombre d'enfants
                        if (while_node->children_count != 2) {  // Attendre 5 enfants pour la boucle for
                            printf("Test parse_while_loop failed: expected 2 children, got %d\n", while_node->children_count);
                            return NULL;
                        }
                            return while_node;
                    } else {
                        printf("Erreur syntaxique dans loop ligne %d, colonne %d: Bloc manquant\n",
                               current.line, current.col);
                    }
                } else {
                    printf("Erreur syntaxique dans loop ligne %d, colonne %d: Parenthèse fermante manquante\n", 
                           current.line, current.col);
                }
            } else {
                printf("Erreur syntaxique dans loop ligne %d, colonne %d: Condition incorrecte\n", 
                       current.line, current.col);
            }
        } else {
            printf("Erreur syntaxique dans loop ligne %d, colonne %d: Parenthèse ouvrante manquante\n", 
                   current.line, current.col);
        }
    }
    printf("Erreur syntaxique dans loop ligne %d, colonne %d\n", 
           current.line, current.col);
    return NULL;  // Retourne NULL en cas d'erreur

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

void write_tokens_to_file(const Token tokens[], int token_count, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == TOKEN_NBR || tokens[i].type == TOKEN_COMMA) {
            // Si le token est un nombre, écrit son type suivi de son lexeme
            fprintf(file, "%s ", tokens[i].lexeme);
        } else {
            // Sinon, écrit le nom lisible du type du token
            const char *token_name = token_type_to_string(tokens[i].type);
            fprintf(file, "%s ", token_name);
        }

        // Ajoute un saut de ligne après un TOKEN_SEMICOLON
        if (tokens[i].type == TOKEN_SEMICOLON) {
            fprintf(file, "\n");
        }
    }

    fclose(file);
    printf("Tokens écrits dans le fichier %s avec succès.\n", filename);
}

#endif