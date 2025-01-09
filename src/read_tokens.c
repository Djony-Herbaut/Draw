#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_LEXEME_LENGTH 100

typedef struct {
    char type[MAX_LEXEME_LENGTH]; // Type du tokenPython.h: No such file or directorygcc
    char lexeme[MAX_LEXEME_LENGTH]; // Lexème associé
    int line; // Ligne où le token est trouvé
    int col; // Colonne où le token commence
} ParsedToken;

/**
 * @brief Initialise Python et configure un environnement tkinter avec un canvas.
 */
void initialize_tkinter_canvas() {
    Py_Initialize(); // Initialiser l'interpréteur Python
    
    const char *init_script = 
        "import tkinter as tk\n"
        "root = tk.Tk()\n"
        "canvas = tk.Canvas(root, width=800, height=600, bg='white')\n"
        "canvas.pack()\n"
        "cursor_x, cursor_y = 400, 300\n"; // Position initiale du curseur
    PyRun_SimpleString(init_script); // Exécuter le script d'initialisation
}

/**
 * @brief Exécute des commandes tkinter basées sur le token.
 * 
 * @param token Le token actuellement traité.
 */
void execute_tkinter_command(const ParsedToken *token) {
    if (strcmp(token->type, "TOKEN_DRAWCREATE_CURSOR") == 0) {
        const char *script = 
            "canvas.create_oval(cursor_x-5, cursor_y-5, cursor_x+5, cursor_y+5, fill='red')\n";
        PyRun_SimpleString(script);
        printf("Commande exécutée : Créer un curseur de dessin.\n");
    } else if (strcmp(token->type, "TOKEN_DRAWSET_POS") == 0) {
        const char *script = 
            "cursor_x, cursor_y = 200, 150\n"; // Position statique
        PyRun_SimpleString(script);
        printf("Commande exécutée : Positionner le curseur de dessin.\n");
    } else if (strcmp(token->type, "TOKEN_DRAWGO") == 0) {
        const char *script = 
            "cursor_x += 50\n"
            "canvas.create_line(cursor_x-50, cursor_y, cursor_x, cursor_y, fill='black')\n";
        PyRun_SimpleString(script);
        printf("Commande exécutée : Faire avancer le curseur.\n");
    } else if (strcmp(token->type, "TOKEN_DRAWSHAPE") == 0) {
        const char *script = 
            "canvas.create_rectangle(300, 300, 400, 400, outline='blue', width=2)\n";
        PyRun_SimpleString(script);
        printf("Commande exécutée : Dessiner une forme.\n");
    } else if (strcmp(token->type, "TOKEN_DRAWCIRCLE") == 0) {
        const char *script = 
            "canvas.create_oval(200, 200, 300, 300, outline='green', width=2)\n";
        PyRun_SimpleString(script);
        printf("Commande exécutée : Dessiner un cercle.\n");
    } else if (strcmp(token->type, "TOKEN_DRAWPEN_SIZE") == 0) {
        const char *script = 
            "canvas.create_text(cursor_x, cursor_y, text='Pen size updated', fill='purple')\n";
        PyRun_SimpleString(script);
        printf("Commande exécutée : Changer la taille du crayon.\n");
    } else if (strcmp(token->type, "TOKEN_DRAWUPDATE") == 0) {
        printf("Commande exécutée : Mettre à jour le dessin.\n");
    } else {
        printf("Commande inconnue pour le type de token : %s.\n", token->type);
    }
}

/**
 * @brief Lit un fichier contenant des tokens et exécute les commandes tkinter correspondantes.
 * 
 * @param filepath Chemin du fichier à lire.
 */
void process_tokens_file(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    char line[MAX_LINE_LENGTH];
    ParsedToken token;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (sscanf(line, "%s %s %d %d", token.type, token.lexeme, &token.line, &token.col) == 4) {
            printf("Ligne %d, Colonne %d : ", token.line, token.col);
            execute_tkinter_command(&token);
        } else {
            fprintf(stderr, "Erreur : Ligne mal formatée : %s\n", line);
        }
    }

    fclose(file);
}

/**
 * @brief Ferme l'application tkinter.
 */
void finalize_tkinter() {
    const char *close_script = "root.mainloop()"; // Lancer la boucle principale tkinter
    PyRun_SimpleString(close_script);
    Py_Finalize(); // Fermer l'interpréteur Python
}

int main() {
    const char *filepath = "../output/tokens.txt"; // Chemin du fichier contenant les tokens
    
    // Initialisation de tkinter et du canvas
    printf("Initialisation de tkinter et du canvas.\n");
    initialize_tkinter_canvas();

    // Lecture et traitement des tokens
    printf("Lecture et traitement des tokens depuis : %s\n", filepath);
    process_tokens_file(filepath);

    // Finaliser tkinter
    printf("Lancement de l'interface tkinter.\n");
    finalize_tkinter();

    return 0;
}
