#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

extern GtkWidget *open_windows[10];

void destroyWindow(GtkWidget *widget, gpointer data);

#endif
