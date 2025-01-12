#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <regex.h>

#define TIMEOUT 1
#define MAX_PORTS 65535

/*
 * @brief Vérifie si une adresse IP est valide (au format IPv4).
 * @param ip L'adresse IP à vérifier.
 * @return 0 si l'adresse est valide, -1 si elle est invalide.
 */
int checkIp(const char *ip);
/*
 * @brief Vérifie si les ports de début et de fin sont valides.
 * @param start_port Le port de début de la plage.
 * @param end_port Le port de fin de la plage.
 * @return 0 si la plage de ports est valide, -1 sinon.
 */
int checkPort(int start_port, int end_port);
/*
 * @brief Crée une socket et tente de se connecter à un port donné.
 * @param ip L'adresse IP du serveur.
 * @param port Le port à tester.
 * @return La socket si la connexion réussit, un code d'erreur si la connexion échoue.
 */
int createSocket(const char *ip, int port); 
/*
 * @brief Scanne un seul port sur une adresse IP.
 * @param ip L'adresse IP à scanner.
 * @param port Le port à scanner.
 * @return -1 en cas d'erreur, 1 si le port est fermé, 0 si le port est ouvert.
 */
int scanPort(const char *ip, int port);
/*
 * @brief Scanne une plage de ports sur une adresse IP.
 * @param ip L'adresse IP à scanner.
 * @param start_port Le port de début de la plage.
 * @param end_port Le port de fin de la plage.
 * @return -1 en cas d'erreur, 0 si le scan réussit.
 */
int scanRange(const char *ip, int start_port, int end_port);
/*
 * @brief Scanne les ports ouverts dans une plage sur une adresse IP.
 * @param ip L'adresse IP à scanner.
 * @param start_port Le port de début de la plage.
 * @param end_port Le port de fin de la plage.
 * @return -1 en cas d'erreur, 0 si le scan réussit.
 */
int scanOpenPort(const char *ip, int start_port, int end_port);
