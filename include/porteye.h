// Bibliothèque standard d'entrée/sortie, fournit des fonctions de base pour la gestion des entrées/sorties (I/O) en C. Elle permet d'effectuer des opérations de lecture et d'écriture via le terminal ou des fichiers. Parmi les fonctions les plus courantes, on trouve printf() pour afficher des messages à l'écran, et scanf() pour lire les entrées de l'utilisateur.
#include <stdio.h>
// Bibliothèque standard de fonctions utilitaires, offre un ensemble de fonctions utiles pour la gestion de la mémoire, des processus, et des conversions de données. Elle inclut des fonctions telles que malloc() et free() pour allouer et libérer de la mémoire dynamique, exit() pour terminer un programme, et atoi() pour convertir des chaînes de caractères en entiers.
#include <stdlib.h>
// Bibliothèque de manipulation de chaînes de caractères, fournit des fonctions pour manipuler des chaînes de caractères en C. Les fonctions les plus utilisées incluent strcmp() pour comparer deux chaînes, strcpy() pour copier une chaîne dans une autre, et strlen() pour obtenir la longueur d'une chaîne. Ces fonctions facilitent le traitement des données textuelles dans les programmes C.
#include <string.h>
// Bibliothèque pour les expressions régulières, permet d'utiliser des expressions régulières en C, ce qui permet de rechercher, valider ou manipuler des chaînes de caractères en utilisant des motifs définis. Les fonctions principales sont regcomp() pour compiler une expression régulière, regexec() pour tester si une chaîne correspond à une expression régulière, et regfree() pour libérer les ressources allouées à l'expression régulière.
#include <regex.h>
// Bibliothèque d'assertions, utilisée pour la vérification des conditions dans le code pendant le développement. Elle permet d'ajouter des assertions, qui sont des tests de conditions logiques dans le programme. Si l'expression testée par assert() est fausse, l'exécution du programme s'arrête et un message d'erreur est affiché. Cela permet de détecter rapidement des erreurs pendant la phase de développement. Une utilisation courante serait :
#include <assert.h>
// Bibliothèque des fonctions POSIX de base, offre des fonctions de bas niveau pour travailler avec des fichiers, des processus, et d'autres mécanismes de l'OS. Elle inclut des fonctions comme close() pour fermer un fichier ou une socket, read() et write() pour la gestion des fichiers ou des flux, et fork() pour créer un nouveau processus. Cette bibliothèque est très utilisée dans la programmation système sous Unix/Linux.
#include <unistd.h>
// Bibliothèque pour la gestion des adresses IPv4, contient des fonctions pour manipuler les adresses réseau, principalement pour IPv4. La fonction inet_pton() est utilisée pour convertir une adresse IP au format texte (par exemple, "192.168.0.1") en un format binaire qui peut être utilisé dans les structures de socket. Cela permet de préparer les adresses avant de les utiliser dans des opérations réseau.
#include <arpa/inet.h>
// Bibliothèque des sockets, fournit des fonctions pour la création, la gestion et la communication via des sockets. Les fonctions incluent socket() pour créer une nouvelle socket, connect() pour établir une connexion, bind() pour lier une socket à une adresse et un port, et setsockopt() pour configurer des options spécifiques de la socket, comme les délais de timeout.
#include <sys/socket.h>
// Bibliothèque des erreurs système, contient la variable globale errno, qui est utilisée pour signaler les erreurs retournées par les fonctions système. Chaque fois qu'une fonction échoue (comme socket() ou connect()), la variable errno est mise à jour avec un code d'erreur spécifique. Ces erreurs peuvent être récupérées et affichées pour aider à diagnostiquer les problèmes. Les codes d'erreur sont définis dans cette bibliothèque, comme ENOENT pour un fichier introuvable, ou ECONNREFUSED pour une connexion refusée.
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
