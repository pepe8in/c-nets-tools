// Bibliothèque standard d'entrée/sortie
#include <stdio.h>
// Bibliothèque standard de fonctions utilitaires
#include <stdlib.h>
// Bibilothèque de manipulation de chaîne de caractères : strcmp() compare deux chaînes, strcpy() copie une chaîne dans une autre
#include <string.h>
// Bibliothèque pour expressions régulières : regcomp() compile une regex, regexec() éxécute la regex compilée pour vérifier si une chaîne correspond, regfree() libère les ressources allouées pour la regex
#include <regex.h>
// Bibliothèque des fonctions POSIX de base (fournit des fonctions système de bas niveau pour l'accès aux fichiers, la gestion des processus, et diverses autres opérations sur les systèmes Unix) : close() ferme un descripteur de fichier (ici, une socket)
#include <unistd.h>
// Bibliothèque utilisée pour gérer les adresses et les communications réseau avec les protocoles de la famille IPv4 : inet_pton() convertit une adresse IPv4 au format texte en format binaire pour être utilisée dans les structures de socket
#include <arpa/inet.h>
// Bibliothèque des sockets qui fournit des fonctions pour la création, la configuration et la gestion des connexions réseau via des sockets : socket() crée une socket pour une communication TCP ou UDP dans notre cas, connect() établit une connexion avec un serveur spécifié par une adresse IP et un port, setsockopt() permet de configurer certaines options d'une socket (par exemple, le timeout)
#include <sys/socket.h>
// Bibliothèque des codes d'érreur système : errno : Variable globale qui contient le code d'erreur de la dernière fonction système échouée.
#include <errno.h>

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
