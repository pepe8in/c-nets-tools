#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <net/if.h>
#include <linux/ipv6.h>

#define BUFFER_SIZE 65536

/**
 * @brief Traite un paquet réseau capturé et identifie son type.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void process(unsigned char* buffer, int size);
/**
 * @brief Analyse et affiche les informations liées au protocole Ethernet.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void etherType(unsigned char* buffer, int size);
/**
 * @brief Analyse et affiche les informations d'un paquet ICMP.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void icmpPacket(unsigned char* buffer, int size);
/**
 * @brief Analyse et affiche les informations d'un paquet TCP.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void tcpPacket(unsigned char* buffer, int size);
/**
 * @brief Analyse et affiche les informations d'un paquet UDP.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void udpPacket(unsigned char* buffer, int size);
/**
 * @brief Analyse et affiche les données utiles d'un paquet HTTP ou HTTPS.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 * @param s Indique le port (80 pour HTTP, 443 pour HTTPS).
 */
void httpPacket(unsigned char* buffer, int size, int s);

