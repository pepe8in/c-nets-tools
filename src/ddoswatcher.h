#ifndef DDOSWATCHER_H
#define DDOSWATCHER_H

// Déclaration des constantes
#define DEFAULT_THRESHOLD 100  // Nombre maximum de paquets par intervalle
#define DEFAULT_INTERVAL 5     // Intervalle de temps en secondes
#define DEFAULT_PORT 80        // Port à surveiller

// Déclaration des fonctions
void init_watcher(const char *device, int port, int threshold, int interval);
void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

#endif