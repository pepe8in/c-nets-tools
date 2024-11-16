#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define TIMEOUT 1
#define MAX_PORTS 65535

/*
 * @brief Scanne un port donné sur une adresse IP.
 * @param ip Adresse IP à scanner.
 * @param port Port à scanner.
 * @return -1 en cas d'erreur, 0 si le port est fermé, 1 s'il est ouvert.
 */
int scanPort(const char *ip, int port);

/*
 * @brief Scanne une plage de ports sur une adresse IP.
 * @param ip Adresse IP à scanner.
 * @param start_port Port de début de la plage.
 * @param end_port Port de fin de la plage.
 * @return -1 en cas d'erreur, 0 sinon.
 */
int scanRange(const char *ip, int start_port, int end_port);
