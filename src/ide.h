#ifndef IDE_H
#define IDE_H

#include <gtk/gtk.h>

// Callback functions for menu actions
void on_new_file(GtkWidget *widget, gpointer data);
void on_open_file(GtkWidget *widget, gpointer data);
void on_save_file(GtkWidget *widget, gpointer data);

// Menu bar setup functions
GtkWidget* create_menu();
void connect_menu_signals(GtkWidget *menubar, GtkTextBuffer *buffer);

// Setup functions for components
void setup_editor(GtkWidget *vbox, GtkWidget **textview, GtkTextBuffer **buffer);
void setup_console(GtkWidget *vbox, GtkWidget **console);

// Callbacks for lexer and parser
void on_run_lexer(GtkWidget *widget, gpointer data);
void on_run_parser(GtkWidget *widget, gpointer data);

void apply_syntax_highlighting(GtkTextBuffer *buffer);
void init_syntax_highlighting(GtkTextBuffer *buffer);

extern GtkWidget *console_log;  // Déclaration de console_log comme externe

void log_to_console(const char *message);

#endif // IDE_H