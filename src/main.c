#include <stdio.h>
#include <stdlib.h>
#include "ddoswatcher.h"

int main(int argc, char *argv[]) {
    const char *device = "eth0";         // Interface réseau à surveiller
    int port = DEFAULT_PORT;            // Port à surveiller
    int threshold = DEFAULT_THRESHOLD;  // Seuil d'alerte
    int interval = DEFAULT_INTERVAL;    // Intervalle de temps

    // Options utilisateur
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