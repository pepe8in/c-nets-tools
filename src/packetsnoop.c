#include "../include/packetsnoop.h"

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
                httpPacket(buffer, size, 443);
            } else if (ntohs(tcp_header->source) == 80 || ntohs(tcp_header->dest) == 80) {
                httpPacket(buffer, size, 80);
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
    printf("Adresse MAC destination : ");
    for (int i = 0; i < 6; i++) {
        printf("%02X", eth_header->h_dest[i]);
        if (i != 5) printf(":");
    }
    printf("\n");
    printf("Adresse MAC source : ");
    for (int i = 0; i < 6; i++) {
        printf("%02X", eth_header->h_source[i]);
        if (i != 5) printf(":");
    }
    printf("\n");

    unsigned short proto = ntohs(eth_header->h_proto);
    switch (proto) {
        case ETH_P_IP:
            struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
            struct sockaddr_in source_ipv4, dest_ipv4;
            source_ipv4.sin_addr.s_addr = ip_header->saddr;
            dest_ipv4.sin_addr.s_addr = ip_header->daddr;
            printf("(En-tête IPv4) Version              : %d\n", ip_header->version);
            printf("(En-tête IPv4) Longueur d'en-tête   : %d octets\n", ip_header->ihl * 4);
            printf("(En-tête IPv4) TTL                  : %d\n", ip_header->ttl);
            printf("(En-tête IPv4) Adresse source       : %s\n", inet_ntoa(source_ipv4.sin_addr));
            printf("(En-tête IPv4) Adresse destination  : %s\n", inet_ntoa(dest_ipv4.sin_addr));
            printf("(En-tête IPv4) Protocole            : %d", ip_header->protocol); 
        break;
        case ETH_P_IPV6:
            struct ipv6hdr *ipv6_header = (struct ipv6hdr*)(buffer + sizeof(struct ethhdr));
            char source_ipv6[INET6_ADDRSTRLEN], dest_ipv6[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &ipv6_header->saddr, source_ipv6, sizeof(source_ipv6));
            inet_ntop(AF_INET6, &ipv6_header->daddr, dest_ipv6, sizeof(dest_ipv6));
            printf("(En-tête IPv6) Version              : 6\n");
            printf("(En-tête IPv6) Classe de trafic     : 0x%02X\n", (ipv6_header->priority));
            printf("(En-tête IPv6) Flow Label           : 0x%05X\n", ntohl(*(uint32_t *)ipv6_header) & 0x000FFFFF);
            printf("(En-tête IPv6) Payload Length       : %d octets\n", ntohs(ipv6_header->payload_len));
            printf("(En-tête IPv6) Hop Limit            : %d\n", ipv6_header->hop_limit);
            printf("(En-tête IPv6) Adresse source       : %s\n", source_ipv6);
            printf("(En-tête IPv6) Adresse destination  : %s\n", dest_ipv6);
            printf("(En-tête IPv6) Next Header          : %d", ipv6_header->nexthdr);
        break;
        case ETH_P_ARP:
            struct arphdr *arp_header = (struct arphdr*)(buffer + sizeof(struct ethhdr));
            unsigned char *sender_mac = buffer + sizeof(struct ethhdr) + sizeof(struct arphdr);
            unsigned char *sender_ip = sender_mac + 6;
            unsigned char *target_mac = sender_ip + 4;
            unsigned char *target_ip = target_mac + 6;
            printf("(Protocole ARP) Matériel            : %d\n", ntohs(arp_header->ar_hrd));
            printf("(Protocole ARP) Protocole           : 0x%04x\n", ntohs(arp_header->ar_pro));
            printf("(Protocole ARP) Adresse IP émetteur : %d.%d.%d.%d\n", sender_ip[0], sender_ip[1], sender_ip[2], sender_ip[3]);
            printf("(Protocole ARP) Adresse IP cible    : %d.%d.%d.%d\n", target_ip[0], target_ip[1], target_ip[2], target_ip[3]);
            printf("(Protocole ARP) Opération           : %s\n", (ntohs(arp_header->ar_op) == 1) ? "Requête" : "Réponse");
        break;
        default:
            printf("Protocole Ethernet non pris en charge");
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

int main() {
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);
    int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("Erreur de création de la socket");
        return 1;
    }

    printf("Capture des paquets réseau...\n");
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
