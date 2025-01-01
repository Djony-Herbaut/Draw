#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

// Déclarations des fonctions (Indispensable pour que le compilateur connaisse les signatures avant l'appel) peut-être à mettre dans lexer.h
void skip_whitespace_and_comments(const char **current_char);
TokenType is_number(const char *word);
TokenType is_keyword(const char *word);
TokenType is_symbol(char c);

int tokenize(const char *source_code, Token *tokens) {
    // Retourne le nombre de tokens générés, ou -1 en cas d'erreur.
    int token_cmpt = 0;             // Compteur de tokens
    const char *current_char = source_code; // Pointeur courant dans le code source (maintenant const car on ne modifie pas la source directement)
    int line = 1;                   // Numéro de ligne courant
    int col = 1;                    // Numéro de colonne courant

    // Boucle principale : parcourt le code source caractère par caractère
    while (*current_char && token_cmpt < MAX_TOKENS) {
        skip_whitespace_and_comments(&current_char); // Ignore les espaces et les commentaires

        if (*current_char == '\0') break; // Fin de la chaîne source

        tokens[token_cmpt].line = line; // Enregistre le numéro de ligne du token
        tokens[token_cmpt].col = col;  // Enregistre le numéro de colonne du token

        if (isdigit(*current_char)) { // Si le caractère courant est un chiffre
            const char *start = current_char; // Marque le début du nombre
            while (isdigit(*current_char)) { // Avance tant qu'on a des chiffres
                current_char++;
                col++; // Incrémente la colonne
            }
            int len = current_char - start; // Calcule la longueur du nombre
            char *word = malloc((len + 1) * sizeof(char)); // Alloue de la mémoire pour stocker le nombre sous forme de chaîne
            if (word == NULL) {
                perror("Erreur d'allocation mémoire");
                return -1;
            }
            strncpy(word, start, len); // Copie le nombre dans la chaîne allouée
            word[len] = '\0';          // Ajoute le caractère nul de fin de chaîne
            tokens[token_cmpt].type = is_number(word); // Détermine le type de token (TOKEN_NBR)
            strcpy(tokens[token_cmpt].lexeme, word); // Copie le lexème dans le token
            free(word); // Libère la mémoire allouée
            if (tokens[token_cmpt].type == TOKEN_UNKNOWN) { // Gestion d'erreur si le nombre est invalide
                fprintf(stderr, "Erreur lexicale : Nombre invalide ligne %d colonne %d\n", line, col);
                return -1; // Retourne une erreur
            }
            token_cmpt++;
        } else if (isalpha(*current_char) || *current_char == '_') { // Si le caractère courant est une lettre ou un underscore (pour les identifiants)
            const char *start = current_char; // Marque le début du mot
            while (isalnum(*current_char) || *current_char == '_') { // Avance tant qu'on a des caractères alphanumériques ou des underscores
                current_char++;
                col++; // Incrémente la colonne
            }
            int len = current_char - start;
            char *word = malloc((len + 1) * sizeof(char));
            if (word == NULL) {
                perror("Erreur d'allocation mémoire");
                return -1;
            }
            strncpy(word, start, len);
            word[len] = '\0'; // Ajoute le caractère nul de fin de chaîne
            tokens[token_cmpt].type = is_keyword(word); // Vérifie si le mot est un mot-clé
            if (tokens[token_cmpt].type == TOKEN_UNKNOWN) { // Si ce n'est pas un mot clé, c'est une variable
                tokens[token_cmpt].type = TOKEN_VARIABLE; // Assigne le type TOKEN_VARIABLE
            }
            strcpy(tokens[token_cmpt].lexeme, word); // Copie le lexème dans le token
            free(word); // Libère la mémoire allouée
            token_cmpt++;
        } else if (is_symbol(*current_char) != TOKEN_UNKNOWN) { // Si le caractère courant est un symbole
            tokens[token_cmpt].type = is_symbol(*current_char); // Récupère le type du symbole
            tokens[token_cmpt].lexeme[0] = *current_char;     // Stocke le symbole dans le lexème
            tokens[token_cmpt].lexeme[1] = '\0';             // Ajoute le caractère nul de fin de chaîne
            current_char++;
            col++;
            token_cmpt++;
        } else { // Si le caractère courant n'est ni un chiffre, ni une lettre, ni un symbole connu
            fprintf(stderr, "Erreur lexicale : Caractère inconnu '%c' ligne %d colonne %d\n", *current_char, line, col);
            return -1; // Retourne une erreur
        }
        if (*current_char == '\n') { // Si on rencontre un retour à la ligne
            line++;             // Incrémente le numéro de ligne
            col = 1;              // Réinitialise le numéro de colonne
        }
    }

    tokens[token_cmpt].type = TOKEN_EOF; // Ajoute le token de fin de fichier
    strcpy(tokens[token_cmpt].lexeme, "EOF");
    tokens[token_cmpt].line = line;
    tokens[token_cmpt].col = col;
    token_cmpt++;

    return token_cmpt; // Retourne le nombre total de tokens générés
}

TokenType is_keyword(const char *word) {
    if (word == NULL || strlen(word) == 0) {
        return TOKEN_UNKNOWN;
    }
    for (int i = 0; i < keywordsDictSize; i++) {
        if (strcmp(word, keywords[i].name) == 0) { // Utilisation de strcmp pour comparer les chaines
            return keywords[i].type;
        }
    }
    return TOKEN_UNKNOWN;
}

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

void skip_whitespace_and_comments(const char **current_char) {
    while (**current_char) {
        if (**current_char == ' ' || **current_char == '\t' || **current_char == '\r') {
            (*current_char)++;
        } else if (**current_char == '/' && *(*current_char + 1) == '/') {
            while (**current_char && **current_char != '\n') {
                (*current_char)++;
            }
            if (**current_char == '\n') (*current_char)++; // Important pour passer à la ligne suivante après un commentaire
        } else {
            break;
        }
    }
}
int main() {
    // Définir des cas de test
    const char *test_codes[] = {
        "",                                      // Code vide
        "drawcircle 10 ( drawrect 20 )",        // Code simple
        " drawcircle // ceci est un commentaire\n 10", // Avec commentaire et espaces
        "drawv_maVariable = 5",                   // Avec une variable
        "drawcircle 12345678901234567890", //Nombre très long
        "drawcircle 10.5", //Nombre à virgule
        "drawcircle 10a", //Nombre invalide
        "drawcircle (", //Symbole sans opérande
        "drawcircle )", //Symbole sans opérande
        "drawcircle =", //Symbole sans opérande
        "drawcircle >", //Symbole sans opérande
        "drawcircle <", //Symbole sans opérande
        "drawcircle +", //Symbole sans opérande
        "drawcircle -", //Symbole sans opérande
        "drawcircle *", //Symbole sans opérande
        "drawcircle /", //Symbole sans opérande
        "drawcircle ,", //Symbole sans opérande
        "drawcreate_cursor drawsetpos drawgo drawsetx drawsety drawshowcursor drawhidecursor drawcursorcolor drawpensize drawmoveforward drawmovebackward drawpivotleft drawpivotright drawcircle drawdot drawarc drawupdate drawpenup drawpendown drawshape drawclearscreen drawv_variable drawf d_if d_else d_for d_while drawtsleep", //Test de tous les mots clés
    };
    int num_tests = sizeof(test_codes) / sizeof(test_codes[0]);

    for (int i = 0; i < num_tests; i++) {
        printf("\n--- Test %d ---\nCode source : \"%s\"\n", i + 1, test_codes[i]);

        Token tokens[MAX_TOKENS];
        int num_tokens = tokenize(test_codes[i], tokens);

        if (num_tokens > 0) {
            printf("Nombre de tokens : %d\n", num_tokens);
            for (int j = 0; j < num_tokens; j++) {
                printf("Token %d : Type = %d, Lexeme = \"%s\", Ligne = %d, Colonne = %d\n", j, tokens[j].type, tokens[j].lexeme, tokens[j].line, tokens[j].col);
            }
        } else if (num_tokens == -1) {
            printf("Erreur lors de la tokenisation\n");
        } else {
            printf("Aucun token trouvé.\n");
        }
    }

    return 0;
}