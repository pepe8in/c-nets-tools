#include "../include/packetsnoop.h"

void ProcessPacket(unsigned char* buffer, int size) {
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    switch (ip_header->protocol) {
        case 1: // ICMP
            print_icmp_packet(buffer, size);
            break;
        case 6: // TCP
            print_tcp_packet(buffer, size);
            break;
        case 17: // UDP
            print_udp_packet(buffer, size);
            break;
        default:
            printf("Protocole inconnu : %d\n", ip_header->protocol);
            break;
    }
}

void print_ip_header(unsigned char* buffer, int size) {
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    printf("\nEn-tête IP:\n");
    printf("  Version : %d\n", ip_header->version);
    printf("  Longueur d'en-tête : %d\n", ip_header->ihl * 4);
    printf("  Protocole : %d\n", ip_header->protocol);
}

void print_tcp_packet(unsigned char* buffer, int size) {
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct tcphdr *tcp_header = (struct tcphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    printf("\nPaquet TCP:\n");
    printf("  Port source : %u\n", ntohs(tcp_header->source));
    printf("  Port destination : %u\n", ntohs(tcp_header->dest));
}

void print_udp_packet(unsigned char* buffer, int size) {
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct udphdr *udp_header = (struct udphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    printf("\nPaquet UDP:\n");
    printf("  Port source : %u\n", ntohs(udp_header->source));
    printf("  Port destination : %u\n", ntohs(udp_header->dest));
}

void print_icmp_packet(unsigned char* buffer, int size) {
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct icmphdr *icmp_header = (struct icmphdr*)(buffer + ip_header->ihl * 4 + sizeof(struct ethhdr));
    printf("\nPaquet ICMP:\n");
    printf("  Type : %d\n", icmp_header->type);
    printf("  Code : %d\n", icmp_header->code);
}

void PrintData(unsigned char* data, int size) {
    for (int i = 0; i < size; i++) {
        if (i != 0 && i % 16 == 0) printf("\n");
        printf("%02X ", data[i]);
    }
    printf("\n");
}