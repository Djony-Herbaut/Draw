#include "ide.h"

// Callback for "New file" action
void on_new_file(GtkWidget *widget, gpointer data) {
    if (!data) {
        g_printerr("Error: Buffer is NULL in on_new_file.\n");
        return;
    }
    g_printerr("Debug: on_new_file called, buffer = %p\n", data);

    // Clear the text buffer
    GtkTextBuffer *buffer = (GtkTextBuffer *)data;
    gtk_text_buffer_set_text(buffer, "", -1);
    g_printerr("Debug: New file created successfully.\n");
}

// Callback for "Open file" action
void on_open_file(GtkWidget *widget, gpointer data) {
    if (!data) {
        g_printerr("Error: Buffer is NULL in on_open_file.\n");
        return;
    }

    GtkTextBuffer *buffer = (GtkTextBuffer *)data;
    g_printerr("Debug: buffer in on_open_file = %p\n", buffer);

    // Open file chooser dialog
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open a file",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    "Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    // If the user selects a file
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (!filename) {
            g_printerr("Error: No file selected.\n");
            gtk_widget_destroy(dialog);
            return;
        }
        g_printerr("Debug: Selected file name = %s\n", filename);

        // Load file contents
        char *content;
        gsize length;
        if (!g_file_get_contents(filename, &content, &length, NULL)) {
            g_printerr("Error: Cannot read file %s.\n", filename);
            g_free(filename);
            gtk_widget_destroy(dialog);
            return;
        }
        g_printerr("Debug: File content loaded (%lu characters).\n", (unsigned long)length);

        // Set the file contents to the buffer
        gtk_text_buffer_set_text(buffer, content, length);
        g_printerr("Debug: Content inserted into the buffer.\n");

        // Free allocated memory
        g_free(content);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

// Callback for "Save file" action
void on_save_file(GtkWidget *widget, gpointer data) {
    if (!data) {
        g_printerr("Error: Buffer is NULL in on_save_file.\n");
        return;
    }

    GtkTextBuffer *buffer = (GtkTextBuffer *)data;
    g_printerr("Debug: buffer in on_save_file = %p\n", buffer);

    // Open file chooser dialog for saving
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save a file",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    "Save", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (!filename) {
            g_printerr("Error: No file selected.\n");
            gtk_widget_destroy(dialog);
            return;
        }
        g_printerr("Debug: Selected file name = %s\n", filename);

        // Get text content from the buffer
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        if (!text) {
            g_printerr("Error: Cannot retrieve text from the buffer.\n");
            g_free(filename);
            gtk_widget_destroy(dialog);
            return;
        }
        g_printerr("Debug: Text to save (%s).\n", text);

        // Write the text content to the file
        if (!g_file_set_contents(filename, text, -1, NULL)) {
            g_printerr("Error: Cannot write to file %s.\n", filename);
        } else {
            g_printerr("Debug: File %s saved successfully.\n", filename);
        }

        // Free allocated memory
        g_free(filename);
        g_free(text);
    }
    gtk_widget_destroy(dialog);
}

// Function to create the menu bar with "File" options
GtkWidget* create_menu() {
    GtkWidget *menubar = gtk_menu_bar_new();
    GtkWidget *filemenu = gtk_menu_new();
    GtkWidget *fileitem = gtk_menu_item_new_with_label("Fichier");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileitem), filemenu);

    // Menu items: New, Open, Save
    GtkWidget *new_item = gtk_menu_item_new_with_label("Nouveau");
    GtkWidget *open_item = gtk_menu_item_new_with_label("Ouvrir");
    GtkWidget *save_item = gtk_menu_item_new_with_label("Sauvegarder");

    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), new_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileitem);

    // Store menu items for signal connection
    g_object_set_data(G_OBJECT(menubar), "new_item", new_item);
    g_object_set_data(G_OBJECT(menubar), "open_item", open_item);
    g_object_set_data(G_OBJECT(menubar), "save_item", save_item);

    return menubar;
}

// Function to connect menu signals to their callbacks
void connect_menu_signals(GtkWidget *menubar, GtkTextBuffer *buffer) {
    GtkWidget *new_item = g_object_get_data(G_OBJECT(menubar), "new_item");
    GtkWidget *open_item = g_object_get_data(G_OBJECT(menubar), "open_item");
    GtkWidget *save_item = g_object_get_data(G_OBJECT(menubar), "save_item");

    // Connect signals to their respective callbacks
    g_signal_connect(new_item, "activate", G_CALLBACK(on_new_file), buffer);
    g_signal_connect(open_item, "activate", G_CALLBACK(on_open_file), buffer);
    g_signal_connect(save_item, "activate", G_CALLBACK(on_save_file), buffer);
}

// Function to set up the text editor
void setup_editor(GtkWidget *vbox, GtkWidget **textview, GtkTextBuffer **buffer) {
    *textview = gtk_text_view_new();
    *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(*textview));
    GtkWidget *frame = gtk_frame_new("Éditeur de Texte");

    if (!*buffer) {
        g_printerr("Error: Failed to initialize buffer in setup_editor.\n");
        return;
    }

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), *textview);
    gtk_container_add(GTK_CONTAINER(frame), scroll);

    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
}

// Function to set up the console/log area
void setup_console(GtkWidget *vbox, GtkWidget **console) {
    *console = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(*console), FALSE);
    GtkWidget *frame = gtk_frame_new("Console / Logs");

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), *console);
    gtk_container_add(GTK_CONTAINER(frame), scroll);

    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
}

// Function to set up the drawing area
void setup_drawing_area(GtkWidget *vbox, GtkWidget **drawing_area) {
    *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(*drawing_area, 400, 300);
    GtkWidget *frame = gtk_frame_new("Zone Graphique");

    gtk_container_add(GTK_CONTAINER(frame), *drawing_area);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
}

// Main entry point of the program
int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *textview, *drawing_area, *console;
    GtkTextBuffer *buffer = NULL; // Buffer for the text editor
    GtkWidget *menubar;

    // Initialize GTK
    gtk_init(&argc, &argv);
    g_printerr("Debug: GTK initialized successfully.\n");

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "IDE draw++");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box container
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // 1. Create the menu bar (without signals)
    menubar = create_menu();
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    // 2. Set up the editor to initialize the buffer
    setup_editor(vbox, &textview, &buffer);

    // 3. Connect signals to the menu once buffer is ready
    connect_menu_signals(menubar, buffer);

    // 4. Set up the console/log area
    setup_console(vbox, &console);

    // 5. Set up the drawing area
    setup_drawing_area(vbox, &drawing_area);

    // Show the window and all child widgets
    g_printerr("Debug: Window displayed.\n");
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}
