#include <gtk/gtk.h>
#include <string.h>

typedef struct {
    GtkWidget *entry;
} PorteyeData;

void on_button_clicked(GtkWidget *button, gpointer data) {
    const char *label = gtk_button_get_label(GTK_BUTTON(button));
    if (strcmp(label, "porteye") == 0) {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Porteye",
                                                        GTK_WINDOW(data),
                                                        GTK_DIALOG_MODAL,
                                                        (const gchar *)"Close",
                                                        GTK_RESPONSE_CLOSE,
                                                        NULL);
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        PorteyeData *porteye_data = g_new(PorteyeData, 1);

        GtkWidget *entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Saisissez l'adresse IP ici ...");
        gtk_box_pack_start(GTK_BOX(content_area), entry, TRUE, TRUE, 0);
        porteye_data->entry = entry;

        g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);

        gtk_widget_show_all(dialog);
    } else {
        g_print("Bouton cliqué : %s\n", label);
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "C-NETS T00LS");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    const char *button_labels[] = {
        "porteye", "packetsnoop", "filesecure",
        "ddoswatcher", "urlspy", "xmlbuilder"
    };

    for (int i = 0; i < 6; i++) {
        GtkWidget *button = gtk_button_new_with_label(button_labels[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), window);

        int row = i / 2;
        int col = i % 2;
        gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
    }

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

