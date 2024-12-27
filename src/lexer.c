#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> // Pour isdigit, isalpha, isalnum
#include "lexer.h"// Pour les définitions de Token, TokenType, etc.
#include <string.h> // Pour strncpy, strcpy

int tokenize(const char *source_code, Token *tokens) {
// Retourne le nombre de tokens générés, ou -1 en cas d'erreur.
    int token_cmpt = 0;             
    char *current_char = (char *)source_code; // Pointeur courant dans le code source (cast nécessaire car on va le modifier)
    int line = 1;                   
    int col = 1;                    

    // Boucle principale : parcourt le code source caractère par caractère
    while (*current_char && token_cmpt < MAX_TOKENS) { 
        skip_whitespace_and_comments(&current_char); 

        if (*current_char == '\0') break; 

        tokens[token_cmpt].line = line; // Enregistre le numéro de ligne du token
        tokens[token_cmpt].col = col;  // Enregistre le numéro de colonne du token

        if (isdigit(*current_char)) {
            char *start = current_char; // Marque le début du nombre
            while (isdigit(*current_char)) { 
                current_char++;
                col++; // Incremente la colonne
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
            free(word);
            if (tokens[token_cmpt].type == TOKEN_UNKNOWN) { // Gestion d'erreur si le nombre est invalide
                fprintf(stderr, "Erreur lexicale : Nombre invalide ligne %d colonne %d\n", line, col);
                return -1; // Retourne une erreur
            }
            token_cmpt++; 

        } else if (isalpha(*current_char) || *current_char == '_') { // Si le caractère courant est une lettre ou un underscore (pour les identifiants)
            char *start = current_char; // Marque le début du mot
            while (isalnum(*current_char) || *current_char == '_') { // Avance tant qu'on a des caractères alphanumériques ou des underscores
                current_char++;
                col++; // Incremente la colonne
            }
            int len = current_char - start; 
            char *word = malloc((len + 1) * sizeof(char)); 
            if (word == NULL) {
                perror("Erreur d'allocation mémoire");
                return -1;
            }
            strncpy(word, start, len); 
            word[len] = '\0';          // Ajoute le caractère nul de fin de chaîne
            tokens[token_cmpt].type = is_keyword(word); // Vérifie si le mot est un mot-clé
            if (tokens[token_cmpt].type == TOKEN_UNKNOWN) { // Si ce n'est pas un mot clé, c'est une variable
                tokens[token_cmpt].type = TOKEN_VARIABLE; // Assigne le type TOKEN_VARIABLE
                strcpy(tokens[token_cmpt].lexeme, word); // Copie le lexème dans le token
            } else {
                strcpy(tokens[token_cmpt].lexeme, word); // Copie le lexème dans le token
            }
            free(word); 
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

    tokens[token_cmpt].type = TOKEN_EOF;
    strcpy(tokens[token_cmpt].lexeme, "EOF");
    tokens[token_cmpt].line = line;
    tokens[token_cmpt].col = col;
    token_cmpt++;

    return token_cmpt; // Retourne le nombre total de tokens générés
}

TokenType is_keyword(const char *word) {
    // Verification que le parametre ne soit pas vide
    if (word == NULL || strlen(word) == 0) {
        return TOKEN_UNKNOWN;
    }
    // 1. Compare le mot avec la liste des mots-clés.
    for(int i = 0 ; i < keywordsDictSize ; i++) {
        if(word == keywords[i].name){
            return keywords[i].type;
        }
    }
    // 2. Si une correspondance est trouvée, retourne le token correspondant (true).
    return TOKEN_UNKNOWN;
    // 3. Sinon, retourne unknown(pas de correspondance).
}
TokenType is_number(const char *word) {
    // Verification que le parametre ne soit pas vide
    if (word == NULL || strlen(word) == 0) {
        return TOKEN_UNKNOWN;
    }
    // 1. Vérifie que chaque caractère de la chaîne est un chiffre.
    for(int i=0 ; i < strlen(word) ; i++){
        if(!isdigit(word[i])){
            return TOKEN_UNKNOWN;
        }
        // Si un caractère n'est pas un chiffre, retourne TOKEN_UNKNOWN
    }
    // 2. Si tous les caractères sont des chiffres, retourne TOKEN_NBR
    return TOKEN_NBR;
}
TokenType is_symbol(char c) {
    // Verification que le parametre ne soit pas vide
    if (c == NULL) {
        return TOKEN_UNKNOWN;
    }
    // 1. Compare le caractère avec la liste des symboles possibles : (), {}, =, etc.
    for(int i = 0 ; i < symbolsDictSize ; i++) {
        if(c == symbols[i].name){
            return symbols[i].type;// 2. Si une correspondance est trouvée, retourne le token correspondant (true).
        }
    }
    return TOKEN_UNKNOWN;
    // 3. Sinon, retourne unknown(pas de correspondance).
}
void skip_whitespace_and_comments(const char **current_char) {
// 1. Avance le pointeur tant que le caractère actuel est un espace ou une tabulation.
 while (**current_char) {
        if (**current_char == ' ' || **current_char == '\t' || **current_char == '\r') {
            (*current_char)++;
// 2. Si un commentaire est détecté (ex. "//"), avance jusqu’à la fin de la ligne.
        } else if (**current_char == '/' && *(*current_char + 1) == '/') {
            while (**current_char && **current_char != '\n') {
                (*current_char)++;
            }
           if (**current_char == '\n') (*current_char)++; // Gestion du \n après un commentaire
        } else {
            break; // Important : on sort si on n'a ni espace, ni commentaire, ni \n
        }
    }
}