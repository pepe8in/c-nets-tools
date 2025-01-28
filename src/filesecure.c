#include "../include/filesecure.h"

void deriveKeyAndIV(const char *password, unsigned char *key, unsigned char *iv, unsigned char *salt) {
    if (!RAND_bytes(salt, 8)) {
        fprintf(stderr, "Erreur : Impossible de générer un sel aléatoire\n");
        exit(1);
    }

    if (!PKCS5_PBKDF2_HMAC(password, strlen(password), salt, 8, 10000, EVP_sha256(), 32, key)) {
        fprintf(stderr, "Erreur : Échec de la dérivation de la clé\n");
        exit(1);
    }

    if (!RAND_bytes(iv, 16)) {
        fprintf(stderr, "Erreur : Échec de la génération d'un IV aléatoire\n");
        exit(1);
    }
}

int encryptFile(const char *input_file, const char *output_file, const unsigned char *key, const unsigned char *iv) {
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
    unsigned char salt[8];
    fwrite(salt, 1, sizeof(salt), output);
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

int decryptFile(const char *input_file, const char *output_file, const unsigned char *key, unsigned char *iv) {
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
    fread(salt, 1, sizeof(salt), input);
    fread(iv, 1, 16, input);
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
            fprintf(stderr, "Erreur : Échec du déchiffrement\n");
            EVP_CIPHER_CTX_free(ctx);
            fclose(input);
            fclose(output);
            return ERR_AES_KEY;
        }
        fwrite(buffer_out, 1, plain_len, output);
    }

    if (EVP_DecryptFinal_ex(ctx, buffer_out, &plain_len) != 1) {
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

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s [encrypt/decrypt] [fichier_entree] [fichier_sortie]\n", argv[0]);
        return 1;
    }

    const char *operation = argv[1];
    const char *input_file = argv[2];
    const char *output_file = argv[3];
    unsigned char key[32];
    unsigned char iv[16];
    unsigned char salt[8];

    char password[64];
    printf("Entrez un mot de passe (64caractères max) : ");
    scanf("%63s", password);
    deriveKeyAndIV(password, key, iv, salt);

    if (strcmp(operation, "encrypt") == 0) {
        printf("Chiffrement du fichier : %s\n", input_file);
        int result = encryptFile(input_file, output_file, key, iv);
        if (result != 0) {
            printf("Erreur : Échec du chiffrement du fichier. Code d'erreur : %d\n", result);
            return result;
        }
        printf("Fichier chiffré avec succès : %s\n", output_file);
    } else if (strcmp(operation, "decrypt") == 0) {
        printf("Déchiffrement du fichier : %s\n", input_file);
        int result = decryptFile(input_file, output_file, key, iv);
        if (result != 0) {
            printf("Erreur : Échec du déchiffrement du fichier. Code d'erreur : %d\n", result);
            return result;
        }
        printf("Fichier déchiffré avec succès : %s\n", output_file);
    } else {
        printf("Opération invalide : %s. Utilisez 'encrypt' ou 'decrypt'.\n", operation);
        return 1;
    }

    OPENSSL_cleanse(key, sizeof(key));
    OPENSSL_cleanse(iv, sizeof(iv));
    return 0;
}

