#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <regex.h>
#include <glib.h>
#include <gtk/gtk.h>

#define TIMEOUT 1
#define MAX_PORTS 65535

// extern bool verbose;
static GtkWidget *open_windows[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

void destroyWindow(GtkWidget *widget, gpointer data);
void clearContainer(GtkWidget *widget);
void buttonClicked(GtkWidget *widget, gpointer data);
void checkIp(GtkWidget *widget, gpointer data);
void porteye(GtkWidget *widget, gpointer data);
void scanPort(GtkWidget *widget, gpointer data);
int createSocket(const char *ip, int port); 
int checkPort(const char *ip, int port);
int scanRange(const char *ip, int start_port, int end_port);
int scanOpenPort(const char *ip, int start_port, int end_port);
void test_checkIp();
void test_checkPort();
void test_scanPort();
void test_scanRange();
void test_scanOpenPort();

typedef struct {
    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *window;
    char *ip_cleaned;
} AppData;

// bool verbose = true;

void destroyWindow(GtkWidget *window, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    open_windows[index] = NULL;
}
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
    snprintf(result_message, sizeof(result_message), "%d : %s", port, (sock < 0 ? "Port fermé" : "Port ouvert"));
    gtk_label_set_text(GTK_LABEL(result_label), result_message);

    // gtk_label_set_text(GTK_LABEL(result_label), sock < 0 ? "Port fermé" : "Port ouvert");
    if (sock >= 0) close(sock);

    gtk_widget_show_all(gtk_widget_get_parent(port_entry));
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

                GtkWidget *ip_label = gtk_label_new(ip);
                gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);
                GtkWidget *ip_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(ip_entry), ip); // Préremplir l'IP
                // gtk_grid_attach(GTK_GRID(grid), ip_entry, 0, 1, 2, 1);

                GtkWidget *port_label = gtk_label_new("Entrez un port à scanner :");
                gtk_grid_attach(GTK_GRID(grid), port_label, 0, 2, 1, 1);

                GtkWidget *port_entry = gtk_entry_new();
                gtk_entry_set_placeholder_text(GTK_ENTRY(port_entry), "Ex: 80");
                gtk_grid_attach(GTK_GRID(grid), port_entry, 0, 3, 1, 1);

                GtkWidget *scan_button = gtk_button_new_with_label("Scanner");
                gtk_grid_attach(GTK_GRID(grid), scan_button, 0, 4, 1, 1);

                GtkWidget *result_label = gtk_label_new("");
                gtk_grid_attach(GTK_GRID(grid), result_label, 0, 5, 2, 1);

                GtkWidget **data_array = g_malloc(sizeof(GtkWidget *) * 3);
                data_array[0] = port_entry;
                data_array[1] = result_label;
                data_array[2] = ip_entry; // Assurez-vous que c'est bien un GtkEntry
                g_signal_connect(scan_button, "clicked", G_CALLBACK(scanPort), data_array);

                gtk_widget_show_all(grid);
            } else if (g_strcmp0(label, "Scan d'une plage de ports") == 0) {
                clearContainer(grid);

                GtkWidget *ip_label = gtk_label_new(ip);
                gtk_grid_attach(GTK_GRID(grid), ip_label, 0, 1, 2, 1);
                GtkWidget *ip_entry = gtk_entry_new();
                gtk_entry_set_text(GTK_ENTRY(ip_entry), ip); // Préremplir l'IP
                // gtk_grid_attach(GTK_GRID(grid), ip_entry, 0, 1, 2, 1);

                GtkWidget *port_label = gtk_label_new("Entrez une plage de port à scanner :");
                gtk_grid_attach(GTK_GRID(grid), port_label, 0, 2, 2, 1);

                GtkWidget *start_port_entry = gtk_entry_new();
                gtk_entry_set_placeholder_text(GTK_ENTRY(start_port_entry), "Ex: 10");
                gtk_grid_attach(GTK_GRID(grid), start_port_entry, 0, 3, 1, 1);

                GtkWidget *end_port_entry = gtk_entry_new();
                gtk_entry_set_placeholder_text(GTK_ENTRY(end_port_entry), "Ex: 100");
                gtk_grid_attach(GTK_GRID(grid), end_port_entry, 1, 3, 1, 1);

                GtkWidget *scan_button = gtk_button_new_with_label("Scanner");
                gtk_grid_attach(GTK_GRID(grid), scan_button, 0, 4, 2, 1);

                GtkWidget *result_label = gtk_label_new("");
                gtk_grid_attach(GTK_GRID(grid), result_label, 0, 5, 2, 1);

                GtkWidget **data_array = g_malloc(sizeof(GtkWidget *) * 4);
                data_array[0] = start_port_entry;
                data_array[1] = end_port_entry;
                data_array[2] = result_label;
                data_array[3] = ip_entry; 
                g_signal_connect(scan_button, "clicked", G_CALLBACK(scanPort), data_array);

                gtk_widget_show_all(grid);
            } else if (g_strcmp0(label, "Scan de tous les ports") == 0) {
            } else if (g_strcmp0(label, "Scan des ports well-known") == 0) {
            } else if (g_strcmp0(label, "Scan des ports registered") == 0) {
            } else if (g_strcmp0(label, "Scan des ports dynamic/private") == 0) {
            } else if (g_strcmp0(label, "Afficher les ports ouverts d'une plage") == 0) {
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

        GtkWidget *ip_label = gtk_label_new(ip_cleaned);
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
    
    //GtkWidget *space2 = gtk_label_new("");
    //gtk_grid_attach(GTK_GRID(grid), space2, 0, 4, 2, 1); 

    GtkWidget *button = gtk_button_new_with_label("Entrer");
    gtk_widget_set_size_request(button, 300, 30);
    g_signal_connect(button, "clicked", G_CALLBACK(checkIp), entry);
    g_signal_connect(entry, "activate", G_CALLBACK(checkIp), entry);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 4, 2, 1);
    
    gtk_widget_show_all(window);
}
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
                g_signal_connect(button, "clicked", G_CALLBACK(porteye), GINT_TO_POINTER(i));
                break;
            case 2 :
                g_signal_connect(button, "clicked", G_CALLBACK(porteye), GINT_TO_POINTER(i));
                break;
            case 3 :
                g_signal_connect(button, "clicked", G_CALLBACK(porteye), GINT_TO_POINTER(i));
                break;
            case 4 :
                g_signal_connect(button, "clicked", G_CALLBACK(porteye), GINT_TO_POINTER(i));
                break;
            case 5 :
                g_signal_connect(button, "clicked", G_CALLBACK(porteye), GINT_TO_POINTER(i));
                break;
            default:
        }
        gtk_widget_set_size_request(button, 300, 40);
        int row = i / 2; 
        int col = i % 2;
        gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
    }

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
