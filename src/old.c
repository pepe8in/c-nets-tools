// Utilisation :
// ./filesecure encrypt input.txt output.enc
// ./filesecure decrypt output.enc decrypted.txt
#ifndef FILE_SECURE_H
#define FILE_SECURE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>

#define ERR_FILE_OPEN -1
#define ERR_AES_KEY -2

/*
 * @brief Encrypts a file using AES in the specified mode.
 * @param input_file Path to the input file.
 * @param output_file Path to the output (encrypted) file.
 * @param key Encryption key (256 bits).
 * @param iv Initialization vector (16 bytes, used in CBC mode).
 * @return int Returns 0 on success, or an error code on failure.
 */
int encryptFile(const char *input_file, const char *output_file, const unsigned char *key, const unsigned char *iv);

/*
 * @brief Decrypts a file using AES in the specified mode.
 * @param input_file Path to the input (encrypted) file.
 * @param output_file Path to the output (decrypted) file.
 * @param key Decryption key (256 bits).
 * @param iv Initialization vector (16 bytes, used in CBC mode).
 * @return int Returns 0 on success, or an error code on failure.
 */
int decryptFile(const char *input_file, const char *output_file, const unsigned char *key, const unsigned char *iv);

#endif // FILE_SECURE_H

#define AES_KEY_SIZE 256
#define AES_BLOCK_SIZE 16

int encryptFile(const char *input_file, const char *output_file, const unsigned char *key, const unsigned char *iv) {
    FILE *input = fopen(input_file, "rb");
    if (input == NULL) {
        perror("Failed to open input file");
        return ERR_FILE_OPEN;
    }

    FILE *output = fopen(output_file, "wb");
    if (output == NULL) {
        perror("Failed to open output file");
        fclose(input);
        return ERR_FILE_OPEN;
    }

    AES_KEY aes_key;
    if (AES_set_encrypt_key(key, AES_KEY_SIZE, &aes_key) < 0) {
        fprintf(stderr, "Failed to set AES encryption key\n");
        fclose(input);
        fclose(output);
        return ERR_AES_KEY;
    }

    unsigned char buffer_in[AES_BLOCK_SIZE];
    unsigned char buffer_out[AES_BLOCK_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer_in, 1, AES_BLOCK_SIZE, input)) > 0) {
        if (bytes_read < AES_BLOCK_SIZE) {
            memset(buffer_in + bytes_read, AES_BLOCK_SIZE - bytes_read, AES_BLOCK_SIZE - bytes_read);
        }

        AES_encrypt(buffer_in, buffer_out, &aes_key);
        fwrite(buffer_out, 1, AES_BLOCK_SIZE, output);
    }

    fclose(input);
    fclose(output);
    return 0;
}

int decryptFile(const char *input_file, const char *output_file, const unsigned char *key, const unsigned char *iv) {
    FILE *input = fopen(input_file, "rb");
    if (input == NULL) {
        perror("Failed to open input file");
        return ERR_FILE_OPEN;
    }

    FILE *output = fopen(output_file, "wb");
    if (output == NULL) {
        perror("Failed to open output file");
        fclose(input);
        return ERR_FILE_OPEN;
    }

    AES_KEY aes_key;
    if (AES_set_decrypt_key(key, AES_KEY_SIZE, &aes_key) < 0) {
        fprintf(stderr, "Failed to set AES decryption key\n");
        fclose(input);
        fclose(output);
        return ERR_AES_KEY;
    }

    unsigned char buffer_in[AES_BLOCK_SIZE];
    unsigned char buffer_out[AES_BLOCK_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer_in, 1, AES_BLOCK_SIZE, input)) > 0) {
        AES_decrypt(buffer_in, buffer_out, &aes_key);

        if (bytes_read < AES_BLOCK_SIZE) {
            size_t padding = buffer_out[AES_BLOCK_SIZE - 1];
            fwrite(buffer_out, 1, AES_BLOCK_SIZE - padding, output);
        } else {
            fwrite(buffer_out, 1, AES_BLOCK_SIZE, output);
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s [encrypt/decrypt] [input_file] [output_file]\n", argv[0]);
        return 1;
    }

    const char *operation = argv[1];
    const char *input_file = argv[2];
    const char *output_file = argv[3];

    unsigned char key[32] = "12345678901234567890123456789012";
    unsigned char iv[16] = "1234567890123456"; 

    if (strcmp(operation, "encrypt") == 0) {
        printf("Encrypting file: %s\n", input_file);
        int result = encryptFile(input_file, output_file, key, iv);
        if (result != 0) {
            printf("Error: Failed to encrypt file. Error code: %d\n", result);
            return result;
        }
        printf("File encrypted successfully: %s\n", output_file);

    } else if (strcmp(operation, "decrypt") == 0) {
        printf("Decrypting file: %s\n", input_file);
        int result = decryptFile(input_file, output_file, key, iv);
        if (result != 0) {
            printf("Error: Failed to decrypt file. Error code: %d\n", result);
            return result;
        }
        printf("File decrypted successfully: %s\n", output_file);

    } else {
        printf("Invalid operation: %s. Use 'encrypt' or 'decrypt'.\n", operation);
        return 1;
    }

    return 0;
}
