#ifndef READ_TOKEN_H
#define READ_TOKEN_H

#include "token.h" // Inclusion du fichier contenant la définition des tokens
#include <stdbool.h>

#define MAX_LINE_LENGTH 256

// Fonction pour initialiser un canvas avec Tkinter
void initialize_turtle();

// Fonction pour mettre à jour la position du curseur
void update_cursor_position(int x, int y);

// Fonction pour exécuter une commande Python sur le canvas
void execute_turtle_command(const char *command);

// Fonction pour exécuter un token
void execute_token(TokenType token, const char *params);

// Fonction pour identifier les tokens à partir d'une chaîne
TokenType identify_token(const char *token_str);

// Fonction principale pour lire et analyser le fichier
void read_file(const char *filename);

bool validate_params(TokenType token, const char *params);

#endif // READ_TOKEN_H

