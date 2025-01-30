#include "../include/porteye.h"

bool verbose = true;

typedef struct {
    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *window;
} AppData;


void destroyWindow(GtkWidget *window, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    open_windows[index] = NULL;
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

int createSocket(const char *ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erreur de création de socket");
        return -1;
    }
    
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server.sin_addr);
    
    int result = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (result < 0) {
        close(sock);
        return result;
    }
    return sock;
}

void scanPort(GtkWidget *button, gpointer data) {
    GtkWidget **widgets = (GtkWidget **)data;
    GtkWidget *port_entry = widgets[0];
    GtkWidget *result_label = widgets[1];

    const char *ip = gtk_entry_get_text(GTK_ENTRY(widgets[2]));
    const char *port_text = gtk_entry_get_text(GTK_ENTRY(port_entry));
    int port = atoi(port_text);

    if (port <= 0 || port > 65535) {
        gtk_label_set_text(GTK_LABEL(result_label), "Port invalide. Entrez un nombre entre 1 et 65535.");
        return;
    }

    int sock = createSocket(ip, port);
    if (sock < 0) {
        gtk_label_set_text(GTK_LABEL(result_label), "Port fermé ou erreur.");
        return;
    }

    close(sock);
    gtk_label_set_text(GTK_LABEL(result_label), "Port ouvert !");
}

// Fonction appelée lors du clic sur "Confirmer"
void buttonClicked(GtkWidget *button, gpointer data) {
    AppData *appData = (AppData *)data;
    GtkWidget *grid = appData->grid;
    const char *label = NULL;
    // Vérification du bouton radio sélectionné
    GList *children = gtk_container_get_children(GTK_CONTAINER(appData->grid));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        if (GTK_IS_RADIO_BUTTON(iter->data) && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(iter->data))) {
            GtkWidget *child = gtk_bin_get_child(GTK_BIN(iter->data));  // 🔥 Récupère correctement le texte du bouton
            const char *label = gtk_label_get_text(GTK_LABEL(child));

            g_print("Label sélectionné : '%s'\n", label);  // Debug

            if (g_strcmp0(label, "Scan d'un seul port") == 0) {
                g_print("Entrée dans le if de 'Scan d'un seul port'\n");

            // Nettoyage de la grille
                clearContainer(grid);

            // Ajout des éléments pour le scan d'un port
                GtkWidget *port_label = gtk_label_new("Entrez un port à scanner :");
                gtk_grid_attach(GTK_GRID(grid), port_label, 0, 1, 2, 1);

                GtkWidget *port_entry = gtk_entry_new();
                gtk_entry_set_placeholder_text(GTK_ENTRY(port_entry), "Ex: 80");
                gtk_grid_attach(GTK_GRID(grid), port_entry, 0, 2, 2, 1);

                GtkWidget *scan_button = gtk_button_new_with_label("Scanner");
                gtk_grid_attach(GTK_GRID(grid), scan_button, 0, 3, 2, 1);

                GtkWidget *result_label = gtk_label_new("");
                gtk_grid_attach(GTK_GRID(grid), result_label, 0, 4, 2, 1);

                g_signal_connect(scan_button, "clicked", G_CALLBACK(scanPort), grid);
                gtk_widget_show_all(grid);
            }
            break;
        }
    }
    g_list_free(children);
}
// Fonction pour nettoyer un conteneur GTK
void clearContainer(GtkWidget *container) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
}

void checkIp(GtkWidget *button, gpointer data) {
    GtkWidget *entry = GTK_WIDGET(data);
    const char *ip = gtk_entry_get_text(GTK_ENTRY(entry));

    const char *REGEX_PATTERN = "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$";
    regex_t regex;

    if (regcomp(&regex, REGEX_PATTERN, REG_EXTENDED) != 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Erreur de compilation de la regex.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    if (regexec(&regex, ip, 0, NULL, 0) != 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Adresse IP invalide. Veuillez réessayer.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        const char *ip_cleaned = cleanString(ip);
        GtkWidget *window = gtk_widget_get_toplevel(entry);
        GtkWidget *box = gtk_widget_get_parent(entry);
        // Nettoyer le conteneur avant d'afficher les nouveaux éléments
        clearContainer(box);

        // Création d'une grille pour organiser les widgets
        GtkWidget *grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
        gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
        gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

        gtk_grid_attach(GTK_GRID(box), grid, 0, 0, 1, 1);

        // Afficher l'IP validée
        //
        GtkWidget *ip_label = gtk_label_new(ip_cleaned);
        gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);

        // Espacement
        GtkWidget *space1 = gtk_label_new("");
        gtk_grid_attach(GTK_GRID(grid), space1, 0, 2, 2, 1);

        // Liste des options
        const char *options[] = {
            "Scan d'un seul port",
            "Scan d'une plage de ports",
            "Scan de tous les ports",
            "Scan des ports well-known",
            "Scan des ports registered",
            "Scan des ports dynamic/private",
            "Afficher les ports ouverts d'une plage"
        };

        // Création dynamique des boutons radio
        GtkWidget *prev_radio = NULL;
        GtkWidget *radio_buttons[7];  // Stocker les boutons pour une éventuelle récupération de sélection

        for (int i = 0; i < 7; i++) {
            GtkWidget *radio;
            if (prev_radio == NULL) {
                radio = gtk_radio_button_new_with_label(NULL, options[i]);
            } else {
                radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(prev_radio), options[i]);
            }
            prev_radio = radio;
            radio_buttons[i] = radio;  // Stocker le bouton radio pour une éventuelle récupération de sélection
            gtk_grid_attach(GTK_GRID(grid), radio, i % 2, 3 + (i / 2), 1, 1);
        }
        // Espacement avant le bouton de confirmation
        GtkWidget *space2 = gtk_label_new("");
        gtk_grid_attach(GTK_GRID(grid), space2, 0, 7, 2, 1);

        // Bouton "Confirmer"
        GtkWidget *confirm_button = gtk_button_new_with_label("Confirmer");
        gtk_widget_set_halign(confirm_button, GTK_ALIGN_CENTER);
        gtk_grid_attach(GTK_GRID(grid), confirm_button, 0, 8, 2, 1);

        // Allocation et passage des données à la fonction de validation
        AppData *appData = g_malloc(sizeof(AppData));
        appData->grid = grid;
        appData->entry = entry;
        appData->window = window;

        // Ajout d'une connexion pour récupérer le choix sélectionné
        g_signal_connect(confirm_button, "clicked", G_CALLBACK(buttonClicked), appData);

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
    
    g_signal_connect(window, "destroy", G_CALLBACK(destroyWindow), GINT_TO_POINTER(button_index));
    
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

    GtkWidget *space1 = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space1, 0, 2, 2, 1); 
    
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "192.168.1.1");
    gtk_entry_set_max_length(GTK_ENTRY(entry), 14);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 3, 2, 1);
    
    GtkWidget *space2 = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space2, 0, 4, 2, 1); 

    GtkWidget *button = gtk_button_new_with_label("Entrer");
    g_signal_connect(button, "clicked", G_CALLBACK(checkIp), entry);
    g_signal_connect(entry, "activate", G_CALLBACK(checkIp), entry);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 5, 2, 1);
    
    gtk_widget_show_all(window);
}

