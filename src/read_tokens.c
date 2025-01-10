#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h" // Inclusion du fichier contenant la définition des tokens

#define MAX_LINE_LENGTH 256

// Variables globales pour gérer l'état du curseur
int cursor_x = 640; // Position initiale du curseur (centre du canvas)
int cursor_y = 360;
char cursor_color[20] = "black"; // Couleur initiale
int pen_size = 1; // Taille initiale du stylo

// Gestion du contexte Python
PyObject *canvas = NULL;
PyObject *root = NULL;

// Fonction pour initialiser un canvas avec Tkinter
void initialize_canvas() {
    // Initialiser l'interpréteur Python
    Py_Initialize();

    // Script Python pour créer une fenêtre Tkinter avec un canvas
    const char *python_script =
        "import tkinter as tk\n"
        "global root, canvas\n"
        "root = tk.Tk()\n"
        "root.title('Canvas Example')\n"
        "canvas = tk.Canvas(root, width=1280, height=720, bg='white')\n"
        "canvas.pack()\n";

    // Exécuter le script Python
    int result = PyRun_SimpleString(python_script);
    if (result != 0) {
        fprintf(stderr, "Erreur : Échec de l'exécution du script Python.\n");
        Py_Finalize();
        exit(EXIT_FAILURE);
    }

    // Obtenir les objets Python pour canvas et root
    PyObject *main_module = PyImport_AddModule("__main__");
    if (main_module) {
        PyObject *main_dict = PyModule_GetDict(main_module);
        canvas = PyDict_GetItemString(main_dict, "canvas");
        root = PyDict_GetItemString(main_dict, "root");

        if (!canvas || !root) {
            fprintf(stderr, "Erreur : Impossible de récupérer les objets Tkinter.\n");
            Py_Finalize();
            exit(EXIT_FAILURE);
        }
        Py_INCREF(canvas);
        Py_INCREF(root);
    }
}

// Fonction pour mettre à jour la position du curseur
void update_cursor_position(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

// Fonction pour exécuter une commande Python sur le canvas
void execute_canvas_command(const char *command) {
    int result = PyRun_SimpleString(command);
    if (result != 0) {
        fprintf(stderr, "Erreur : Échec de l'exécution de la commande Python.\n");
    }
}

// Fonction pour exécuter un token
void execute_token(TokenType token, const char *params) {
    char command[256];
    switch (token) {
        case TOKEN_DRAWCREATE_CURSOR: {
            printf("TOKEN_DRAWCREATE_CURSOR : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWSET_POS: {
            int x, y;
            sscanf(params, "%d %d", &x, &y);
            update_cursor_position(x, y);
            printf("Position du curseur mise à jour : (%d, %d)\n", cursor_x, cursor_y);
            break;
        }
        case TOKEN_DRAWGO: {
            printf("TOKEN_DRAWGO : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWSETX: {
            int x;
            sscanf(params, "%d", &x);
            cursor_x = x;
            printf("Position X du curseur mise à jour : %d\n", cursor_x);
            break;
        }
        case TOKEN_DRAWSETY: {
            int y;
            sscanf(params, "%d", &y);
            cursor_y = y;
            printf("Position Y du curseur mise à jour : %d\n", cursor_y);
            break;
        }
        case TOKEN_DRAWSHOW_CURSOR: {
            printf("TOKEN_DRAWSHOW_CURSOR : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWHIDE_CURSOR: {
            printf("TOKEN_DRAWHIDE_CURSOR : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWCURSOR_COLOR: {
            strncpy(cursor_color, params, sizeof(cursor_color) - 1);
            cursor_color[sizeof(cursor_color) - 1] = '\0';
            printf("Couleur du curseur mise à jour : %s\n", cursor_color);
            break;
        }
        case TOKEN_DRAWPEN_SIZE: {
            sscanf(params, "%d", &pen_size);
            printf("Taille du stylo mise à jour : %d\n", pen_size);
            break;
        }
        case TOKEN_DRAWMOVE_FORWARD: {
            int distance;
            sscanf(params, "%d", &distance);
            cursor_y -= distance; // Mouvement vers le haut
            printf("Déplacement vers l'avant : nouvelle position (%d, %d)\n", cursor_x, cursor_y);
            break;
        }
        case TOKEN_DRAWMOVE_BACKWARD: {
            int distance;
            sscanf(params, "%d", &distance);
            cursor_y += distance; // Mouvement vers le bas
            printf("Déplacement vers l'arrière : nouvelle position (%d, %d)\n", cursor_x, cursor_y);
            break;
        }
        case TOKEN_DRAWPIVOT_LEFT: {
            printf("TOKEN_DRAWPIVOT_LEFT : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWPIVOT_RIGHT: {
            printf("TOKEN_DRAWPIVOT_RIGHT : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWCIRCLE: {
            int radius;
            sscanf(params, "%d", &radius);
            snprintf(command, sizeof(command),
                     "canvas.create_oval(%d-%d, %d-%d, %d+%d, %d+%d, outline='%s', width=%d)",
                     cursor_x, radius, cursor_y, radius, cursor_x, radius, cursor_y, radius,
                     cursor_color, pen_size);
            execute_canvas_command(command);
            printf("Cercle dessiné avec un rayon de %d à la position (%d, %d)\n", radius, cursor_x, cursor_y);
            break;
        }
        case TOKEN_DRAWDOT: {
            snprintf(command, sizeof(command),
                     "canvas.create_oval(%d-1, %d-1, %d+1, %d+1, fill='%s')",
                     cursor_x, cursor_y, cursor_x, cursor_y, cursor_color);
            execute_canvas_command(command);
            printf("Point dessiné à la position (%d, %d) avec la couleur %s\n", cursor_x, cursor_y, cursor_color);
            break;
        }
        case TOKEN_DRAWARC: {
            printf("TOKEN_DRAWARC : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWUPDATE: {
            printf("TOKEN_DRAWUPDATE : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWPENUP: {
            printf("TOKEN_DRAWPENUP : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWPENDOWN: {
            printf("TOKEN_DRAWPENDOWN : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWSHAPE: {
            printf("TOKEN_DRAWSHAPE : Non implémenté (params: %s)\n", params);
            break;
        }
        case TOKEN_DRAWCLEAR_SCREEN: {
            snprintf(command, sizeof(command), "canvas.delete('all')");
            execute_canvas_command(command);
            printf("Écran effacé.\n");
            break;
        }
        case TOKEN_COMMA:
        case TOKEN_SEMICOLON:
        case TOKEN_EOF:
            // Aucun effet direct sur le canvas
            printf("Token de syntaxe détecté : %s\n", params ? params : "(aucun paramètre)");
            break;
        default:
            printf("Token inconnu : Non implémenté (params: %s)\n", params);
            break;
    }
}


// Fonction principale pour lire et analyser le fichier
void parse_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char *token_str = strtok(line, " ");
        while (token_str) {
            char *params = strtok(NULL, "\n"); // Lire les paramètres après le token
            Token token = identify_token(token_str);
            execute_token(token, params ? params : "");
            token_str = strtok(NULL, " "); // Passer au token suivant
        }
    }

    fclose(file);
}

int main() {
    printf("Initialisation du canvas avec Tkinter...\n");
    initialize_canvas();

    const char *filename = "../tokens.txt";
    parse_file(filename);

    printf("Lancement de la boucle Tkinter...\n");
    execute_canvas_command("root.mainloop()");
    
    Py_Finalize();
    return 0;
}
