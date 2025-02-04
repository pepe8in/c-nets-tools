#ifndef DDOSWATCHER_H
#define DDOSWATCHER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "interface.h"

#define DEFAULT_THRESHOLD 100
#define DEFAULT_INTERVAL 5
#define DEFAULT_PORT 80

typedef struct {
    GtkWidget *interface;
    GtkWidget *port;
    GtkWidget *seuil;
    GtkWidget *time;
} DDosWatcher;

void listInterfaces(GtkComboBox *combo_box);
void initWatcher(GtkWidget *widget, gpointer data);
void packetHandler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void *startCapture(void *arg);
void blockIp(const char *ip);
void ddoswatcher(GtkWidget *widget, gpointer data);

static int packet_count = 0;
static time_t start_time;
static int suspicious_ips[1024];
static int ip_count = 0; 

#endif
