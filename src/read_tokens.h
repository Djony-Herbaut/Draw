#ifndef READ_TOKENS_H
#define READ_TOKENS_H

#include "token.h"

// Taille maximale d'une ligne dans le fichier de tokens
#define MAX_LINE_LENGTH 256

/**
 * @brief Lit un fichier contenant des tokens et affiche leurs informations.
 * 
 * @param filename Le chemin vers le fichier contenant les tokens.
 */
void read_tokens_file(const char *filename);

#endif // READ_TOKENS_H
