#include "../include/filesecure.h"


void deriveKeyAndIV(const char *password, unsigned char *key, unsigned char *iv, unsigned char *salt, int generate) {
    if (generate) {
        if (!RAND_bytes(salt, 8)) {
            fprintf(stderr, "Erreur : Impossible de générer un sel aléatoire\n");
            exit(1);
        }
    }
    if (!PKCS5_PBKDF2_HMAC(password, strlen(password), salt, 8, 10000, EVP_sha256(), 32, key)) {
        fprintf(stderr, "Erreur : Échec de la dérivation de la clé\n");
        exit(1);
    }
    if (generate) {
        if (!RAND_bytes(iv, 16)) {
            fprintf(stderr, "Erreur : Échec de la génération d'un IV aléatoire\n");
            exit(1);
        }
    }
}

int encryptFile(const char *input_file, const char *output_file, const unsigned char *key, const unsigned char *iv, const unsigned char *salt) {
    FILE *input = fopen(input_file, "rb");
    if (input == NULL) {
        perror("Erreur : Impossible d'ouvrir le fichier d'entrée");
        return ERR_FILE_OPEN;
    }

    FILE *output = fopen(output_file, "wb");
    if (output == NULL) {
        perror("Erreur : Impossible d'ouvrir le fichier de sortie");
        fclose(input);
        return ERR_FILE_OPEN;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Erreur : Impossible de créer le contexte EVP\n");
        fclose(input);
        fclose(output);
        return ERR_AES_KEY;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        fprintf(stderr, "Erreur : Échec de l'initialisation du chiffrement\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(input);
        fclose(output);
        return ERR_AES_KEY;
    }

    fwrite(salt, 1, 8, output);
    fwrite(iv, 1, 16, output);

    unsigned char buffer_in[BUFFER_SIZE];
    unsigned char buffer_out[BUFFER_SIZE + EVP_CIPHER_block_size(EVP_aes_256_cbc())];
    int len, cipher_len;

    while ((len = fread(buffer_in, 1, BUFFER_SIZE, input)) > 0) {
        if (EVP_EncryptUpdate(ctx, buffer_out, &cipher_len, buffer_in, len) != 1) {
            fprintf(stderr, "Erreur : Échec du chiffrement\n");
            EVP_CIPHER_CTX_free(ctx);
            fclose(input);
            fclose(output);
            return ERR_AES_KEY;
        }
        fwrite(buffer_out, 1, cipher_len, output);
    }

    if (EVP_EncryptFinal_ex(ctx, buffer_out, &cipher_len) != 1) {
        fprintf(stderr, "Erreur : Échec de la finalisation du chiffrement\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(input);
        fclose(output);
        return ERR_AES_KEY;
    }
    fwrite(buffer_out, 1, cipher_len, output);

    EVP_CIPHER_CTX_free(ctx);
    fclose(input);
    fclose(output);
    return 0;
}
int decryptFile(const char *input_file, const char *output_file, const char *password, unsigned char *key, unsigned char *iv) {
    FILE *input = fopen(input_file, "rb");
    if (input == NULL) {
        perror("Erreur : Impossible d'ouvrir le fichier d'entrée");
        return ERR_FILE_OPEN;
    }

    FILE *output = fopen(output_file, "wb");
    if (output == NULL) {
        perror("Erreur : Impossible d'ouvrir le fichier de sortie");
        fclose(input);
        return ERR_FILE_OPEN;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Erreur : Impossible de créer le contexte EVP\n");
        fclose(input);
        fclose(output);
        return ERR_AES_KEY;
    }

    unsigned char salt[8];

    if (fread(salt, 1, 8, input) != 8) {
        fprintf(stderr, "Erreur : Impossible de lire le sel\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(input);
        fclose(output);
        return ERR_AES_KEY;
    }

    if (fread(iv, 1, 16, input) != 16) {
        fprintf(stderr, "Erreur : Impossible de lire l'IV\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(input);
        fclose(output);
        return ERR_AES_KEY;
    }

    deriveKeyAndIV(password, key, iv, salt, 0);

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        fprintf(stderr, "Erreur : Échec de l'initialisation du déchiffrement\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(input);
        fclose(output);
        return ERR_AES_KEY;
    }

    unsigned char buffer_in[BUFFER_SIZE];
    unsigned char buffer_out[BUFFER_SIZE + EVP_CIPHER_block_size(EVP_aes_256_cbc())];
    int len, plain_len;

    while ((len = fread(buffer_in, 1, BUFFER_SIZE, input)) > 0) {
        if (EVP_DecryptUpdate(ctx, buffer_out, &plain_len, buffer_in, len) != 1) {
            ERR_print_errors_fp(stderr);
            fprintf(stderr, "Erreur : Échec du déchiffrement\n");
            EVP_CIPHER_CTX_free(ctx);
            fclose(input);
            fclose(output);
            return ERR_AES_KEY;
        }
        fwrite(buffer_out, 1, plain_len, output);
    }

    if (EVP_DecryptFinal_ex(ctx, buffer_out, &plain_len) != 1) {
        ERR_print_errors_fp(stderr);  // Affiche les erreurs OpenSSL
        fprintf(stderr, "Erreur : Échec de la finalisation du déchiffrement\n");
        EVP_CIPHER_CTX_free(ctx);
        fclose(input);
        fclose(output);
        return ERR_AES_KEY;
    }
    fwrite(buffer_out, 1, plain_len, output);

    EVP_CIPHER_CTX_free(ctx);
    fclose(input);
    fclose(output);
    return 0;
}

void buttonClick(GtkWidget *widget, gpointer data) {
    AppData *appData = (AppData *)data;
    const char *input_file = gtk_entry_get_text(GTK_ENTRY(appData->entry_source));
    const char *output_file = gtk_entry_get_text(GTK_ENTRY(appData->entry_dest));
    const char *password = gtk_entry_get_text(GTK_ENTRY(appData->entry_password));
    gboolean encrypt = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(appData->radio_encrypt));

    unsigned char key[32], iv[16], salt[8];
    deriveKeyAndIV(password, key, iv, salt, 1);

    int result;
    if (encrypt) {
        result = encryptFile(input_file, output_file, key, iv, salt);
    } else {
        result = decryptFile(input_file, output_file, password, key, iv);
    }

    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, result == 0 ? GTK_MESSAGE_INFO : GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, result == 0 ? "Opération réussie !" : "Erreur lors du traitement.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    OPENSSL_cleanse(key, sizeof(key));
    OPENSSL_cleanse(iv, sizeof(iv));
}

void filesecure(GtkWidget *widget, gpointer data) {
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

    GtkWidget *radio_encrypt = gtk_radio_button_new_with_label(NULL, "Chiffrement");
    gtk_grid_attach(GTK_GRID(grid), radio_encrypt, 0, 3, 2, 1);

    GtkWidget *radio_decrypt = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_encrypt), "Déchiffrement");
    gtk_grid_attach(GTK_GRID(grid), radio_decrypt, 1, 3, 2, 1);

    GtkWidget *space2 = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space2, 0, 4, 2, 1);

    GtkWidget *label_source = gtk_label_new("Fichier source :");
    gtk_grid_attach(GTK_GRID(grid), label_source, 0, 5, 2, 1);

    GtkWidget *entry_source = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_source), "Chemin du fichier");
    gtk_widget_set_size_request(entry_source, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), entry_source, 0, 6, 2, 1);

    GtkWidget *label_dest = gtk_label_new("Fichier destination :");
    gtk_grid_attach(GTK_GRID(grid), label_dest, 0, 7, 2, 1);

    GtkWidget *entry_dest = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_dest), "Chemin du fichier");
    gtk_widget_set_size_request(entry_dest, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), entry_dest, 0, 8, 2, 1);

    GtkWidget *label_password = gtk_label_new("Mot de passe :");
    gtk_grid_attach(GTK_GRID(grid), label_password, 0, 9, 2, 1);

    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_widget_set_size_request(entry_password, 300, 30);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Mot de passe");
    gtk_grid_attach(GTK_GRID(grid), entry_password, 0, 10, 2, 1);

    GtkWidget *space3 = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space3, 0, 11, 2, 1);

    GtkWidget *button = gtk_button_new_with_label("Confirmer");
    gtk_widget_set_size_request(button, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 12, 2, 1);

    AppData *appData = g_new(AppData, 1);
    appData->entry_source = entry_source;
    appData->entry_dest = entry_dest;
    appData->entry_password = entry_password;
    appData->radio_encrypt = radio_encrypt;

    g_signal_connect(button, "clicked", G_CALLBACK(buttonClick), appData);

    gtk_widget_show_all(window);
}
