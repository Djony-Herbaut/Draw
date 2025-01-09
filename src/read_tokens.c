#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_LINE_LENGTH 256
#define MAX_LEXEME_LENGTH 100
#define MAX_PARAMETERS 10

typedef struct {
    char type[MAX_LEXEME_LENGTH]; // Type du token
    char lexeme[MAX_LEXEME_LENGTH]; // Lexème associé
    int parameters[MAX_PARAMETERS]; // Paramètres de la fonction
    int param_count; // Nombre de paramètres
} ParsedToken;

// Prototypes
void execute_tkinter_command(const ParsedToken *token);
void process_tokens_file(const char *filepath);

/**
 * @brief Lit un fichier contenant des tokens et traite les commandes tkinter correspondantes.
 */
void process_tokens_file(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    char line[MAX_LINE_LENGTH];
    ParsedToken token;
    int stop_processing = 0; // Indicateur pour arrêter la boucle

    printf("Lecture et exécution des instructions à partir du fichier : %s\n", filepath);

    // Initialiser l'interpréteur Python
    Py_Initialize();
    PyRun_SimpleString("import tkinter as tk\n"
                       "root = tk.Tk()\n"
                       "canvas = tk.Canvas(root, width=800, height=600, bg='white')\n"
                       "canvas.pack()\n"
                       "cursor = None\n");

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL && !stop_processing) {
        // Réinitialiser le token
        memset(&token, 0, sizeof(ParsedToken));

        // Extraire les informations de la ligne
        char *token_type = strtok(line, " ");
        if (!token_type) continue;

        strcpy(token.type, token_type);

        // Gérer TOKEN_EOF directement pour arrêter le traitement
        if (strcmp(token.type, "TOKEN_EOF") == 0) {
            stop_processing = 1;
            continue; // Arrêter la boucle après ce token
        }

        // Lire les paramètres associés
        int param_count = 0;
        char *param = strtok(NULL, " ");
        while (param && strcmp(param, "TOKEN_SEMICOLON") != 0) {
            token.parameters[param_count++] = atoi(param);
            param = strtok(NULL, " ");
        }
        token.param_count = param_count;

        // Exécuter la commande tkinter correspondante
        execute_tkinter_command(&token);
    }

    // Mettre à jour et lancer l'interface tkinter
    PyRun_SimpleString("root.mainloop()");

    // Finaliser l'interpréteur Python
    Py_Finalize();

    fclose(file);
}


/**
 * @brief Exécute une commande tkinter en fonction du token analysé.
 */
void execute_tkinter_command(const ParsedToken *token) {
    char command[512] = {0};

    if (strcmp(token->type, "TOKEN_DRAWCREATE_CURSOR") == 0) {
        snprintf(command, sizeof(command), 
                 "cursor = canvas.create_oval(50, 50, 60, 60, fill='black')");
    } else if (strcmp(token->type, "TOKEN_DRAWSET_POS") == 0 && token->param_count >= 2) {
        snprintf(command, sizeof(command), 
                 "canvas.move(cursor, %d - canvas.coords(cursor)[0], %d - canvas.coords(cursor)[1])",
                 token->parameters[0], token->parameters[1]);
    } else if (strcmp(token->type, "TOKEN_DRAWCIRCLE") == 0 && token->param_count >= 4) {
        snprintf(command, sizeof(command), 
                 "canvas.create_oval(%d, %d, %d, %d, outline='black')",
                 token->parameters[0], token->parameters[1],
                 token->parameters[2], token->parameters[3]);
    } else if (strcmp(token->type, "TOKEN_DRAWUPDATE") == 0) {
        snprintf(command, sizeof(command), "canvas.update()");
    } else if (strcmp(token->type, "TOKEN_DRAWCLEAR_SCREEN") == 0) {
        snprintf(command, sizeof(command), "canvas.delete('all')");
    } else if (strcmp(token->type, "TOKEN_EOF") == 0) {
        snprintf(command, sizeof(command), "root.destroy()");
    } else {
        printf("Commande inconnue ou non prise en charge : %s\n", token->type);
        return;
    }

    // Exécuter la commande Python
    PyRun_SimpleString(command);
}

int main() {
    const char *filepath = "../output/tokens.txt"; // Chemin du fichier contenant les tokens
    process_tokens_file(filepath);
    return 0;
}
