#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "lexer.h"

int tokenize(const char *source_code, Token *tokens) {
    // 1. Initialise un compteur pour suivre le nombre de tokens générés.
    int token_cmpt = 0 ;
    const char *current_char = source_code;  
    // 2. Parcourt le code caractère par caractère (ou mot par mot).
    // 3. Détecte les espaces, tabulations, et commentaires pour les ignorer.
    // 4. Identifie les mots-clés, nombres, et symboles.
    //    - Si un mot correspond à un mot-clé (ex. "drawcircle"), crée un token avec le type approprié.
    //    - Si c’est un nombre, crée un token de type TOKEN_NBR.
    //    - Si c’est un symbole (ex. '('), ?crée? un token de type TOKEN_LPAREN.
    // 5. Traite les erreurs lexicales (éléments inconnus).
    // 6. Renvoie le nombre total de tokens générés.
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
            return symbols[i].type;
        }
    }
    // 2. Si une correspondance est trouvée, retourne le token correspondant (true).
    return TOKEN_UNKNOWN;
    // 3. Sinon, retourne unknown(pas de correspondance).
}
void skip_whitespace_and_comments(const char **current_char) {
    // 1. Avance le pointeur tant que le caractère actuel est un espace ou une tabulation.
    // 2. Si un commentaire est détecté (ex. "//"), avance jusqu’à la fin de la ligne.
}