#include "../include/packetsnoop.h"
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <unistd.h> // Pour la fonction close

#define BUFFER_SIZE 65536

int main() {
    int sock_raw;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);

    // Création d'une socket brute
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
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
        ProcessPacket(buffer, data_size);
    }

    close(sock_raw);
    free(buffer);
    return 0;
}