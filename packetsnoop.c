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

void process(unsigned char* buffer, int size) {
    etherType(buffer, size);
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    switch (ip_header->protocol) {
        case IPPROTO_ICMP:
            icmpPacket(buffer, size);
            break;
        case IPPROTO_TCP:
            tcpPacket(buffer, size);
            struct tcphdr *tcp_header = (struct tcphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
            if (ntohs(tcp_header->source) == 443 || ntohs(tcp_header->dest) == 443) {
                int s = 443;
                httpPacket(buffer, size, s);
            } else if (ntohs(tcp_header->source) == 80 || ntohs(tcp_header->dest) == 80) {
                int s = 80;
                httpPacket(buffer, size, s);
            }
            break;
        case IPPROTO_UDP:
            udpPacket(buffer, size);
            break;
        default:
            printf(" --> informations du paquet non prises en charge.\n");
            break;
    }
}

void etherType(unsigned char* buffer, int size) {
    struct ethhdr *eth_header = (struct ethhdr *)buffer;
    unsigned short proto = ntohs(eth_header->h_proto);
    switch (proto) {
        case ETH_P_IP:
            struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
            struct sockaddr_in source_ipv4, dest_ipv4;
            source_ipv4.sin_addr.s_addr = ip_header->saddr;
            dest_ipv4.sin_addr.s_addr = ip_header->daddr;
            printf("(En-tête IPv4) Adresse source       : %s\n", inet_ntoa(source_ipv4.sin_addr));
            printf("(En-tête IPv4) Adresse destination  : %s\n", inet_ntoa(dest_ipv4.sin_addr));
            printf("(En-tête IPv4) Protocole            : %d", ip_header->protocol); 
        break;
        case ETH_P_IPV6:
            struct ipv6hdr *ipv6_header = (struct ipv6hdr*)(buffer + sizeof(struct ethhdr));
            char source_ipv6[INET6_ADDRSTRLEN], dest_ipv6[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &ipv6_header->saddr, source_ipv6, sizeof(source_ipv6));
            inet_ntop(AF_INET6, &ipv6_header->daddr, dest_ipv6, sizeof(dest_ipv6));
            printf("(En-tête IPv6) Adresse source       : %s\n", source_ipv6);
            printf("(En-tête IPv6) Adresse destination  : %s\n", dest_ipv6);
            printf("(En-tête IPv6) Next Header          : %d", ipv6_header->nexthdr);
        break;
        default:
        break;
    }
}

void icmpPacket(unsigned char* buffer, int size) {
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct icmphdr *icmp_header = (struct icmphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    printf(" (ICMP)\n");
    printf("(En-tête ICMP) Type                 : %d\n", icmp_header->type);
    printf("(En-tête ICMP) Code                 : %d\n", icmp_header->code);
}

void tcpPacket(unsigned char* buffer, int size) {
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct tcphdr *tcp_header = (struct tcphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    printf(" (TCP)\n");
    printf("(En-tête TCP) Port source           : %u\n", ntohs(tcp_header->source));
    printf("(En-tête TCP) Port destination      : %u\n", ntohs(tcp_header->dest));
    printf("(En-tête TCP) Flags                 : 0x%02X\n", tcp_header->fin|tcp_header->syn|tcp_header->rst|tcp_header->psh|tcp_header->ack|tcp_header->urg);
}

void udpPacket(unsigned char* buffer, int size) {
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct udphdr *udp_header = (struct udphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    printf(" (UDP)\n");
    printf("(En-tête UDP) Port source           : %u\n", ntohs(udp_header->source));
    printf("(En-tête UDP) Port destination      : %u\n", ntohs(udp_header->dest));
}

void httpPacket(unsigned char* buffer, int size, int s) {
    const char* protocol = (s == 443) ? "HTTPS" : "HTTP";
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct tcphdr *tcp_header = (struct tcphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    unsigned char* data = buffer + sizeof(struct ethhdr) + ip_header->ihl * 4 + tcp_header->doff * 4;
    int data_size = size - (data - buffer);
    if (data_size > 0) {
        printf("(Corps %s) Payload               : %d bytes\n", protocol, data_size);
        printf("(Corps %s) Data (hex & ASCII)    :\n", protocol);
        for (int i = 0; i < data_size; i++) {
            if (i != 0 && i % 16 == 0) printf("\n");
            printf("%02X ", data[i]);
            if (i % 16 == 15 || i == data_size - 1) {
                printf(" | ");
                for (int j = i - (i % 16); j <= i; j++) {
                    printf("%c", isprint(data[j]) ? data[j] : '.');
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    //if (argc != 2){
    //    fprintf(stderr, "Usage : %s <interface>\n", argv[0]);
    //    return 1;
    //}

    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);
    int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("Erreur de création de la socket");
        return 1;
    }

    //printf("Capture des paquets réseau...\n");
    while (1) {
        socklen_t saddr_len = sizeof(saddr);
        int data_size = recvfrom(sock_raw, buffer, BUFFER_SIZE, 0, &saddr, &saddr_len);
        if (data_size < 0) {
            perror("Erreur de réception des données");
            break;
        }
        process(buffer, data_size);
        printf("\n");
    }

    close(sock_raw);
    free(buffer);
    return 0;
}
