#include "../include/porteye.h"

static GtkWidget *open_windows[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

bool verbose = true;

void on_window_destroy(GtkWidget *window, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    open_windows[index] = NULL;
}

void on_confirm_button_clicked(GtkWidget *button, gpointer data) {
    GtkWidget *grid = GTK_WIDGET(data);
    GList *children = gtk_container_get_children(GTK_CONTAINER(grid));

    for (int i = 0; i < g_list_length(children); i++) {
        GtkWidget *child = GTK_WIDGET(g_list_nth_data(children, i));
        if (GTK_IS_TOGGLE_BUTTON(child)) {
            if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(child))) {
                switch (i) {
                    case 0:
                        printf("1\n");
                        // printf("Quel port scanner ? ");
                        // scanf("%d", &port);
                        // scanPort(ip, port);
                        break;
                    case 1:
                        printf("2\n");
                        // printf("Quelle plage scanner ? ");
                        // scanf("%d %d", &start_port, &end_port);
                        // scanRange(ip, start_port, end_port);
                        break;
                    case 2:
                        printf("3\n");
                        // start_port = 0;
                        // end_port = MAX_PORTS;
                        // scanRange(ip, start_port, end_port);
                        break;
                    case 3:
                        printf("4\n");
                        // start_port = 0;
                        // end_port = 1023;
                        // scanRange(ip, start_port, end_port);
                        break;
                    case 4:
                        printf("5\n");
                        // start_port = 1024;
                        // end_port = 49151;
                        // scanRange(ip, start_port, end_port);
                        break;
                    case 5:
                        printf("6\n");
                        // start_port = 49151;
                        // end_port = MAX_PORTS;
                        // scanRange(ip, start_port, end_port);
                        break;
                    case 6:
                        printf("7\n");
                        // printf("Quelle plage scanner ? ");
                        // scanf("%d %d", &start_port, &end_port);
                        // scanOpenPort(ip, start_port, end_port);
                        break;
                }
            }
        }
    }
    g_list_free(children);
}

char *cleanString(const char *str) {
    char *cleaned = g_strdup(str);
    char *p = cleaned;
    while (*p) {
        if (!g_ascii_isprint(*p) && *p != '\n' && *p != '\t') {
            *p = '\0';
            break;
        }
        p++;
    }
    g_strstrip(cleaned);
    return cleaned;
}

void checkIp(GtkWidget *button, gpointer data) {
    GtkWidget *entry = GTK_WIDGET(data);
    const char *ip = gtk_entry_get_text(GTK_ENTRY(entry));
    const char *REGEX_PATTERN = "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$";
    regex_t regex;

    if (regcomp(&regex, REGEX_PATTERN, REG_EXTENDED) != 0) {
        // fprintf(stderr, "Erreur de compilation de la regex.\n");
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Erreur de compilation de la regex.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    if (regexec(&regex, ip, 0, NULL, 0) != 0) {
        // fprintf(stderr, "Adresse IP invalide : %s\n", ip);
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Adresse IP invalide. Veuillez réessayer.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        // fprintf(stdout, "Adresse IP valide : %s\n", ip);
        const char *ip_cleaned = cleanString(ip);
        GtkWidget *window = gtk_widget_get_toplevel(entry);
        GtkWidget *box = gtk_widget_get_parent(entry);
        GList *children = gtk_container_get_children(GTK_CONTAINER(box));
        for (GList *iter = children; iter != NULL; iter = iter->next) {
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        }
        g_list_free(children);

        GtkWidget *grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
        gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
        gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
        gtk_grid_attach(GTK_GRID(box), grid, 0, 0, 1, 1);
        GtkWidget *ip_label = gtk_label_new(ip_cleaned);
        gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);
        GtkWidget *space1 = gtk_label_new("");
        gtk_grid_attach(GTK_GRID(grid), space1, 0, 2, 2, 1);
        GtkWidget *radio1 = gtk_radio_button_new_with_label(NULL, "Scan d'un seul port");
        gtk_grid_attach(GTK_GRID(grid), radio1, 0, 3, 1, 1);
        GtkWidget *radio2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), "Scan d'une plage de ports");
        gtk_grid_attach(GTK_GRID(grid), radio2, 1, 3, 1, 1);
        GtkWidget *radio3 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), "Scan de tous les ports");
        gtk_grid_attach(GTK_GRID(grid), radio3, 0, 4, 1, 1);
        GtkWidget *radio4 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), "Scan des ports well-known");
        gtk_grid_attach(GTK_GRID(grid), radio4, 1, 4, 1, 1); 
        GtkWidget *radio5 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), "Scan des ports registered");
        gtk_grid_attach(GTK_GRID(grid), radio5, 0, 5, 1, 1);
        GtkWidget *radio6 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), "Scan des ports dynamic/private");
        gtk_grid_attach(GTK_GRID(grid), radio6, 1, 5, 1, 1);
        GtkWidget *radio7 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), "Afficher les ports ouverts d'une plage");
        gtk_grid_attach(GTK_GRID(grid), radio7, 0, 6, 2, 1); 
        gtk_widget_set_halign(radio7, GTK_ALIGN_CENTER);
        GtkWidget *space2 = gtk_label_new("");
        gtk_grid_attach(GTK_GRID(grid), space2, 0, 7, 2, 1);
        GtkWidget *button = gtk_button_new_with_label("Confirmer");
        gtk_grid_attach(GTK_GRID(grid), button, 0, 8, 2, 1);
        g_signal_connect(button, "clicked", G_CALLBACK(on_confirm_button_clicked), grid);

        gtk_widget_show_all(window);
    }
    regfree(&regex);
}

void porteye(GtkWidget *porteye, gpointer data) {
    const char *title = gtk_button_get_label(GTK_BUTTON(porteye));
    int button_index = GPOINTER_TO_INT(data);

    if (open_windows[button_index] != NULL) {
        gtk_window_present(GTK_WINDOW(open_windows[button_index]));
        return;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200); 
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    open_windows[button_index] = window;
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), GINT_TO_POINTER(button_index));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), box);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 10);
    GtkWidget *label = gtk_label_new("Entrez une adresse IP à scanner :");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 2, 1);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    GtkWidget *space = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space, 0, 2, 2, 1); 
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "192.168.1.1");
    gtk_entry_set_max_length(GTK_ENTRY(entry), 14);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 3, 2, 1); 
    GtkWidget *button = gtk_button_new_with_label("Entrer");
    g_signal_connect(button, "clicked", G_CALLBACK(checkIp), entry);
    g_signal_connect(entry, "activate", G_CALLBACK(checkIp), entry);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 4, 2, 1);

    gtk_widget_show_all(window);
}
