#ifndef PORTEYE_H
#define PORTEYE_H

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

typedef struct {
    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *window;
    char *ip_cleaned;
} AppData;

typedef struct {
    char ip[64];
    int start_port;
    int end_port;
    GtkWidget *result_textview;
} ScanData;

void destroyWindow(GtkWidget *widget, gpointer data);
void clearContainer(GtkWidget *widget);
char *cleanString(const char *str);
int createSocket(const char *ip, int port);
void scanPort(GtkWidget *widget, gpointer data);
void scanPortRange(GtkWidget *widget, gpointer data);
void scanPortOpenRange(GtkWidget *widget, gpointer data);
void *scanPortRangeThread(void *arg);
void *scanPortOpenRangeThread(void *arg);
gboolean updateTextView(gpointer data);
void buttonClicked(GtkWidget *widget, gpointer data);
void checkIp(GtkWidget *widget, gpointer data);
void porteye(GtkWidget *widget, gpointer data);

void test_checkIp();
void test_checkPort();
void test_scanPort();
void test_scanRange();
void test_scanOpenPort();

#endif
