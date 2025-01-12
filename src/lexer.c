#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "ide.h"

/**
 * Vérifie si une chaîne représente un nombre à virgule flottante
 * @param word La chaîne à vérifier
 * @return 1 si c'est un float valide, 0 sinon
 */
int is_float(const char *word) {
    int has_decimal = 0;
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] == '.') {
            if (has_decimal) return 0; // Rejet si plus d'un point décimal
            has_decimal = 1;
        } else if (!isdigit(word[i])) {
            return 0;
        }
    }
    return has_decimal;
}

/**
 * Vérifie si les deux caractères forment un opérateur composé (==, <=, >=, etc.)
 * @param current Pointeur vers le caractère courant
 * @return Le type de token correspondant ou TOKEN_UNKNOWN
 */
TokenType check_double_operator(const char *current) {
    if (!current || !current[0] || !current[1]) return TOKEN_UNKNOWN;
    
    if (current[0] == '*' && current[1] == '*') return TOKEN_POW;    // Puissance
    if (current[0] == '<' && current[1] == '=') return TOKEN_LEQ;    // Inférieur ou égal
    if (current[0] == '>' && current[1] == '=') return TOKEN_GEQ;    // Supérieur ou égal
    if (current[0] == '=' && current[1] == '=') return TOKEN_EQ;     // Égalité
    if (current[0] == '!' && current[1] == '=') return TOKEN_NEQ;    // Différent
    
    return TOKEN_UNKNOWN;
}

/**
 * Fonction principale de tokenization
 * Analyse le code source et génère une liste de tokens
 * @param source_code Code source à analyser
 * @param tokens Tableau où stocker les tokens générés
 * @return Nombre de tokens générés ou -1 en cas d'erreur
 */
int tokenize(const char *source_code, Token *tokens) {
    int token_cmpt = 0;
    const char *current_char = source_code;
    int line = 1; // Numéro de ligne courant
    int col = 1; // Numéro de colonne courant

    while (*current_char && token_cmpt < MAX_TOKENS) {
        // Ignore les espaces et commentaires
        skip_whitespace_and_comments(&current_char, &col);

        if (*current_char == '\0') break;

        tokens[token_cmpt].line = line;
        tokens[token_cmpt].col = col;

        // Traitement des chaînes de caractères (entre guillemets)
        if (*current_char == '"') {
            current_char++; // Saute le guillemet ouvrant
            col++;
            const char *start = current_char;
            while (*current_char && *current_char != '"') {
                if (*current_char == '\n') {
                    line++;
                    col = 1;
                } else {
                    col++;
                }
                current_char++;
            }

            // Vérifie si la chaîne est bien terminée
            if (*current_char != '"') {
                fprintf(stderr, "Erreur lexicale : Chaîne non terminée ligne %d\n", line);
                return -1;
            }

            // Vérifie la longueur maximale de la chaîne
            int len = current_char - start;
            if (len >= TOKEN_STRING - 1) {
                fprintf(stderr, "Erreur lexicale : Chaîne trop longue ligne %d\n", line);
                return -1;
            }

            strncpy(tokens[token_cmpt].lexeme, start, len);
            tokens[token_cmpt].lexeme[len] = '\0';
            tokens[token_cmpt].type = TOKEN_STR;
            current_char++; // Saute le guillemet fermant
            col++;
            token_cmpt++;
        }
        // Traitement des nombres (entiers et flottants)
        else if (isdigit(*current_char) || *current_char == '.') {
            const char *start = current_char;
            int has_decimal = (*current_char == '.');

            // Lecture du nombre complet
            while (isdigit(*current_char) || *current_char == '.') {
                if (*current_char == '.') {
                    if (has_decimal) {
                        fprintf(stderr, "Erreur lexicale : Nombre invalide (multiple points décimaux) ligne %d colonne %d\n", line, col);
                        return -1;
                    }
                    has_decimal = 1;
                }
                current_char++;
                col++;
            }

            // Copie et validation du nombre
            int len = current_char - start;
            char *word = malloc((len + 1) * sizeof(char));
            if (word == NULL) {
                perror("Erreur d'allocation mémoire");
                return -1;
            }
            strncpy(word, start, len);
            word[len] = '\0';

            tokens[token_cmpt].type = is_float(word) ? TOKEN_NBR : is_number(word);
            strcpy(tokens[token_cmpt].lexeme, word);
            free(word);

            if (tokens[token_cmpt].type == TOKEN_UNKNOWN) {
                fprintf(stderr, "Erreur lexicale : Nombre invalide ligne %d colonne %d\n", line, col);
                return -1;
            }
            token_cmpt++;
        }
        // Traitement des identificateurs et mots-clés
        else if (isalpha(*current_char) || *current_char == '_') {
            const char *start = current_char;
            // Lecture de l'identificateur complet
            while (isalnum(*current_char) || *current_char == '_') {
                current_char++;
                col++;
            }
            int len = current_char - start;
            char *word = malloc((len + 1) * sizeof(char));
            if (word == NULL) {
                perror("Erreur d'allocation mémoire");
                return -1;
            }
            strncpy(word, start, len);
            word[len] = '\0';

            // Traitement spécial des déclarations de variables avec drawv_
            if (strncmp(word, "drawv_", 6) == 0) {
                tokens[token_cmpt].type = TOKEN_DRAWV_;
                strcpy(tokens[token_cmpt].lexeme, "drawv_variable");
                token_cmpt++;

                // Analyse du nom et du type de la variable
                if (strlen(word) > 6) {
                    const char *var_name = word + 6;
                    tokens[token_cmpt].line = line;
                    tokens[token_cmpt].col = col - strlen(var_name);

                    // Extraction du type de variable s'il est spécifié
                    char *type_pos = strstr(var_name, "_");
                    if (type_pos) {
                        *type_pos = '\0'; // Sépare le nom et le type
                        const char *type = type_pos + 1;

                        if (strcmp(type, "nbr") == 0) {
                            tokens[token_cmpt].type = TOKEN_VARNBR;
                        } else if (strcmp(type, "str") == 0) {
                            tokens[token_cmpt].type = TOKEN_VARSTR;
                        } else {
                            tokens[token_cmpt].type = TOKEN_VARIABLE;
                        }

                        strcpy(tokens[token_cmpt].lexeme, var_name);
                    } else {
                        tokens[token_cmpt].type = TOKEN_VARIABLE;
                        strcpy(tokens[token_cmpt].lexeme, var_name);
                    }
                    token_cmpt++;
                }
            } else {
                // Vérification si c'est un mot-clé ou une variable normale
                tokens[token_cmpt].type = is_keyword(word);
                if (tokens[token_cmpt].type == TOKEN_UNKNOWN) {
                    tokens[token_cmpt].type = TOKEN_VARIABLE;
                }
                strcpy(tokens[token_cmpt].lexeme, word);
                token_cmpt++;
            }
            free(word);
        }
        // Traitement des opérateurs et symboles
        else {
            // Vérification des opérateurs à deux caractères
            TokenType double_op = check_double_operator(current_char);
            if (double_op != TOKEN_UNKNOWN) {
                tokens[token_cmpt].type = double_op;
                strncpy(tokens[token_cmpt].lexeme, current_char, 2);
                tokens[token_cmpt].lexeme[2] = '\0';
                current_char += 2;
                col += 2;
                token_cmpt++;
            }
            // Vérification des symboles simples
            else if (is_symbol(*current_char) != TOKEN_UNKNOWN) {
                tokens[token_cmpt].type = is_symbol(*current_char);
                tokens[token_cmpt].lexeme[0] = *current_char;
                tokens[token_cmpt].lexeme[1] = '\0';
                current_char++;
                col++;
                token_cmpt++;
            }
            else {
                // Caractère non reconnu
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg),
                         "Erreur lexicale : Caractère inconnu '%c' ligne %d colonne %d",
                         *current_char, line, col);
                log_to_console(error_msg);
                return -1;
            }
        }

        // Gestion des sauts de ligne
        if (*current_char == '\n') {
            line++;
            col = 1;
        }
    }

    // Ajout du token de fin de fichier
    tokens[token_cmpt].type = TOKEN_EOF;
    strcpy(tokens[token_cmpt].lexeme, "EOF");
    tokens[token_cmpt].line = line;
    tokens[token_cmpt].col = col;
    token_cmpt++;

    return token_cmpt;
}

/**
 * Vérifie si un mot est un mot-clé du langage
 * @param word Le mot à vérifier
 * @return Le type de token correspondant ou TOKEN_UNKNOWN
 */
TokenType is_keyword(const char *word) {
    if (word == NULL || strlen(word) == 0) {
        return TOKEN_UNKNOWN;
    }
    for (int i = 0; i < keywordsDictSize; i++) {
        if (strcmp(word, keywords[i].name) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_UNKNOWN;
}

/**
 * Vérifie si une chaîne représente un nombre entier
 * @param word La chaîne à vérifier
 * @return TOKEN_NBR si c'est un nombre valide, TOKEN_UNKNOWN sinon
 */
TokenType is_number(const char *word) {
    if (word == NULL || strlen(word) == 0) {
        return TOKEN_UNKNOWN;
    }
    for (int i = 0; i < strlen(word); i++) {
        if (!isdigit(word[i])) {
            return TOKEN_UNKNOWN;
        }
    }
    return TOKEN_NBR;
}

/**
 * Vérifie si un caractère est un symbole du langage
 * @param c Le caractère à vérifier
 * @return Le type de token correspondant ou TOKEN_UNKNOWN
 */
TokenType is_symbol(char c) {
    if (c == '\0') {
        return TOKEN_UNKNOWN;
    }
    for (int i = 0; i < symbolsDictSize; i++) {
        if (c == symbols[i].name[0]) {
            return symbols[i].type;
        }
    }
    return TOKEN_UNKNOWN;
}

/**
 * Ignore les espaces blancs et les commentaires
 * @param current_char Pointeur vers le caractère courant (mis à jour)
 * @param col Pointeur vers la colonne courante (mise à jour)
 */
void skip_whitespace_and_comments(const char **current_char, int *col) {
    while (**current_char) {
        if (**current_char == ' ') {
            (*current_char)++;
            (*col)++;
        } else if (**current_char == '\t') {
            (*current_char)++;
            (*col) += 4;  // Un tab compte pour 4 espaces
        } else if (**current_char == '\r') {
            (*current_char)++;
            (*col)++;
        } else if (**current_char == '/' && *(*current_char + 1) == '/') {
            // Ignore les commentaires jusqu'à la fin de la ligne
            while (**current_char && **current_char != '\n') {
                (*current_char)++;
            }
            if (**current_char == '\n') (*current_char)++;
            (*col) = 1;
        } else {
            break;
        }
    }
}