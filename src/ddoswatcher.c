#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pcap.h>
#include "ddoswatcher.h"

// Variables globales pour le comptage
static int packet_count = 0;
static time_t start_time;

// Fonction pour initialiser le système de surveillance
void init_watcher(const char *device, int port, int threshold, int interval) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    // Ouvrir l'interface réseau
    handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le périphérique %s : %s\n", device, errbuf);
        exit(EXIT_FAILURE);
    }

    // Configurer le filtre pour surveiller un port spécifique
    struct bpf_program filter;
    char filter_exp[50];
    snprintf(filter_exp, sizeof(filter_exp), "tcp port %d", port);

    if (pcap_compile(handle, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Erreur de compilation du filtre : %s\n", pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    if (pcap_setfilter(handle, &filter) == -1) {
        fprintf(stderr, "Erreur d'application du filtre : %s\n", pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    printf("Surveillance du trafic sur le port %d...\n", port);
    start_time = time(NULL);

    // Lancer la capture des paquets
    pcap_loop(handle, 0, packet_handler, (u_char *)&threshold);

    // Fermer la session
    pcap_close(handle);
}

// Fonction appelée pour chaque paquet capturé
void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    int *threshold = (int *)args;
    time_t current_time = time(NULL);

    // Réinitialiser le compteur après l'intervalle
    if (difftime(current_time, start_time) > INTERVAL) {
        if (packet_count > *threshold) {
            printf("ALERTE : Trafic suspect détecté ! (%d paquets reçus en %d secondes)\n", packet_count, INTERVAL);
        }
        packet_count = 0;
        start_time = current_time;
    }

    // Incrémenter le compteur
    packet_count++;
}