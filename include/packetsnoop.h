#ifndef PACKETSNOOP_H
#define PACKETSNOOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <net/if.h>
#include <linux/ipv6.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <glib.h>
#include "interface.h"

#define BUFFER_SIZE 65536

typedef struct {
    GtkWidget *widget;
    gboolean stop;
    GThread *thread;
} PacketSnoopData;

typedef struct {
    unsigned char *buffer;
    GtkWidget *widget;
} EtherTypeData;

void httpPacket(unsigned char* buffer, int size, int s, GtkWidget *widget);
void icmpPacket(unsigned char* buffer, int size, GtkWidget *widget);
void tcpPacket(unsigned char* buffer, int size, GtkWidget *widget);
void udpPacket(unsigned char* buffer, int size, GtkWidget *widget);
void etherType(unsigned char* buffer, int size, GtkWidget *widget);
void process(unsigned char* buffer, int size, GtkWidget *widget);
gboolean etherType_idle(gpointer data);
void *capture_packets(void *arg);
void packetsnoop_confirm(GtkWidget *button, gpointer data);
void packetsnoop(GtkWidget *widget, gpointer data);

#endif
