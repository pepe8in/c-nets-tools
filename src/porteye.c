#include "../include/porteye.h"


// bool verbose = true; 

void clearContainer(GtkWidget *container) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
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
    if (!widgets) return;

    GtkWidget *port_entry = widgets[0];
    GtkWidget *result_label = widgets[1];
    GtkWidget *ip_entry = widgets[2];

    if (!GTK_IS_ENTRY(ip_entry) || !GTK_IS_ENTRY(port_entry)) {
        gtk_label_set_text(GTK_LABEL(result_label), "Erreur : champ invalide !");
        return;
    }

    const char *ip = gtk_entry_get_text(GTK_ENTRY(ip_entry));
    const char *port_text = gtk_entry_get_text(GTK_ENTRY(port_entry));

    if (!ip || !port_text || strlen(port_text) == 0) {
        gtk_label_set_text(GTK_LABEL(result_label), "Veuillez entrer un IP et un port.");
        return;
    }

    int port = atoi(port_text);
    if (port <= 0 || port > MAX_PORTS) {
        gtk_label_set_text(GTK_LABEL(result_label), "Port invalide.");
        return;
    }

    int sock = createSocket(ip, port);
    char result_message[50];
    snprintf(result_message, sizeof(result_message), "Port %d : %s", port, (sock < 0 ? "fermé" : "ouvert"));
    gtk_label_set_text(GTK_LABEL(result_label), result_message);

    if (sock >= 0) close(sock);

    gtk_widget_show_all(gtk_widget_get_parent(port_entry));
}

void scanPortRange(GtkWidget *button, gpointer data) {
    GtkWidget **widgets = (GtkWidget **)data;
    if (!widgets) return;

    int start_port = -1, end_port = -1;
    GtkWidget *result_textview = NULL;
    GtkWidget *ip_entry = NULL;
    GtkWidget *start_port_entry = widgets[0];
    GtkWidget *end_port_entry = widgets[1];

    const char *start_str = gtk_entry_get_text(GTK_ENTRY(start_port_entry));
    const char *end_str   = gtk_entry_get_text(GTK_ENTRY(end_port_entry));

    start_port = atoi(start_str);
    end_port   = atoi(end_str);

    result_textview = widgets[2];
    ip_entry = widgets[3];

    const char *ip = gtk_entry_get_text(GTK_ENTRY(ip_entry));

    if (start_port < 0 || start_port > MAX_PORTS || end_port < 0 || end_port > MAX_PORTS) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_textview));
        gtk_text_buffer_set_text(buffer, "Plage de ports invalide.", -1);
        return;
    }

    if (start_port > end_port) {
        long int temp = start_port;
        start_port = end_port;
        end_port = temp;
    }
    ScanData *scan_data = g_malloc(sizeof(ScanData));

    strncpy(scan_data->ip, ip, sizeof(scan_data->ip) - 1);
    scan_data->ip[sizeof(scan_data->ip) - 1] = '\0'; 
    scan_data->start_port = start_port;
    scan_data->end_port = end_port;
    scan_data->result_textview = result_textview;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_textview));
    gtk_text_buffer_set_text(buffer, "Scan en cours...\n", -1);
    while (gtk_events_pending()) gtk_main_iteration();  

    g_thread_new("scan_thread", scanPortRangeThread, scan_data);
}

void scanPortOpenRange(GtkWidget *button, gpointer data) {
    GtkWidget **widgets = (GtkWidget **)data;
    if (!widgets) return;

    int start_port, end_port;
    GtkWidget *result_textview = widgets[2];
    GtkWidget *ip_entry = widgets[3];
    
    const char *start_str = gtk_entry_get_text(GTK_ENTRY(widgets[0]));
    const char *end_str = gtk_entry_get_text(GTK_ENTRY(widgets[1]));
    start_port = atoi(start_str);
    end_port = atoi(end_str);

    const char *ip = gtk_entry_get_text(GTK_ENTRY(ip_entry));
    
    if (start_port < 0 || start_port > MAX_PORTS || end_port < 0 || end_port > MAX_PORTS) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_textview));
        gtk_text_buffer_set_text(buffer, "Plage de ports invalide.", -1);
        return;
    }

    if (start_port > end_port) {
        int temp = start_port;
        start_port = end_port;
        end_port = temp;
    }

    ScanData *scan_data = g_malloc(sizeof(ScanData));

    strncpy(scan_data->ip, ip, sizeof(scan_data->ip) - 1);
    scan_data->ip[sizeof(scan_data->ip) - 1] = '\0';
    scan_data->start_port = start_port;
    scan_data->end_port = end_port;
    scan_data->result_textview = result_textview;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_textview));
    gtk_text_buffer_set_text(buffer, "Scan en cours...\n", -1);
    while (gtk_events_pending()) gtk_main_iteration();

    g_thread_new("scan_open_ports_thread", scanPortOpenRangeThread, scan_data);
}

void *scanPortRangeThread(void *arg) {
    ScanData *data = (ScanData *)arg;
    GString *results = g_string_new("");

    for (int port = data->start_port; port <= data->end_port; port++) {
        int sock = createSocket(data->ip, port);
        const char *etat = (sock < 0) ? "fermé" : "ouvert";
        g_string_append_printf(results, "Port %d : %s\n", port, etat);
        if (sock >= 0) close(sock);
    }

    g_object_set_data(G_OBJECT(data->result_textview), "scan_result", g_strdup(results->str));
    g_idle_add((GSourceFunc) updateTextView, data->result_textview);
    g_string_free(results, TRUE);
    g_free(data);
    return NULL;
}

void *scanPortOpenRangeThread(void *arg) {
    ScanData *data = (ScanData *)arg;
    GString *results = g_string_new("");
    gboolean port_open_found = FALSE;

    for (int port = data->start_port; port <= data->end_port; port++) {
        int sock = createSocket(data->ip, port);
        if (sock >= 0) {
            g_string_append_printf(results, "Port %d : ouvert\n", port);
            close(sock);
            port_open_found = TRUE;
        }
    }

    if (!port_open_found) {
        g_string_append(results, "Aucun port ouvert\n");
    }

    g_object_set_data(G_OBJECT(data->result_textview), "scan_result", g_strdup(results->str));
    g_idle_add((GSourceFunc)updateTextView, data->result_textview);
    g_string_free(results, TRUE);
    g_free(data);
    return NULL;
}

gboolean updateTextView(gpointer user_data) {
    GtkWidget *textview = GTK_WIDGET(user_data);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    const char *text = g_object_get_data(G_OBJECT(textview), "scan_result");

    if (text) {
        gtk_text_buffer_set_text(buffer, text, -1);
        g_free((gpointer)text);  
    }
    return FALSE;
}

void buttonClicked(GtkWidget *button, gpointer data) {
    AppData *appData = (AppData *)data;
    GtkWidget *grid = appData->grid;
    const char *label = NULL;
    GList *children = gtk_container_get_children(GTK_CONTAINER(appData->grid));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        if (GTK_IS_RADIO_BUTTON(iter->data) && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(iter->data))) {
            GtkWidget *child = gtk_bin_get_child(GTK_BIN(iter->data)); 
            const char *label = gtk_label_get_text(GTK_LABEL(child));
            const char *ip = appData->ip_cleaned;

            if (g_strcmp0(label, "Scan d'un seul port") == 0) {
                clearContainer(grid);

                char ip_display[50];  
                snprintf(ip_display, sizeof(ip_display), "Adresse IP : %s", ip); 
                GtkWidget *ip_label = gtk_label_new(ip_display);
                gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);
                GtkWidget *ip_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(ip_entry), ip); 

                GtkWidget *port_label = gtk_label_new("Entrez un port à scanner :");
                gtk_grid_attach(GTK_GRID(grid), port_label, 0, 2, 1, 1);

                GtkWidget *port_entry = gtk_entry_new();
                gtk_widget_set_size_request(port_entry, 300, 30);
                gtk_entry_set_placeholder_text(GTK_ENTRY(port_entry), "Ex: 80");
                gtk_grid_attach(GTK_GRID(grid), port_entry, 0, 3, 1, 1);

                GtkWidget *scan_button = gtk_button_new_with_label("Scanner");
                gtk_widget_set_size_request(scan_button, 300, 30);
                gtk_grid_attach(GTK_GRID(grid), scan_button, 0, 4, 1, 1);

                GtkWidget *result_label = gtk_label_new("");
                gtk_grid_attach(GTK_GRID(grid), result_label, 0, 5, 2, 1);

                GtkWidget **data_array = g_malloc(sizeof(GtkWidget *) * 3);
                data_array[0] = port_entry;
                data_array[1] = result_label;
                data_array[2] = ip_entry; 
                g_signal_connect(scan_button, "clicked", G_CALLBACK(scanPort), data_array);

                gtk_widget_show_all(grid);
            } else if (g_strcmp0(label, "Scan d'une plage de ports") == 0) {
                clearContainer(grid);

                char ip_display[50];  
                snprintf(ip_display, sizeof(ip_display), "Adresse IP : %s", ip); 
                GtkWidget *ip_label = gtk_label_new(ip_display);
                gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);
                GtkWidget *ip_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(ip_entry), ip); 

                GtkWidget *port_label = gtk_label_new("Entrez une plage de port à scanner :");
                gtk_grid_attach(GTK_GRID(grid), port_label, 0, 2, 2, 1);

                GtkWidget *start_port_entry = gtk_entry_new();
                gtk_entry_set_placeholder_text(GTK_ENTRY(start_port_entry), "Ex: 10");
                gtk_grid_attach(GTK_GRID(grid), start_port_entry, 0, 3, 1, 1);

                GtkWidget *end_port_entry = gtk_entry_new();
                gtk_entry_set_placeholder_text(GTK_ENTRY(end_port_entry), "Ex: 100");
                gtk_grid_attach(GTK_GRID(grid), end_port_entry, 1, 3, 1, 1);

                GtkWidget *scan_button = gtk_button_new_with_label("Scanner");
                gtk_widget_set_size_request(scan_button, 300, 30);
                gtk_grid_attach(GTK_GRID(grid), scan_button, 0, 4, 2, 1);

                GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
                gtk_widget_set_size_request(scrolled_window, 400, 200); 
                gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

                GtkWidget *result_textview = gtk_text_view_new();
                gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(result_textview), GTK_WRAP_WORD);
                gtk_text_view_set_editable(GTK_TEXT_VIEW(result_textview), FALSE); 
                gtk_container_add(GTK_CONTAINER(scrolled_window), result_textview);

                gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 5, 2, 1);

                GtkWidget **data_array = g_malloc(sizeof(GtkWidget *) * 4);
                data_array[0] = start_port_entry;
                data_array[1] = end_port_entry;
                data_array[2] = result_textview;
                data_array[3] = ip_entry; 
                g_signal_connect(scan_button, "clicked", G_CALLBACK(scanPortRange), data_array);

                gtk_widget_show_all(grid);
            } else if (g_strcmp0(label, "Scan de tous les ports") == 0) {
                clearContainer(grid);

                char ip_display[50];  
                snprintf(ip_display, sizeof(ip_display), "Adresse IP : %s", ip); 
                GtkWidget *ip_label = gtk_label_new(ip_display);
                gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);
                GtkWidget *ip_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(ip_entry), ip);

                GtkWidget *port_label = gtk_label_new("Scan de tous les ports :");
                gtk_grid_attach(GTK_GRID(grid), port_label, 0, 2, 2, 1);

                GtkWidget *start_port_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(start_port_entry), "0"); 

                GtkWidget *end_port_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(end_port_entry), "65535");

                GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
                gtk_widget_set_size_request(scrolled_window, 400, 200); 
                gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

                GtkWidget *result_textview = gtk_text_view_new();
                gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(result_textview), GTK_WRAP_WORD);
                gtk_text_view_set_editable(GTK_TEXT_VIEW(result_textview), FALSE); 
                gtk_container_add(GTK_CONTAINER(scrolled_window), result_textview);

                gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 3, 2, 1);
                
                GtkWidget **data_array = g_malloc(sizeof(GtkWidget *) * 4);
                data_array[0] = start_port_entry;
                data_array[1] = end_port_entry;
                data_array[2] = result_textview;
                data_array[3] = ip_entry; 

                scanPortRange(NULL, data_array);

                gtk_widget_show_all(grid);
            } else if (g_strcmp0(label, "Scan des ports well-known") == 0) {
                clearContainer(grid);

                char ip_display[50];
                snprintf(ip_display, sizeof(ip_display), "Adresse IP : %s", ip); 
                GtkWidget *ip_label = gtk_label_new(ip_display);
                gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);
                GtkWidget *ip_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(ip_entry), ip); 

                GtkWidget *port_label = gtk_label_new("Scan des ports \"well-know\" :");
                gtk_grid_attach(GTK_GRID(grid), port_label, 0, 2, 2, 1);

                GtkWidget *start_port_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(start_port_entry), "0"); 

                GtkWidget *end_port_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(end_port_entry), "1023");

                GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
                gtk_widget_set_size_request(scrolled_window, 400, 200); 
                gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

                GtkWidget *result_textview = gtk_text_view_new();
                gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(result_textview), GTK_WRAP_WORD);
                gtk_text_view_set_editable(GTK_TEXT_VIEW(result_textview), FALSE); 
                gtk_container_add(GTK_CONTAINER(scrolled_window), result_textview);

                gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 3, 2, 1);
                
                GtkWidget **data_array = g_malloc(sizeof(GtkWidget *) * 4);
                data_array[0] = start_port_entry;
                data_array[1] = end_port_entry;
                data_array[2] = result_textview;
                data_array[3] = ip_entry; 

                scanPortRange(NULL, data_array);

                gtk_widget_show_all(grid);
            } else if (g_strcmp0(label, "Scan des ports registered") == 0) {
                clearContainer(grid);

                char ip_display[50];  
                snprintf(ip_display, sizeof(ip_display), "Adresse IP : %s", ip); 
                GtkWidget *ip_label = gtk_label_new(ip_display);
                gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);
                GtkWidget *ip_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(ip_entry), ip); 

                GtkWidget *port_label = gtk_label_new("Scan des ports \"registered\" :");
                gtk_grid_attach(GTK_GRID(grid), port_label, 0, 2, 2, 1);

                GtkWidget *start_port_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(start_port_entry), "1024"); 

                GtkWidget *end_port_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(end_port_entry), "49151");

                GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
                gtk_widget_set_size_request(scrolled_window, 400, 200); 
                gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

                GtkWidget *result_textview = gtk_text_view_new();
                gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(result_textview), GTK_WRAP_WORD);
                gtk_text_view_set_editable(GTK_TEXT_VIEW(result_textview), FALSE); 
                gtk_container_add(GTK_CONTAINER(scrolled_window), result_textview);

                gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 3, 2, 1);
                
                GtkWidget **data_array = g_malloc(sizeof(GtkWidget *) * 4);
                data_array[0] = start_port_entry;
                data_array[1] = end_port_entry;
                data_array[2] = result_textview;
                data_array[3] = ip_entry; 

                scanPortRange(NULL, data_array);

                gtk_widget_show_all(grid);

            } else if (g_strcmp0(label, "Scan des ports dynamic/private") == 0) {
                clearContainer(grid);

                char ip_display[50];  
                snprintf(ip_display, sizeof(ip_display), "Adresse IP : %s", ip); 
                GtkWidget *ip_label = gtk_label_new(ip_display);
                gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);
                GtkWidget *ip_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(ip_entry), ip); 

                GtkWidget *port_label = gtk_label_new("Scan des ports \"dynamic/private\" :");
                gtk_grid_attach(GTK_GRID(grid), port_label, 0, 2, 2, 1);

                GtkWidget *start_port_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(start_port_entry), "49152"); 

                GtkWidget *end_port_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(end_port_entry), "65535");

                GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
                gtk_widget_set_size_request(scrolled_window, 400, 200); 
                gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

                GtkWidget *result_textview = gtk_text_view_new();
                gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(result_textview), GTK_WRAP_WORD);
                gtk_text_view_set_editable(GTK_TEXT_VIEW(result_textview), FALSE); 
                gtk_container_add(GTK_CONTAINER(scrolled_window), result_textview);

                gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 3, 2, 1);
                
                GtkWidget **data_array = g_malloc(sizeof(GtkWidget *) * 4);
                data_array[0] = start_port_entry;
                data_array[1] = end_port_entry;
                data_array[2] = result_textview;
                data_array[3] = ip_entry; 

                scanPortRange(NULL, data_array);

                gtk_widget_show_all(grid);

            } else if (g_strcmp0(label, "Afficher les ports ouverts d'une plage") == 0) {
                clearContainer(grid);

                char ip_display[50]; 
                snprintf(ip_display, sizeof(ip_display), "Adresse IP : %s", ip); 
                GtkWidget *ip_label = gtk_label_new(ip_display);
                gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);
                GtkWidget *ip_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(ip_entry), ip); 

                GtkWidget *port_label = gtk_label_new("Entrez une plage de port à scanner :");
                gtk_grid_attach(GTK_GRID(grid), port_label, 0, 2, 2, 1);

                GtkWidget *start_port_entry = gtk_entry_new();
                gtk_entry_set_placeholder_text(GTK_ENTRY(start_port_entry), "Ex: 10");
                gtk_grid_attach(GTK_GRID(grid), start_port_entry, 0, 3, 1, 1);

                GtkWidget *end_port_entry = gtk_entry_new();
                gtk_entry_set_placeholder_text(GTK_ENTRY(end_port_entry), "Ex: 100");
                gtk_grid_attach(GTK_GRID(grid), end_port_entry, 1, 3, 1, 1);

                GtkWidget *scan_button = gtk_button_new_with_label("Scanner");
                gtk_widget_set_size_request(scan_button, 300, 30);
                gtk_grid_attach(GTK_GRID(grid), scan_button, 0, 4, 2, 1);

                GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
                gtk_widget_set_size_request(scrolled_window, 400, 200); 
                gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

                GtkWidget *result_textview = gtk_text_view_new();
                gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(result_textview), GTK_WRAP_WORD);
                gtk_text_view_set_editable(GTK_TEXT_VIEW(result_textview), FALSE); 
                gtk_container_add(GTK_CONTAINER(scrolled_window), result_textview);

                gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 5, 2, 1);

                GtkWidget **data_array = g_malloc(sizeof(GtkWidget *) * 4);
                data_array[0] = start_port_entry;
                data_array[1] = end_port_entry;
                data_array[2] = result_textview;
                data_array[3] = ip_entry; 
                g_signal_connect(scan_button, "clicked", G_CALLBACK(scanPortOpenRange), data_array);

                gtk_widget_show_all(grid);
            }
            break;
        }
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

        clearContainer(box);

        GtkWidget *grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
        gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
        gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
        gtk_grid_attach(GTK_GRID(box), grid, 0, 0, 1, 1);

        char ip_display[50];  
        snprintf(ip_display, sizeof(ip_display), "Adresse IP : %s", ip_cleaned); 
        GtkWidget *ip_label = gtk_label_new(ip_display);
        gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);

        GtkWidget *space1 = gtk_label_new("");
        gtk_grid_attach(GTK_GRID(grid), space1, 0, 2, 2, 1);

        const char *options[] = {
            "Scan d'un seul port",
            "Scan d'une plage de ports",
            "Scan de tous les ports",
            "Scan des ports well-known",
            "Scan des ports registered",
            "Scan des ports dynamic/private",
            "Afficher les ports ouverts d'une plage"
        };

        GtkWidget *prev_radio = NULL;
        GtkWidget *radio_buttons[7];  

        for (int i = 0; i < 7; i++) {
            GtkWidget *radio;
            if (prev_radio == NULL) {
                radio = gtk_radio_button_new_with_label(NULL, options[i]);
            } else {
                radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(prev_radio), options[i]);
            }
            prev_radio = radio;
            radio_buttons[i] = radio;
            if (i == 6) {
                gtk_widget_set_halign(radio, GTK_ALIGN_CENTER);  
                gtk_grid_attach(GTK_GRID(grid), radio, 0, 3 + (i / 2), 2, 1);  
            } else {
                gtk_grid_attach(GTK_GRID(grid), radio, i % 2, 3 + (i / 2), 1, 1);
            }
        }

        GtkWidget *space2 = gtk_label_new("");
        gtk_grid_attach(GTK_GRID(grid), space2, 0, 7, 2, 1);

        GtkWidget *confirm_button = gtk_button_new_with_label("Confirmer");
        gtk_widget_set_size_request(confirm_button, 300, 30);
        gtk_widget_set_halign(confirm_button, GTK_ALIGN_CENTER);
        gtk_grid_attach(GTK_GRID(grid), confirm_button, 0, 8, 2, 1);

        AppData *appData = g_malloc(sizeof(AppData));
        appData->grid = grid;
        appData->entry = entry;
        appData->window = window;
        appData->ip_cleaned = g_strdup(ip_cleaned);

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
    
    GtkWidget *label = gtk_label_new("Entrez une adresse IPv4 à scanner :");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 2, 1);

    GtkWidget *space1 = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space1, 0, 2, 2, 1); 
    
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Ex: 192.168.1.1");
    gtk_entry_set_max_length(GTK_ENTRY(entry), 14);
    gtk_widget_set_size_request(entry, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 3, 2, 1);

    GtkWidget *button = gtk_button_new_with_label("Entrer");
    gtk_widget_set_size_request(button, 300, 30);
    g_signal_connect(button, "clicked", G_CALLBACK(checkIp), entry);
    g_signal_connect(entry, "activate", G_CALLBACK(checkIp), entry);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 4, 2, 1);
    
    gtk_widget_show_all(window);
}

