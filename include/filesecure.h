#ifndef FILESECURE_H
#define FILESECURE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <gtk/gtk.h>
#include "interface.h"

#define ERR_FILE_OPEN -1
#define ERR_AES_KEY -2
#define AES_KEY_SIZE 256
#define BUFFER_SIZE 1024

typedef struct {
    GtkWidget *entry_source;
    GtkWidget *entry_dest;
    GtkWidget *entry_password;
    GtkWidget *radio_encrypt;
} FileSecure;

void deriveKeyAndIV(const char *password, unsigned char *key, unsigned char *iv, unsigned char *salt, int generate);
int encryptFile(const char *input_file, const char *output_file, const unsigned char *key, const unsigned char *iv, const unsigned char *salt);
int decryptFile(const char *input_file, const char *output_file, const char *password, unsigned char *key, unsigned char *iv);
void buttonClick(GtkWidget *widget, gpointer data);
void filesecure(GtkWidget *widget, gpointer data);

#endif 
