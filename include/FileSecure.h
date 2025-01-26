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