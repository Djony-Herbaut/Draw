#include <stdlib.h>
#include <stdio.h>
#include "lexer.h"

tokenize(const char *source_code, Token *tokens) {
    // 1. Initialise un compteur pour suivre le nombre de tokens générés.
    int cmpt =0;
    // 2. Parcourt le code caractère par caractère (ou mot par mot).
    // 3. Détecte les espaces, tabulations, et commentaires pour les ignorer.
    // 4. Identifie les mots-clés, nombres, et symboles.
    //    - Si un mot correspond à un mot-clé (ex. "drawcircle"), crée un token avec le type approprié.
    //    - Si c’est un nombre, crée un token de type TOKEN_NBR.
    //    - Si c’est un symbole (ex. '('), ?crée? un token de type TOKEN_LPAREN.
    // 5. Traite les erreurs lexicales (éléments inconnus).
    // 6. Renvoie le nombre total de tokens générés.
}
int is_keyword(const char *word) {
    // 1. Compare le mot avec la liste des mots-clés.
    // 2. Si une correspondance est trouvée, retourne 1 (true).
    // 3. Sinon, retourne 0 (false).
}
int is_number(const char *word) {
    // 1. Vérifie que chaque caractère de la chaîne est un chiffre.
    // 2. Retourne 1 (true) si tous les caractères sont des chiffres, sinon 0 (false).
}
int is_symbol(char c) {
    // 1. Compare le caractère avec la liste des symboles possibles : (), {}, =, etc.
    // 2. Retourne 1 (true) si c’est un symbole valide, sinon 0 (false).
}
void skip_whitespace_and_comments(const char **current_char) {
    // 1. Avance le pointeur tant que le caractère actuel est un espace ou une tabulation.
    // 2. Si un commentaire est détecté (ex. "//"), avance jusqu’à la fin de la ligne.
}