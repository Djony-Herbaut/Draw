#ifndef READ_TOKEN_H
#define READ_TOKEN_H

#include "token.h" // Inclusion du fichier contenant la définition des tokens

#define MAX_LINE_LENGTH 256

// Fonction pour initialiser un canvas avec Tkinter
void initialize_canvas();

// Fonction pour mettre à jour la position du curseur
void update_cursor_position(int x, int y);

// Fonction pour exécuter une commande Python sur le canvas
void execute_canvas_command(const char *command);

// Fonction pour exécuter un token
void execute_token(Token token, const char *params);

// Fonction pour identifier les tokens à partir d'une chaîne
Token identify_token(const char *token_str);

// Fonction principale pour lire et analyser le fichier
void parse_file(const char *filename);

#endif // READ_TOKEN_H
