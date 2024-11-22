#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <string.h>

#define MAX_PACKET_SIZE 65536
#define HTTP_PORT 80

// Fonction pour extraire les URL des requêtes GET HTTP
void extract_url(const u_char *packet, struct pcap_pkthdr header) {
    const char *http_request;
    
    // Recherche du début de la requête HTTP GET (après les en-têtes Ethernet et IP)
    http_request = (const char *)(packet + 14 + 20 + 20); // 14 (Ethernet), 20 (IP), 20 (TCP)
    
    // Vérification si la requête commence par "GET "
    if (strncmp(http_request, "GET ", 4) == 0) {
        const char *url_start = http_request + 4;
        const char *url_end = strchr(url_start, ' ');

        if (url_end != NULL) {
            char url[256];
            int url_len = url_end - url_start;
            if (url_len > 255) url_len = 255;
            strncpy(url, url_start, url_len);
            url[url_len] = '\0';  // Null-terminate the string
            
            // Affichage de l'URL sur la console
            printf("URL capturée : %s\n", url);
        }
    }
}

// Fonction de rappel pour traiter chaque paquet capturé
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    printf("Paquet capturé : Taille = %d\n", pkthdr->len);
    // Filtrer uniquement les paquets TCP vers le port HTTP (80)
    if (packet[23] == 6) { // Si le protocole est TCP (protocole 6)
        u_int16_t port_dest = (packet[36] << 8) + packet[37];  // Extraction du port destination
        printf("Port destination : %d\n", port_dest); // Affichez le port pour voir si c'est bien HTTP
        if (port_dest == HTTP_PORT) {
            // Analyser le paquet pour extraire l'URL
            extract_url(packet, *pkthdr);
        }
    }
}

// Fonction principale
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <interface>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    // Ouvrir l'interface réseau pour la capture
    handle = pcap_open_live(argv[1], MAX_PACKET_SIZE, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Erreur d'ouverture de l'interface : %s\n", errbuf);
        return EXIT_FAILURE;
    }

    printf("Démarrage de la capture sur l'interface %s...\n", argv[1]);
    
    // Capturer les paquets indéfiniment jusqu'à l'arrêt
    if (pcap_loop(handle, 0, packet_handler, NULL) < 0) {
        fprintf(stderr, "Erreur de capture de paquets : %s\n", pcap_geterr(handle));
        pcap_close(handle);
        return EXIT_FAILURE;
    }

    pcap_close(handle);
    return EXIT_SUCCESS;
}

