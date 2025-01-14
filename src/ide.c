#include "ide.h"
#include "lexer.h" 
#include "read_tokens.h"
#include "parser.h"
#include <pthread.h>
#include "test_parser.c"

GtkWidget *console_log = NULL; // Widget global pour l'affichage de la console / des logs

Token global_tokens[MAX_TOKENS];
int global_num_tokens = 0;
char *global_string_tokens[MAX_TOKENS] = {NULL};

// Callback pour l'action "Nouveau fichier"
void on_new_file(GtkWidget *widget, gpointer data) {
    if (!data) {
        g_printerr("Erreur : Le buffer est NULL dans on_new_file.\n");
        return;
    }
    g_printerr("Debug : on_new_file appelé, buffer = %p\n", data);

    // Effacer le contenu du buffer
    GtkTextBuffer *buffer = (GtkTextBuffer *)data;
    gtk_text_buffer_set_text(buffer, "", -1);
    g_printerr("Debug : Nouveau fichier créé avec succès.\n");
}

// Callback pour l'action "Ouvrir un fichier"
void on_open_file(GtkWidget *widget, gpointer data) {
    if (!data) {
        g_printerr("Erreur : Le buffer est NULL dans on_open_file.\n");
        return;
    }

    GtkTextBuffer *buffer = (GtkTextBuffer *)data;
    g_printerr("Debug : buffer dans on_open_file = %p\n", buffer);

    // Ouvrir la boîte de dialogue pour choisir un fichier
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Ouvrir un fichier",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "Annuler", GTK_RESPONSE_CANCEL,
                                                    "Ouvrir", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    // Si l'utilisateur sélectionne un fichier
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (!filename) {
            g_printerr("Erreur : Aucun fichier sélectionné.\n");
            gtk_widget_destroy(dialog);
            return;
        }
        g_printerr("Debug : Nom du fichier sélectionné = %s\n", filename);

        // Charger le contenu du fichier
        char *content;
        gsize length;
        if (!g_file_get_contents(filename, &content, &length, NULL)) {
            g_printerr("Erreur : Impossible de lire le fichier %s.\n", filename);
            g_free(filename);
            gtk_widget_destroy(dialog);
            return;
        }
        g_printerr("Debug : Contenu du fichier chargé (%lu caractères).\n", (unsigned long)length);

        // Insérer le contenu du fichier dans le buffer
        gtk_text_buffer_set_text(buffer, content, length);
        g_printerr("Debug : Contenu inséré dans le buffer.\n");

        // Libérer la mémoire allouée
        g_free(content);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

// Callback pour l'action "Sauvegarder un fichier"
void on_save_file(GtkWidget *widget, gpointer data) {
    if (!data) {
        g_printerr("Erreur : Le buffer est NULL dans on_save_file.\n");
        return;
    }

    GtkTextBuffer *buffer = (GtkTextBuffer *)data;
    g_printerr("Debug : buffer dans on_save_file = %p\n", buffer);

    // Ouvrir la boîte de dialogue pour sauvegarder
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Sauvegarder un fichier",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "Annuler", GTK_RESPONSE_CANCEL,
                                                    "Sauvegarder", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (!filename) {
            g_printerr("Erreur : Aucun fichier sélectionné.\n");
            gtk_widget_destroy(dialog);
            return;
        }
        g_printerr("Debug : Nom du fichier sélectionné = %s\n", filename);

        // Récupérer le texte du buffer
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        if (!text) {
            g_printerr("Erreur : Impossible de récupérer le texte du buffer.\n");
            g_free(filename);
            gtk_widget_destroy(dialog);
            return;
        }
        g_printerr("Debug : Texte à sauvegarder (%s).\n", text);

        // Écrire le texte dans le fichier
        if (!g_file_set_contents(filename, text, -1, NULL)) {
            g_printerr("Erreur : Impossible d'écrire dans le fichier %s.\n", filename);
        } else {
            g_printerr("Debug : Fichier %s sauvegardé avec succès.\n", filename);
        }

        // Libérer la mémoire allouée
        g_free(filename);
        g_free(text);
    }
    gtk_widget_destroy(dialog);
}

// Fonction pour créer la barre de menu avec les options "Fichier"
GtkWidget* create_menu() {
    GtkWidget *menubar = gtk_menu_bar_new();

    // Éléments du menu
    GtkWidget *new_item = gtk_menu_item_new_with_label("Nouveau");
    GtkWidget *open_item = gtk_menu_item_new_with_label("Ouvrir");
    GtkWidget *save_item = gtk_menu_item_new_with_label("Sauvegarder");
    GtkWidget *lexer_item = gtk_menu_item_new_with_label("Exécuter Lexer");
    GtkWidget *parser_item = gtk_menu_item_new_with_label("Exécuter Parser");

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), new_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), open_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), lexer_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), parser_item);

    // Stocker les éléments du menu pour connecter les signaux
    g_object_set_data(G_OBJECT(menubar), "new_item", new_item);
    g_object_set_data(G_OBJECT(menubar), "open_item", open_item);
    g_object_set_data(G_OBJECT(menubar), "save_item", save_item);
    g_object_set_data(G_OBJECT(menubar), "lexer_item", lexer_item);
    g_object_set_data(G_OBJECT(menubar), "parser_item", parser_item);

    return menubar;
}

// Fonction pour connecter les signaux des menus à leurs callbacks
void connect_menu_signals(GtkWidget *menubar, GtkTextBuffer *buffer) {
    GtkWidget *new_item = g_object_get_data(G_OBJECT(menubar), "new_item");
    GtkWidget *open_item = g_object_get_data(G_OBJECT(menubar), "open_item");
    GtkWidget *save_item = g_object_get_data(G_OBJECT(menubar), "save_item");
    GtkWidget *lexer_item = g_object_get_data(G_OBJECT(menubar), "lexer_item");
    GtkWidget *parser_item = g_object_get_data(G_OBJECT(menubar), "parser_item");

    // Connecter les signaux existants
    g_signal_connect(new_item, "activate", G_CALLBACK(on_new_file), buffer);
    g_signal_connect(open_item, "activate", G_CALLBACK(on_open_file), buffer);
    g_signal_connect(save_item, "activate", G_CALLBACK(on_save_file), buffer);

    // Connecter les nouveaux signaux
    g_signal_connect(lexer_item, "activate", G_CALLBACK(on_run_lexer), buffer);
    g_signal_connect(parser_item, "activate", G_CALLBACK(on_run_parser), buffer);
}

// Fonction pour configurer l'éditeur de texte
void setup_editor(GtkWidget *vbox, GtkWidget **textview, GtkTextBuffer **buffer) {
    *textview = gtk_text_view_new();
    *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(*textview));
    GtkWidget *frame = gtk_frame_new("Éditeur de Texte");

    if (!*buffer) {
        g_printerr("Erreur : Échec de l'initialisation du buffer dans setup_editor.\n");
        return;
    }

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), *textview);
    gtk_container_add(GTK_CONTAINER(frame), scroll);

    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

    init_syntax_highlighting(*buffer);

    // Appliquer la coloration à chaque changement de texte
    g_signal_connect(*buffer, "changed", G_CALLBACK(apply_syntax_highlighting), NULL);
}

// Fonction pour configurer la zone console/logs
void setup_console(GtkWidget *vbox, GtkWidget **console) {
    *console = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(*console), FALSE);
    GtkWidget *frame = gtk_frame_new("Console / Logs");

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), *console);
    gtk_container_add(GTK_CONTAINER(frame), scroll);

    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

    console_log = *console;
}

void *run_tk_canvas(void *arg) {
    printf("Initialisation de Turtle avec Tkinter...\n");

    // Initialiser Turtle
    initialize_turtle();

    // Nom du fichier d'output contenant les commandes
    const char *filename = "../output/tokens.txt";

    // Utiliser read_file pour exécuter les commandes à partir du fichier
    printf("Lecture et exécution des commandes depuis le fichier : %s\n", filename);
    read_file(filename); 

    // Lancer la boucle principale de Turtle
    printf("Lancement de la fenêtre Turtle...\n");
    execute_turtle_command("screen.mainloop()");

    return NULL;
}

void on_run_lexer(GtkWidget *widget, gpointer data) {
    if (!data) {
        log_to_console("Erreur : le buffer est nul.");
        return;
    }

    GtkTextBuffer *buffer = (GtkTextBuffer *)data;
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Appeler le lexer pour générer les tokens
    global_num_tokens = tokenize(text, global_tokens);

    if (global_num_tokens <= 0) {
        log_to_console("Erreur : aucun token valide trouvé.");
        g_free(text);
        return;
    }

    // Afficher la sortie du lexer pour débogage
    printf("=== Sortie du lexer ===\n");
    for (int i = 0; i < global_num_tokens; i++) {
        printf("Token %d : Type = %d, Lexeme = \"%s\", Ligne = %d, Colonne = %d\n",
               i, global_tokens[i].type, global_tokens[i].lexeme, global_tokens[i].line, global_tokens[i].col);
    }

    log_to_console("Lexage réussi : liste des tokens générée.");
    g_free(text);
}

void on_run_parser(GtkWidget *widget, gpointer data) {
    if (global_num_tokens <= 0) {
        log_to_console("Erreur : aucune liste de tokens disponible. Exécutez le lexer d'abord.");
        return;
    }

    // Afficher les tokens pour débogage
    printf("=== Entrée du parser ===\n");
    for (int i = 0; i < global_num_tokens; i++) {
        printf("Token %d : Type = %d, Lexeme = \"%s\"\n",
               i, global_tokens[i].type, global_tokens[i].lexeme);
    }

    log_to_console("Analyse syntaxique en cours...");
    int index = 0;
    ASTNode *ast = parse_program(global_tokens, &index);

    if (ast == NULL) {
        log_to_console("Erreur de syntaxe : vérifiez votre code.");
        return;
    }

    log_to_console("Syntaxe valide : écriture des tokens nécessaires dans output/tokens.txt...");

    // Écrire les tokens dans le fichier
    write_tokens_to_file(global_tokens, global_num_tokens, "../output/tokens.txt");

    log_to_console("Écriture réussie et AST validé. Le dessin peut commencer.");

    // Lancer un thread pour exécuter les instructions depuis le fichier généré
    pthread_t canvas_thread;
    if (pthread_create(&canvas_thread, NULL, run_tk_canvas, "../output/tokens.txt") != 0) {
        log_to_console("Erreur : Impossible de lancer l'exécution du canvas.");
        free_ast(ast); // Libérer la mémoire en cas d'échec
        return;
    }

    pthread_detach(canvas_thread); // Détacher le thread pour éviter les fuites mémoire

    log_to_console("Exécution du canvas lancée avec succès.");

    // Libérer la mémoire de l'AST
    free_ast(ast);
}

// Applique la coloration syntaxique en utilisant les fonctions du lexer
void apply_syntax_highlighting(GtkTextBuffer *buffer) {
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    Token tokens[MAX_TOKENS];
    int num_tokens = tokenize(text, tokens);

    GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(buffer);
    GtkTextTag *keyword_tag = gtk_text_tag_table_lookup(tag_table, "keyword");
    GtkTextTag *symbol_tag = gtk_text_tag_table_lookup(tag_table, "symbol");

    for (int i = 0; i < num_tokens; i++) {
        GtkTextIter token_start, token_end;
        gtk_text_buffer_get_iter_at_line_offset(buffer, &token_start, tokens[i].line - 1, tokens[i].col - 1);
        gtk_text_buffer_get_iter_at_line_offset(buffer, &token_end, tokens[i].line - 1, tokens[i].col + strlen(tokens[i].lexeme) - 1);

        // Utilise is_keyword et is_symbol pour détecter la nature du token
        if (is_keyword(tokens[i].lexeme) != TOKEN_UNKNOWN) {
            gtk_text_buffer_apply_tag(buffer, keyword_tag, &token_start, &token_end);
        } else if (is_symbol(tokens[i].lexeme[0]) != TOKEN_UNKNOWN) {
            gtk_text_buffer_apply_tag(buffer, symbol_tag, &token_start, &token_end);
        }
    }
    g_free(text);
}

// Initialisation des tags pour la coloration syntaxique
void init_syntax_highlighting(GtkTextBuffer *buffer) {
    GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(buffer);

    GtkTextTag *keyword_tag = gtk_text_tag_new("keyword");
    g_object_set(keyword_tag, "foreground", "red", NULL);
    gtk_text_tag_table_add(tag_table, keyword_tag);

    GtkTextTag *symbol_tag = gtk_text_tag_new("symbol");
    g_object_set(symbol_tag, "foreground", "blue", NULL);
    gtk_text_tag_table_add(tag_table, symbol_tag);
}

// Fonction pour ajouter des erreurs à la console/logs
void log_to_console(const char *message) {
    if (console_log != NULL && GTK_IS_TEXT_VIEW(console_log)) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console_log));
        if (buffer != NULL) {
            GtkTextIter end;
            gtk_text_buffer_get_end_iter(buffer, &end);

            // Vérifie que le message est en UTF-8 avant de l'insérer
            if (g_utf8_validate(message, -1, NULL)) {
                gtk_text_buffer_insert(buffer, &end, message, -1);
                gtk_text_buffer_insert(buffer, &end, "\n", -1);
            } else {
                // Si la chaîne n'est pas valide, log une erreur générique
                gtk_text_buffer_insert(buffer, &end, "[Erreur UTF-8 détectée]\n", -1);
            }
        } else {
            g_warning("Impossible de récupérer le GtkTextBuffer associé à console_log.");
        }
    } else {
        g_warning("console_log est NULL ou n'est pas un GtkTextView.");
    }
}

// Point d'entrée principal du programme
int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *textview, *console;
    GtkTextBuffer *buffer = NULL; // Buffer pour l'éditeur de texte
    GtkWidget *menubar;

    // Initialisation de GTK
    gtk_init(&argc, &argv);
    g_printerr("Debug : GTK initialisé avec succès.\n");

    // Création de la fenêtre principale
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "IDE draw++");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Création d'un conteneur vertical
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // 1. Créer la barre de menu (sans signaux)
    menubar = create_menu();
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    // 2. Configurer l'éditeur pour initialiser le buffer
    setup_editor(vbox, &textview, &buffer);

    // 3. Connecter les signaux au menu une fois le buffer prêt
    connect_menu_signals(menubar, buffer);

    // 4. Configurer la zone console/logs
    setup_console(vbox, &console);

    // Afficher la fenêtre et tous les widgets enfants
    g_printerr("Debug : Fenêtre affichée.\n");
    gtk_widget_show_all(window);

    // Démarrer la boucle principale de GTK
    gtk_main();

    return 0;
}