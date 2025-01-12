#ifndef PACKETSNOOP_H
#define PACKETSNOOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/ipv6.h>

#define BUFFER_SIZE 65536

/**
 * @brief
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void process(unsigned char* buffer, int size);
/**
 * @brief 
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void etherType (unsigned char* buffer, int size);
/**
 * @brief
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void icmpPacket(unsigned char* buffer, int size);
/**
 * @brief 
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void tcpPacket(unsigned char* buffer, int size);
/**
 * @brief
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void udpPacket(unsigned char* buffer, int size);
/**
 * @brief
 * @param buffer Pointeur vers les données du paquet.
 * @param size Taille du paquet en octets.
 */
void httpPacket(unsigned char* buffer, int size, int s);

#endif


