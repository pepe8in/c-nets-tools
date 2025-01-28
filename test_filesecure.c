#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_secure.h" // Assurez-vous que c'est le bon nom pour l'inclusion

#define INPUT_FILE "test_input.txt"
#define ENCRYPTED_FILE "test_encrypted.bin"
#define DECRYPTED_FILE "test_decrypted.txt"

void createTestInputFile() {
    // Création d'un fichier contenant des données de test
    FILE *file = fopen(INPUT_FILE, "w");
    if (file == NULL) {
        perror("Erreur : Impossible de créer le fichier de test");
        exit(1);
    }

    fprintf(file, "Ceci est un test pour vérifier le chiffrement et le déchiffrement.");
    fclose(file);
}

int compareFiles(const char *file1, const char *file2) {
    // Compare le contenu de deux fichiers
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    if (!f1 || !f2) {
        perror("Erreur : Impossible d'ouvrir les fichiers pour comparaison");
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0;
    }

    int result = 1; // Supposons que les fichiers sont identiques
    char c1, c2;
    while (!feof(f1) && !feof(f2)) {
        c1 = fgetc(f1);
        c2 = fgetc(f2);
        if (c1 != c2) {
            result = 0; // Les fichiers diffèrent
            break;
        }
    }

    if (!feof(f1) || !feof(f2)) {
        result = 0; // Taille des fichiers différente
    }

    fclose(f1);
    fclose(f2);
    return result;
}

int main() {
    printf("Démarrage du test...\n");

    // Étape 1 : Créer un fichier d'entrée de test
    createTestInputFile();
    printf("Fichier d'entrée créé : %s\n", INPUT_FILE);

    // Étape 2 : Mot de passe pour dériver la clé et l'IV
    char password[64] = "MotDePasseTest"; // Mot de passe fixe pour le test
    unsigned char key[32];
    unsigned char iv[16];
    unsigned char salt[8];

    // Étape 3 : Dériver la clé et l'IV à partir du mot de passe
    deriveKeyAndIV(password, key, iv, salt);

    // Étape 4 : Chiffrement du fichier
    printf("Chiffrement en cours...\n");
    if (encryptFile(INPUT_FILE, ENCRYPTED_FILE, key, iv) != 0) {
        fprintf(stderr, "Erreur : Échec du chiffrement\n");
        return 1;
    }
    printf("Fichier chiffré avec succès : %s\n", ENCRYPTED_FILE);

    // Étape 5 : Réinitialisation de l'IV pour le déchiffrement
    unsigned char iv_decrypt[16];
    memcpy(iv_decrypt, iv, 16);

    // Étape 6 : Déchiffrement du fichier
    printf("Déchiffrement en cours...\n");
    if (decryptFile(ENCRYPTED_FILE, DECRYPTED_FILE, key, iv_decrypt) != 0) {
        fprintf(stderr, "Erreur : Échec du déchiffrement\n");
        return 1;
    }
    printf("Fichier déchiffré avec succès : %s\n", DECRYPTED_FILE);

    // Étape 7 : Validation du résultat
    if (compareFiles(INPUT_FILE, DECRYPTED_FILE)) {
        printf("Test réussi : Le fichier déchiffré correspond à l'original.\n");
    } else {
        printf("Test échoué : Le fichier déchiffré ne correspond pas à l'original.\n");
        return 1;
    }

    // Nettoyage des fichiers temporaires (optionnel)
    remove(INPUT_FILE);
    remove(ENCRYPTED_FILE);
    remove(DECRYPTED_FILE);

    printf("Test terminé.\n");
    return 0;
}

