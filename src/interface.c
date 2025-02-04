#include "../include/interface.h"


GtkWidget *open_windows[10] = {NULL};  // Définition

void destroyWindow(GtkWidget *widget, gpointer data) {
    for (int i = 0; i < 10; i++) {
        if (open_windows[i] == widget) {
            open_windows[i] = NULL;
            break;
        }
    }
    gtk_widget_destroy(widget);
}

