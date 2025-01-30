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

extern bool verbose;
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

#endif
