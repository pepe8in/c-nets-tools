#include "../include/porteye.h"
#include "../include/packetsnoop.h"
#include "../include/filesecure.h"
#include "../include/ddoswatcher.h"
#include "../include/xmlbuilder.h"


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "C-NETS T00LS");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box), grid, FALSE, FALSE, 10);

    const char *button_labels[] = { "porteye", "packetsnoop", "filesecure", "ddoswatcher", "urlspy", "xmlbuilder" };

    for (int i = 0; i < 6; i++) {
        GtkWidget *button = gtk_button_new_with_label(button_labels[i]);
        switch (i) {
            case 0 :
                g_signal_connect(button, "clicked", G_CALLBACK(porteye), GINT_TO_POINTER(i));
                break;
            case 1 :
                g_signal_connect(button, "clicked", G_CALLBACK(packetsnoop), GINT_TO_POINTER(i));
                break;
            case 2 :
                g_signal_connect(button, "clicked", G_CALLBACK(filesecure), GINT_TO_POINTER(i));
                break;
            case 3 :
                g_signal_connect(button, "clicked", G_CALLBACK(ddoswatcher), GINT_TO_POINTER(i));
                break;
            case 4 :
                break;
            case 5 :
                g_signal_connect(button, "clicked", G_CALLBACK(xmlbuilder), GINT_TO_POINTER(i));
                break;
            default:
        }
        gtk_widget_set_size_request(button, 300, 40);
        gtk_grid_attach(GTK_GRID(grid), button, i % 2, i / 2, 1, 1);
    }

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
