#ifndef FILE_SECURE_H
#define FILE_SECURE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#define ERR_FILE_OPEN -1
#define ERR_AES_KEY -2
#define AES_KEY_SIZE 256
#define BUFFER_SIZE 1024

/*
 * @brief Dérive une clé et un IV à partir d'un mot de passe avec un sel unique.
 * @param password Mot de passe fourni par l'utilisateur.
 * @param key Tampon pour la clé dérivée (32 octets pour AES-256).
 * @param iv Tampon pour le vecteur d'initialisation (16 octets).
 * @param salt Tampon pour le sel (8 octets).
 */
void deriveKeyAndIV(const char *password, unsigned char *key, unsigned char *iv, unsigned char *salt);
/*
 * @brief Chiffre un fichier en utilisant AES en mode CBC.
 * @param input_file Chemin du fichier d'entrée.
 * @param output_file Chemin du fichier de sortie (chiffré).
 * @param key Clé de chiffrement (256 bits).
 * @param iv Vecteur d'initialisation (16 octets, utilisé en mode CBC).
 * @return int Retourne 0 en cas de succès, ou un code d'erreur en cas d'échec.
 */
int encryptFile(const char *input_file, const char *output_file, const unsigned char *key, const unsigned char *iv);
/*
 * @brief Déchiffre un fichier en utilisant AES en mode CBC.
 * @param input_file Chemin du fichier d'entrée (chiffré).
 * @param output_file Chemin du fichier de sortie (déchiffré).
 * @param key Clé de déchiffrement (256 bits).
 * @param iv Vecteur d'initialisation (16 octets, lu depuis le fichier chiffré).
 * @return int Retourne 0 en cas de succès, ou un code d'erreur en cas d'échec.
 */
int decryptFile(const char *input_file, const char *output_file, const unsigned char *key, unsigned char *iv);

#endif // FILE_SECURE_H
