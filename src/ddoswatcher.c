#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pcap.h>

#define DEFAULT_THRESHOLD 100  // Nombre maximum de paquets par intervalle
#define DEFAULT_INTERVAL 5     // Intervalle de temps en secondes
#define DEFAULT_PORT 80        // Port à surveiller

// Déclaration des fonctions
void init_watcher(const char *device, int port, int threshold, int interval);
void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

// Variables globales pour le comptage
static int packet_count = 0;   // Compteur de paquets
static time_t start_time;      // Heure de début du comptage

// Fonction pour initialiser le système de surveillance
void init_watcher(const char *device, int port, int threshold, int interval) {
    char errbuf[PCAP_ERRBUF_SIZE]; // Buffer pour les messages d'erreur
    pcap_t *handle;               // Handle pour capturer le trafic

    // Ouvrir l'interface réseau en mode capture
    handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le périphérique %s : %s\n", device, errbuf);
        exit(EXIT_FAILURE);
    }

    // Configurer le filtre BPF pour surveiller un port spécifique
    struct bpf_program filter;
    char filter_exp[50];
    snprintf(filter_exp, sizeof(filter_exp), "tcp port %d", port);

    if (pcap_compile(handle, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Erreur de compilation du filtre : %s\n", pcap_geterr(handle));
        pcap_close(handle);
        exit(EXIT_FAILURE);
    }

    if (pcap_setfilter(handle, &filter) == -1) {
        fprintf(stderr, "Erreur d'application du filtre : %s\n", pcap_geterr(handle));
        pcap_freecode(&filter);
        pcap_close(handle);
        exit(EXIT_FAILURE);
    }

    printf("Surveillance du trafic sur le port %d...\n", port);
    start_time = time(NULL); // Initialiser le temps de début

    // Lancer la capture des paquets (boucle infinie)
    pcap_loop(handle, 0, packet_handler, (u_char *)&threshold);

    // Nettoyage
    pcap_freecode(&filter);
    pcap_close(handle);
}

// Fonction appelée pour chaque paquet capturé
void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    int *threshold = (int *)args; // Récupérer le seuil passé en argument
    time_t current_time = time(NULL); // Temps actuel

    // Réinitialiser le compteur après l'intervalle
    if (difftime(current_time, start_time) >= DEFAULT_INTERVAL) {
        if (packet_count > *threshold) {
            printf("ALERTE : Trafic suspect détecté ! (%d paquets reçus en %d secondes)\n", packet_count, DEFAULT_INTERVAL);
        }
        packet_count = 0;          // Réinitialiser le compteur
        start_time = current_time; // Réinitialiser l'heure de début
    }

    // Incrémenter le compteur pour chaque paquet
    packet_count++;
}

// Point d'entrée du programme
int main(int argc, char *argv[]) {
    const char *device = "eth0";         // Interface réseau à surveiller
    int port = DEFAULT_PORT;            // Port à surveiller
    int threshold = DEFAULT_THRESHOLD;  // Seuil d'alerte
    int interval = DEFAULT_INTERVAL;    // Intervalle de temps

    // Gérer les options de la ligne de commande
    if (argc > 1) device = argv[1];
    if (argc > 2) port = atoi(argv[2]);
    if (argc > 3) threshold = atoi(argv[3]);
    if (argc > 4) interval = atoi(argv[4]);

    printf("Lancement de DDoSWatcher...\n");
    printf("Interface : %s, Port : %d, Seuil : %d, Intervalle : %d secondes\n", device, port, threshold, interval);

    // Initialiser et démarrer le watcher
    init_watcher(device, port, threshold, interval);

    return 0;
}

