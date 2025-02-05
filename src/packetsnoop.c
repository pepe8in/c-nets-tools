#include "../include/packetsnoop.h"

gboolean etherType_idle(gpointer data);

void httpPacket(unsigned char* buffer, int size, int s, GtkWidget *widget) {
    GtkTextBuffer *buffer_text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer_text, &end);

    const char* protocol = (s == 443) ? "HTTPS" : "HTTP";
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct tcphdr *tcp_header = (struct tcphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    unsigned char* data = buffer + sizeof(struct ethhdr) + ip_header->ihl * 4 + tcp_header->doff * 4;
    int data_size = size - (data - buffer);

    if (data_size > 0) {
        char output[8192];
        int offset = snprintf(output, sizeof(output), "(Corps %s) Payload : %d bytes\n", protocol, data_size);
        offset += snprintf(output + offset, sizeof(output) - offset, "(Corps %s) Data (hex & ASCII) :\n", protocol);

        for (int i = 0; i < data_size; i++) {
            if (i != 0 && i % 16 == 0)
                offset += snprintf(output + offset, sizeof(output) - offset, "\n");

            offset += snprintf(output + offset, sizeof(output) - offset, "%02X ", data[i]);

            if (i % 16 == 15 || i == data_size - 1) {
                offset += snprintf(output + offset, sizeof(output) - offset, " | ");
                for (int j = i - (i % 16); j <= i; j++) {
                    offset += snprintf(output + offset, sizeof(output) - offset, "%c", isprint(data[j]) ? data[j] : '.');
                }
                offset += snprintf(output + offset, sizeof(output) - offset, "\n");
            }
        }

        gtk_text_buffer_insert(buffer_text, &end, output, -1);
    }
}
void icmpPacket(unsigned char* buffer, int size, GtkWidget *widget) {
    GtkTextBuffer *buffer_text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer_text, &end);

    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct icmphdr *icmp_header = (struct icmphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    gchar *text = text = g_strdup_printf(" (ICMP)\n");
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
    text = g_strdup_printf("(En-tête ICMP) Type                 : %d\n", icmp_header->type);
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
    text = g_strdup_printf("(En-tête ICMP) Code                 : %d\n", icmp_header->code);
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
}

void tcpPacket(unsigned char* buffer, int size, GtkWidget *widget) {
    GtkTextBuffer *buffer_text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer_text, &end);

    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct tcphdr *tcp_header = (struct tcphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    gchar *text = text = g_strdup_printf(" (TCP)\n");
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
    text = g_strdup_printf("(En-tête TCP) Port source           : %u\n", ntohs(tcp_header->source));
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
    text = g_strdup_printf("(En-tête TCP) Port destination      : %u\n", ntohs(tcp_header->dest));
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
    text = g_strdup_printf("(En-tête TCP) Flags                 : 0x%02X\n", tcp_header->fin|tcp_header->syn|tcp_header->rst|tcp_header->psh|tcp_header->ack|tcp_header->urg);
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
}

void udpPacket(unsigned char* buffer, int size, GtkWidget *widget) {
    GtkTextBuffer *buffer_text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer_text, &end);

    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct udphdr *udp_header = (struct udphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    gchar *text = text = g_strdup_printf(" (UDP)\n");
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
    text = g_strdup_printf("(En-tête UDP) Port source           : %u\n", ntohs(udp_header->source));    
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
    text = g_strdup_printf("(En-tête UDP) Port destination      : %u\n", ntohs(udp_header->dest));
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
}

void etherType(unsigned char* buffer, int size, GtkWidget *widget) {
    GtkTextBuffer *buffer_text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer_text, &end);


    struct ethhdr *eth_header = (struct ethhdr *)buffer;

    gchar *text = g_strdup_printf("Adresse MAC destination : %02X:%02X:%02X:%02X:%02X:%02X\n", eth_header->h_dest[0], eth_header->h_dest[1], eth_header->h_dest[2], eth_header->h_dest[3], eth_header->h_dest[4], eth_header->h_dest[5]);
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);

    text = g_strdup_printf("Adresse MAC source      : %02X:%02X:%02X:%02X:%02X:%02X\n", eth_header->h_source[0], eth_header->h_source[1], eth_header->h_source[2], eth_header->h_source[3], eth_header->h_source[4], eth_header->h_source[5]);
    gtk_text_buffer_insert(buffer_text, &end, text, -1);
    g_free(text);
    
    unsigned short proto = ntohs(eth_header->h_proto);
    switch (proto) {
        case ETH_P_IP: {
            struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
            struct sockaddr_in source_ipv4, dest_ipv4;
            source_ipv4.sin_addr.s_addr = ip_header->saddr;
            dest_ipv4.sin_addr.s_addr = ip_header->daddr;
            text = g_strdup_printf("(En-tête IPv4) Version              : %d\n", ip_header->version);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv4) Longueur d'en-tête   : %d octets\n", ip_header->ihl * 4);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv4) TTL                  : %d\n", ip_header->ttl);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv4) Adresse source       : %s\n", inet_ntoa(source_ipv4.sin_addr));
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv4) Adresse destination  : %s\n", inet_ntoa(dest_ipv4.sin_addr));
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv4) Protocole            : %d\n", ip_header->protocol);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
        break;
        }
        case ETH_P_IPV6: {
            struct ipv6hdr *ipv6_header = (struct ipv6hdr*)(buffer + sizeof(struct ethhdr));
            char source_ipv6[INET6_ADDRSTRLEN], dest_ipv6[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &ipv6_header->saddr, source_ipv6, sizeof(source_ipv6));
            inet_ntop(AF_INET6, &ipv6_header->daddr, dest_ipv6, sizeof(dest_ipv6));
            text = g_strdup_printf("(En-tête IPv6) Version              : 6\n");
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv6) Classe de trafic     : 0x%02X\n", (ipv6_header->priority));
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv6) Flow Label           : 0x%05X\n", ntohl(*(uint32_t *)ipv6_header) & 0x000FFFFF);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv6) Payload Length       : %d octets\n", ntohs(ipv6_header->payload_len));
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv6) Hop Limit            : %d\n", ipv6_header->hop_limit);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv6) Adresse source       : %s\n", source_ipv6);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv6) Adresse destination  : %s\n", dest_ipv6);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(En-tête IPv6) Next Header          : %d", ipv6_header->nexthdr);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
        break;
        }
        case ETH_P_ARP: {
            struct arphdr *arp_header = (struct arphdr*)(buffer + sizeof(struct ethhdr));
            unsigned char *sender_mac = buffer + sizeof(struct ethhdr) + sizeof(struct arphdr);
            unsigned char *sender_ip = sender_mac + 6;
            unsigned char *target_mac = sender_ip + 4;
            unsigned char *target_ip = target_mac + 6;
            text = g_strdup_printf("(Protocole ARP) Matériel            : %d\n", ntohs(arp_header->ar_hrd));
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(Protocole ARP) Protocole           : 0x%04x\n", ntohs(arp_header->ar_pro));
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(Protocole ARP) Adresse IP émetteur : %d.%d.%d.%d\n", sender_ip[0], sender_ip[1], sender_ip[2], sender_ip[3]);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(Protocole ARP) Adresse IP cible    : %d.%d.%d.%d\n", target_ip[0], target_ip[1], target_ip[2], target_ip[3]);
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
            text = g_strdup_printf("(Protocole ARP) Opération           : %s\n", (ntohs(arp_header->ar_op) == 1) ? "Requête" : "Réponse");
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
        break;
        }
        default:
            text = g_strdup("Protocole Ethernet non pris en charge\n");
            gtk_text_buffer_insert(buffer_text, &end, text, -1);
            g_free(text);
        break;
    }
}

void process(unsigned char* buffer, int size, GtkWidget *widget) {
    EtherTypeData *data = g_new(EtherTypeData, 1);
    data->buffer = g_strdup(buffer);
    data->widget = widget;
    gdk_threads_add_idle((GSourceFunc)etherType_idle, data);
}

gboolean etherType_idle(gpointer data) {
    EtherTypeData *ether_data = (EtherTypeData *)data;
    etherType(ether_data->buffer, BUFFER_SIZE, ether_data->widget);
    struct iphdr *ip_header = (struct iphdr*)(ether_data->buffer + sizeof(struct ethhdr));
    switch (ip_header->protocol) {
        case IPPROTO_ICMP:
            icmpPacket(ether_data->buffer, BUFFER_SIZE, ether_data->widget);
            break;
        case IPPROTO_TCP:
            tcpPacket(ether_data->buffer, BUFFER_SIZE, ether_data->widget);
            struct tcphdr *tcp_header = (struct tcphdr*)(ether_data->buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
            if (ntohs(tcp_header->source) == 443 || ntohs(tcp_header->dest) == 443) {
                httpPacket(ether_data->buffer, BUFFER_SIZE, 443, ether_data->widget);
            } else if (ntohs(tcp_header->source) == 80 || ntohs(tcp_header->dest) == 80) {
                httpPacket(ether_data->buffer, BUFFER_SIZE, 80, ether_data->widget);
            }
            break;
        case IPPROTO_UDP:
            udpPacket(ether_data->buffer, BUFFER_SIZE, ether_data->widget);
            break;
        default:
            GtkTextBuffer *buffer_text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ether_data->widget));
            GtkTextIter end;
            gtk_text_buffer_get_end_iter(buffer_text, &end);
            gtk_text_buffer_insert(buffer_text, &end, " --> informations du paquet non prises en charge.\n", -1);
            break;
    }
    g_free(ether_data->buffer);
    g_free(ether_data);
    return FALSE;
}
void *capture_packets(void *arg) {
    PacketSnoopData *data = (PacketSnoopData *)arg;
    GtkWidget *widget = data->widget;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);
    int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Erreur de création de la socket.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return NULL;
    }

    while (!data->stop) {
        socklen_t saddr_len = sizeof(saddr);
        int data_size = recvfrom(sock_raw, buffer, BUFFER_SIZE, 0, &saddr, &saddr_len);
        if (data_size < 0) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Erreur de réception des données.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            break;
        }
        process(buffer, data_size, widget);
    }

    close(sock_raw);
    free(buffer);
    return NULL;
}

void packetsnoop_confirm(GtkWidget *button, gpointer data) {
    PacketSnoopData *ps_data = g_new(PacketSnoopData, 1);
    ps_data->widget = GTK_WIDGET(data);
    ps_data->stop = FALSE;
    ps_data->thread = g_thread_new("packet-capture", capture_packets, ps_data);
    g_signal_connect_swapped(button, "destroy", G_CALLBACK(g_thread_join), ps_data->thread);
    g_signal_connect_swapped(button, "destroy", G_CALLBACK(g_free), ps_data);
}

void packetsnoop(GtkWidget *widget, gpointer data) {
    const char *title = gtk_button_get_label(GTK_BUTTON(widget));
    int button_index = GPOINTER_TO_INT(data);

    if (open_windows[button_index] != NULL) {
        gtk_window_present(GTK_WINDOW(open_windows[button_index]));
        return;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
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

    GtkWidget *space = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 2, 1); 

    GtkWidget *packetsnoop_button = gtk_button_new_with_label("Capture des paquets réseau");
    gtk_widget_set_size_request(packetsnoop_button, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), packetsnoop_button, 0, 1, 2, 1);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled_window, 600, 400);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkWidget *result_textview = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(result_textview), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(result_textview), FALSE); 
    gtk_container_add(GTK_CONTAINER(scrolled_window), result_textview);
    
    g_signal_connect(packetsnoop_button, "clicked", G_CALLBACK(packetsnoop_confirm), result_textview);

    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 3, 2, 1); 

    gtk_widget_show_all(window);
}
