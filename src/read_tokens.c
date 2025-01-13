#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include <stdbool.h>
#include "read_tokens.h"

#define MAX_LINE_LENGTH 256

// Gestion du contexte Python
PyObject *main_module = NULL;

// Fonction pour initialiser Turtle
void initialize_turtle() {
    // Initialiser l'interpréteur Python
    Py_Initialize();

    // Script Python pour initialiser Turtle
    const char *python_script =
        "import turtle\n"
        "from turtle import *\n"
        "screen = turtle.Screen()\n"
        "screen.setup(width=1280, height=720)\n"
        "pen = turtle.Turtle()\n"
        "pen.speed(0)\n";

    // Exécuter le script Python
    int result = PyRun_SimpleString(python_script);
    if (result != 0) {
        fprintf(stderr, "Erreur : Échec de l'exécution du script Python pour Turtle.\n");
        Py_Finalize();
        exit(EXIT_FAILURE);
    }

    // Obtenir le contexte global Python
    main_module = PyImport_AddModule("__main__");
    if (!main_module) {
        fprintf(stderr, "Erreur : Impossible de récupérer le module principal Python.\n");
        Py_Finalize();
        exit(EXIT_FAILURE);
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
    if (strcmp(token_str, "TOKEN_DRAWPENUP") == 0)              return TOKEN_DRAWPENUP;
    if (strcmp(token_str, "TOKEN_DRAWPENDOWN") == 0)            return TOKEN_DRAWPENDOWN;
    if (strcmp(token_str, "TOKEN_DRAWCLEAR_SCREEN") == 0)       return TOKEN_DRAWCLEAR_SCREEN;
    if (strcmp(token_str, "TOKEN_COMMA") == 0)                  return TOKEN_COMMA;
    if (strcmp(token_str, "TOKEN_SEMICOLON") == 0)              return TOKEN_SEMICOLON;
    if (strcmp(token_str, "TOKEN_EOF") == 0)                    return TOKEN_EOF;
    return -1;
}

// Fonction pour exécuter une commande Python Turtle
void execute_turtle_command(const char *command) {
    int result = PyRun_SimpleString(command);
    printf("Commande exécutée : %s\n", command);
    if (result != 0) {
        fprintf(stderr, "Erreur : Échec de l'exécution de la commande Turtle : %s\n", command);
    }
}

// Fonction pour exécuter un token
void execute_token(TokenType token, const char *params) {
    char command[256];
    switch (token) {
        case TOKEN_DRAWSET_POS: {
            int x, y;
            sscanf(params, "%d %d", &x, &y);
            snprintf(command, sizeof(command), "pen.penup(); pen.goto(%d, %d); pen.pendown()", x, y);
            execute_turtle_command(command);
            printf("Curseur déplacé à la position : (%d, %d)\n", x, y);
            break;
        }
        case TOKEN_DRAWGO: {
            int distance;
            sscanf(params, "%d", &distance);
            snprintf(command, sizeof(command), "pen.forward(%d)", distance);
            execute_turtle_command(command);
            printf("Avancé de %d unités\n", distance);
            break;
        }
        case TOKEN_DRAWSETX: {
            int x;
            sscanf(params, "%d", &x);
            snprintf(command, sizeof(command), "pen.setx(%d)", x);
            execute_turtle_command(command);
            printf("Position X définie à : %d\n", x);
            break;
        }
        case TOKEN_DRAWSETY: {
            int y;
            sscanf(params, "%d", &y);
            snprintf(command, sizeof(command), "pen.sety(%d)", y);
            execute_turtle_command(command);
            printf("Position Y définie à : %d\n", y);
            break;
        }
        case TOKEN_DRAWSHOW_CURSOR: {
            snprintf(command, sizeof(command), "pen.showturtle()");
            execute_turtle_command(command);
            printf("Curseur affiché\n");
            break;
        }
        case TOKEN_DRAWHIDE_CURSOR: {
            snprintf(command, sizeof(command), "pen.hideturtle()");
            execute_turtle_command(command);
            printf("Curseur caché\n");
            break;
        }
        case TOKEN_DRAWCURSOR_COLOR: {
            snprintf(command, sizeof(command), "pen.color('%s')", params);
            execute_turtle_command(command);
            printf("Couleur du curseur définie à : %s\n", params);
            break;
        }
        case TOKEN_DRAWPEN_SIZE: {
            int size;
            sscanf(params, "%d", &size);
            snprintf(command, sizeof(command), "pen.pensize(%d)", size);
            execute_turtle_command(command);
            printf("Taille du stylo définie à : %d\n", size);
            break;
        }
        case TOKEN_DRAWMOVE_FORWARD: {
            int distance;
            sscanf(params, "%d", &distance);
            snprintf(command, sizeof(command), "pen.forward(%d)", distance);
            execute_turtle_command(command);
            printf("Avancé de %d unités\n", distance);
            break;
        }
        case TOKEN_DRAWMOVE_BACKWARD: {
            int distance;
            sscanf(params, "%d", &distance);
            snprintf(command, sizeof(command), "pen.backward(%d)", distance);
            execute_turtle_command(command);
            printf("Reculé de %d unités\n", distance);
            break;
        }
        case TOKEN_DRAWPIVOT_LEFT: {
            int angle;
            sscanf(params, "%d", &angle);
            snprintf(command, sizeof(command), "pen.left(%d)", angle);
            execute_turtle_command(command);
            printf("Pivoté à gauche de %d degrés\n", angle);
            break;
        }
        case TOKEN_DRAWPIVOT_RIGHT: {
            int angle;
            sscanf(params, "%d", &angle);
            snprintf(command, sizeof(command), "pen.right(%d)", angle);
            execute_turtle_command(command);
            printf("Pivoté à droite de %d degrés\n", angle);
            break;
        }
        case TOKEN_DRAWCIRCLE: {
            int radius;
            sscanf(params, "%d", &radius);
            snprintf(command, sizeof(command), "pen.circle(%d)", radius);
            execute_turtle_command(command);
            printf("Cercle dessiné avec un rayon de %d\n", radius);
            break;
        }
         case TOKEN_DRAWPENUP: {
            snprintf(command, sizeof(command), "pen.penup()");
            execute_turtle_command(command);
            printf("Stylo levé.\n");
            break;
        }
        case TOKEN_DRAWPENDOWN: {
            snprintf(command, sizeof(command), "pen.pendown()");
            execute_turtle_command(command);
            printf("Stylo baissé.\n");
            break;
        }
        case TOKEN_DRAWCLEAR_SCREEN: {
            snprintf(command, sizeof(command), "pen.clear()");
            execute_turtle_command(command);
            printf("Écran effacé\n");
            break;
        }
        case TOKEN_COMMA:
        case TOKEN_SEMICOLON:
        case TOKEN_EOF:
            // Aucun effet direct
            printf("Token de syntaxe détecté\n");
            break;

        default:
            printf("Token inconnu ou non implémenté\n");
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
            // Identifier le token
            TokenType token = identify_token(token_str);
            if (token == -1) {
                fprintf(stderr, "Erreur : Token non reconnu : %s\n", token_str);
                token_str = strtok(NULL, " ");
                continue;
            }

            // Lire les paramètres après le token
            char *params = strtok(NULL, " ");
            if (!validate_params(token, params)) {
                fprintf(stderr, "Erreur : Paramètres invalides pour le token %s\n", token_str);
                continue;
            }

            // Exécuter le token avec ses paramètres
            execute_token(token, params ? params : "");

            // Passer au token suivant
            token_str = strtok(NULL, " ");
        }
    }

    fclose(file);
    printf("Lecture et exécution des commandes terminées.\n");
}

bool validate_params(TokenType token, const char *params) {
    if (params == NULL) {
        // Les tokens qui nécessitent des paramètres ne peuvent pas avoir des paramètres NULL
        switch (token) {
            case TOKEN_DRAWSET_POS:
            case TOKEN_DRAWGO:
            case TOKEN_DRAWSETX:
            case TOKEN_DRAWSETY:
            case TOKEN_DRAWCURSOR_COLOR:
            case TOKEN_DRAWPEN_SIZE:
            case TOKEN_DRAWMOVE_FORWARD:
            case TOKEN_DRAWMOVE_BACKWARD:
            case TOKEN_DRAWPIVOT_LEFT:
            case TOKEN_DRAWPIVOT_RIGHT:
            case TOKEN_DRAWCIRCLE:
            case TOKEN_DRAWDOT:
            case TOKEN_DRAWARC:
            case TOKEN_DRAWSHAPE:
            case TOKEN_DRAWT_SLEEP:
                printf("Erreur : le token %d nécessite des paramètres, mais aucun n'a été fourni.\n", token);
                return false;
            default:
                return true; // Les autres tokens n'ont pas besoin de paramètres
        }
    }

    // Validation des paramètres pour les tokens qui en ont besoin
    switch (token) {
        case TOKEN_DRAWSET_POS: {
            int x, y;
            if (sscanf(params, "%d %d", &x, &y) != 2) {
                printf("Erreur : le token TOKEN_DRAWSET_POS nécessite deux paramètres (x y).\n");
                return false;
            }
            break;
        }
        case TOKEN_DRAWGO:
        case TOKEN_DRAWSETX:
        case TOKEN_DRAWSETY:
        case TOKEN_DRAWPEN_SIZE:
        case TOKEN_DRAWMOVE_FORWARD:
        case TOKEN_DRAWMOVE_BACKWARD:
        case TOKEN_DRAWPIVOT_LEFT:
        case TOKEN_DRAWPIVOT_RIGHT:
        case TOKEN_DRAWCIRCLE:
        case TOKEN_DRAWT_SLEEP: {
            int value;
            if (sscanf(params, "%d", &value) != 1) {
                printf("Erreur : le token %d nécessite un paramètre entier.\n", token);
                return false;
            }
            break;
        }
        case TOKEN_DRAWCURSOR_COLOR: {
            // Accepte une chaîne comme paramètre
            if (strlen(params) == 0) {
                printf("Erreur : le token TOKEN_DRAWCURSOR_COLOR nécessite une chaîne comme paramètre.\n");
                return false;
            }
            break;
        }
        case TOKEN_DRAWARC:
        case TOKEN_DRAWSHAPE:
        case TOKEN_DRAWDOT:
            // Ajoutez des validations spécifiques si nécessaire
            break;

        default:
            // Aucun paramètre requis pour les autres tokens
            break;
    }

    return true;
}