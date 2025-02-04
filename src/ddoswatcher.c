#include "../include/ddoswatcher.h"


void listInterfaces(GtkComboBox *combo_box) {
    pcap_if_t *alldevs, *dev;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Erreur lors de la récupération des interfaces : %s\n", errbuf);
        return;
    }

    for (dev = alldevs; dev != NULL; dev = dev->next) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), dev->name);
    }

    pcap_freealldevs(alldevs);
}

void packetHandler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    int *threshold = (int *)args;
    time_t current_time = time(NULL);

    if (difftime(current_time, start_time) >= DEFAULT_INTERVAL) {
        if (packet_count > *threshold) {
            char message[256];
            snprintf(message, sizeof(message), "ALERTE : Trafic suspect détecté ! (%d paquets reçus en %d secondes)", packet_count, DEFAULT_INTERVAL);
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "%s", message);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            for (int i = 0; i < ip_count; i++) {
                blockIp(inet_ntoa(*(struct in_addr *)&suspicious_ips[i]));
            }
        } else {
            char message[256];
            snprintf(message, sizeof(message), "Analyse terminée: %d paquets analysés en %d secondes.", packet_count, DEFAULT_INTERVAL);
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", message);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
        packet_count = 0;
        start_time = current_time;
        ip_count = 0;
    }
    struct ip *ip_header = (struct ip *)(packet + 14);
    if (ip_header->ip_p == IPPROTO_TCP) {
        struct in_addr src_ip = ip_header->ip_src;
        int found = 0;
        for (int i = 0; i < ip_count; i++) {
            if (suspicious_ips[i] == *(int *)&src_ip) {
                found = 1;
                break;
            }
        }
        if (!found && ip_count < 1024) {
            suspicious_ips[ip_count++] = *(int *)&src_ip;
        }
    }
    packet_count++;
}

void initWatcher(GtkWidget *widget, gpointer data) {
    DDosWatcher *appData = (DDosWatcher *)data;
    if (!gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(appData->interface)) ||
        atoi(gtk_entry_get_text(GTK_ENTRY(appData->port))) <= 0 ||
        atoi(gtk_entry_get_text(GTK_ENTRY(appData->seuil))) <= 0 ||
        atoi(gtk_entry_get_text(GTK_ENTRY(appData->time))) <= 0) {

        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Erreur : Veuillez entrer des valeurs valides !");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    pthread_t capture_thread;
    pthread_create(&capture_thread, NULL, startCapture, (void *)appData);
    pthread_detach(capture_thread);
}

void *startCapture(void *arg) {
    DDosWatcher *appData = (DDosWatcher *)arg;
    const char *device = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(appData->interface));
    int port = atoi(gtk_entry_get_text(GTK_ENTRY(appData->port)));
    int threshold = atoi(gtk_entry_get_text(GTK_ENTRY(appData->seuil)));
    int interval = atoi(gtk_entry_get_text(GTK_ENTRY(appData->time)));

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        gdk_threads_add_idle((GSourceFunc)gtk_message_dialog_new, "Erreur : Impossible d'ouvrir l'interface !");
        return NULL;
    }

    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Analyse du trafic en cours...");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    struct bpf_program filter;
    char filter_exp[50];
    snprintf(filter_exp, sizeof(filter_exp), "tcp port %d", port);

    if (pcap_compile(handle, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1 || pcap_setfilter(handle, &filter) == -1) {
        gdk_threads_add_idle((GSourceFunc)gtk_message_dialog_new, "Erreur : Filtrage des paquets échoué !");
        pcap_close(handle);
        return NULL;
    }

    start_time = time(NULL);
    pcap_loop(handle, 0, packetHandler, (u_char *)&threshold);

    pcap_freecode(&filter);
    pcap_close(handle);
    return NULL;
}

void blockIp(const char *ip) {
    char command[128];
    snprintf(command, sizeof(command), "iptables -A INPUT -s %s -j DROP", ip);
    system(command);

    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "IP bloquée : %s", ip);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void ddoswatcher(GtkWidget *widget, gpointer data) {
    const char *title = gtk_button_get_label(GTK_BUTTON(widget));
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

    GtkWidget *space1 = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space1, 0, 1, 2, 1);

    GtkWidget *label_interface = gtk_label_new("Port à surveiler :");
    gtk_grid_attach(GTK_GRID(grid), label_interface, 0, 2, 2, 1);
    
    GtkWidget *combo_interface = gtk_combo_box_text_new();
    gtk_grid_attach(GTK_GRID(grid), combo_interface, 0, 3, 2, 1);
    listInterfaces(GTK_COMBO_BOX(combo_interface));

    GtkWidget *label_port = gtk_label_new("Port à surveiler :");
    gtk_grid_attach(GTK_GRID(grid), label_port, 0, 4, 2, 1);

    GtkWidget *entry_port = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_port), "Ex: 80");
    gtk_widget_set_size_request(entry_port, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), entry_port, 0, 5, 2, 1);

    GtkWidget *label_seuil = gtk_label_new("Seuil d'alerte :");
    gtk_grid_attach(GTK_GRID(grid), label_seuil, 0, 6, 2, 1);

    GtkWidget *entry_seuil = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_seuil), "Ex: 100");
    gtk_widget_set_size_request(entry_seuil, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), entry_seuil, 0, 7, 2, 1);

    GtkWidget *label_time = gtk_label_new("Intervalle de temps en seconde :");
    gtk_grid_attach(GTK_GRID(grid), label_time, 0, 8, 2, 1);

    GtkWidget *entry_time = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_time), "Ex: 5");
    gtk_widget_set_size_request(entry_time, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), entry_time, 0, 9, 2, 1);

    GtkWidget *space2 = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space2, 0, 10, 2, 1);

    GtkWidget *button = gtk_button_new_with_label("Confirmer");
    gtk_widget_set_size_request(button, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 11, 2, 1);

    DDosWatcher *appData = g_new(DDosWatcher, 1);
    appData->interface = combo_interface;
    appData->port = entry_port;
    appData->seuil = entry_seuil;
    appData->time = entry_time;

    g_signal_connect(button, "clicked", G_CALLBACK(initWatcher), appData);

    gtk_widget_show_all(window);
}
