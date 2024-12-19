#ifndef PACKETSNOOP_H
#define PACKETSNOOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>       // Pour les structures IP
#include <netinet/tcp.h>      // Pour les structures TCP
#include <netinet/udp.h>      // Pour les structures UDP
#include <netinet/ip_icmp.h>  // Pour les structures ICMP
#include <netinet/if_ether.h> // Pour la structure ethhdr
#include <unistd.h>           // Pour la fonction close

/**
 * @brief Traite un paquet réseau reçu.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void ProcessPacket(unsigned char* buffer, int size);

/**
 * @brief Affiche les informations de l'en-tête IP d'un paquet.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void print_ip_header(unsigned char* buffer, int size);

/**
 * @brief Affiche les informations d'un paquet TCP.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void print_tcp_packet(unsigned char* buffer, int size);

/**
 * @brief Affiche les informations d'un paquet UDP.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void print_udp_packet(unsigned char* buffer, int size);

/**
 * @brief Affiche les informations d'un paquet ICMP.
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void print_icmp_packet(unsigned char* buffer, int size);

/**
 * @brief Affiche une représentation hexadécimale et ASCII des données.
 * @param data Pointeur vers les données à afficher.
 * @param size Taille des données en octets.
 */
void PrintData(unsigned char* data, int size);

#endif // PACKETSNOOP_H