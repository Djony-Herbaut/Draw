#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

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

// Fonction pour identifier un token, retourn -1 en cas d'échec
TokenType identify_token(const char *token_str) {
    if (strcmp(token_str, "TOKEN_DRAWCREATE_CURSOR") == 0)      return TOKEN_DRAWCREATE_CURSOR;
    if (strcmp(token_str, "TOKEN_DRAWSET_POS") == 0)            return TOKEN_DRAWSET_POS;
    if (strcmp(token_str, "TOKEN_DRAWGO") == 0)                 return TOKEN_DRAWGO;
    if (strcmp(token_str, "TOKEN_DRAWSETX") == 0)               return TOKEN_DRAWSETX;
    if (strcmp(token_str, "TOKEN_DRAWSETY") == 0)               return TOKEN_DRAWSETY;
    if (strcmp(token_str, "TOKEN_DRAWSHOW_CURSOR") == 0)        return TOKEN_DRAWSHOW_CURSOR;
    if (strcmp(token_str, "TOKEN_DRAWHIDE_CURSOR") == 0)        return TOKEN_DRAWHIDE_CURSOR;
    if (strcmp(token_str, "TOKEN_DRAWCURSOR_COLOR") == 0)       return TOKEN_DRAWCURSOR_COLOR;
    if (strcmp(token_str, "TOKEN_DRAWPEN_SIZE") == 0)           return TOKEN_DRAWPEN_SIZE;
    if (strcmp(token_str, "TOKEN_DRAWMOVE_FORWARD") == 0)       return TOKEN_DRAWMOVE_FORWARD;
    if (strcmp(token_str, "TOKEN_DRAWMOVE_BACKWARD") == 0)      return TOKEN_DRAWMOVE_BACKWARD;
    if (strcmp(token_str, "TOKEN_DRAWPIVOT_LEFT") == 0)         return TOKEN_DRAWPIVOT_LEFT;
    if (strcmp(token_str, "TOKEN_DRAWPIVOT_RIGHT") == 0)        return TOKEN_DRAWPIVOT_RIGHT;
    if (strcmp(token_str, "TOKEN_DRAWCIRCLE") == 0)             return TOKEN_DRAWCIRCLE;
    if (strcmp(token_str, "TOKEN_DRAWDOT") == 0)                return TOKEN_DRAWDOT;
    if (strcmp(token_str, "TOKEN_DRAWARC") == 0)                return TOKEN_DRAWARC;
    if (strcmp(token_str, "TOKEN_DRAWUPDATE") == 0)             return TOKEN_DRAWUPDATE;
    if (strcmp(token_str, "TOKEN_DRAWPENUP") == 0)              return TOKEN_DRAWPENUP;
    if (strcmp(token_str, "TOKEN_DRAWPENDOWN") == 0)            return TOKEN_DRAWPENDOWN;
    if (strcmp(token_str, "TOKEN_DRAWSHAPE") == 0)              return TOKEN_DRAWSHAPE;
    if (strcmp(token_str, "TOKEN_DRAWCLEAR_SCREEN") == 0)       return TOKEN_DRAWCLEAR_SCREEN;
    if (strcmp(token_str, "TOKEN_COMMA") == 0)                  return TOKEN_COMMA;
    if (strcmp(token_str, "TOKEN_SEMICOLON") == 0)              return TOKEN_SEMICOLON;
    if (strcmp(token_str, "TOKEN_EOF") == 0)                    return TOKEN_EOF;
    return -1;
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
        // A MODIFIER (tracer le trait si pendown)
        case TOKEN_DRAWGO: {
            int x, y;
            sscanf(params, "%d %d", &x, &y);
            update_cursor_position(x, y);
            printf("Position du curseur mise à jour : (%d, %d)\n", cursor_x, cursor_y);
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
        // A IMPLEMENTER SI PASSAGE A TURTLE 
        // case TOKEN_DRAWSHOW_CURSOR: {
        //     printf("TOKEN_DRAWSHOW_CURSOR : Non implémenté (params: %s)\n", params);
        //     break;
        // }
        //
        // case TOKEN_DRAWHIDE_CURSOR: {
        //     printf("TOKEN_DRAWHIDE_CURSOR : Non implémenté (params: %s)\n", params);
        //     break;
        // }
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
        // IMPLÉMENTER CA POUR LES DIAGONALES
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
            int radius;
            sscanf(params, "%d %d", &radius);
            snprintf(command, sizeof(command),
                     "canvas.create_arc(%d-%d, %d-%d, %d+%d, %d+%d, outline='%s', width=%d)",
                     cursor_x, radius, cursor_y, radius, cursor_x, radius, cursor_y, radius,
                     cursor_color, pen_size);
            execute_canvas_command(command);
            printf("Cercle dessiné avec un rayon de %d à la position (%d, %d)\n", radius, cursor_x, cursor_y);
            break;
        }
        //INUTILE 
        // case TOKEN_DRAWUPDATE: {
        //     printf("TOKEN_DRAWUPDATE : Non implémenté (params: %s)\n", params);
        //     break;
        // }

        // A implémenter
        case TOKEN_DRAWPENUP: {
            printf("TOKEN_DRAWPENUP : Non implémenté (params: %s)\n", params);
            break;
        }
        
        case TOKEN_DRAWPENDOWN: {
            printf("TOKEN_DRAWPENDOWN : Non implémenté (params: %s)\n", params);
            break;
        }
        // A FAIRE QUE SI EN TURLE
        // case TOKEN_DRAWSHAPE: {
        //     printf("TOKEN_DRAWSHAPE : Non implémenté (params: %s)\n", params);
        //     break;
        // }
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
void read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char *token_str = strtok(line, " ");
        while (token_str) {
            // Lire les paramètres après le token
            char *params = strtok(NULL, "\n");
            // Récupère les tokens dont il s'agit puis l'execute 
            TokenType token = identify_token(token_str);
            execute_token(token, params ? params : "");
            // Passer au token suivant
            token_str = strtok(NULL, " "); 
        }
    }

    fclose(file);
}

int main() {
    printf("Initialisation du canvas avec Tkinter...\n");
    initialize_canvas();

    //RECUPERE LE FICHIER ET EFFECTUE LE TRAITEMENT
    const char *filename = "../output/tokens.txt";
    read_file(filename);


    printf("Lancement de la boucle Tkinter...\n");
    execute_canvas_command("root.mainloop()");
    
    Py_Finalize();
    return 0;
}
