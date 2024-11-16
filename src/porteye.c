#include </utils/codes/c/partiel/include/porteye.h>

int scanPort(const char *ip, int port) {
    if (port < 0 || port > MAX_PORTS) {
        perror("Port inexistant");
        return -1;
    }

    // Création de la socket
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erreur de création de socket");
        return -1;
    }

    // Définir un délai d'attente
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;      
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

    // Configuration de l'adresse du serveur
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server.sin_addr);

    // Essayer de se connecter au port
    int result = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (result < 0) {
        if (errno == ECONNREFUSED) {
            printf("Port %d: Fermé\n", port);
            close(sock);
            return 0;
        } else if (errno == ETIMEDOUT) {
            printf("Port %d: Filtré (délai de connexion)\n", port);
            close(sock);
            return 0;
        }
        printf("Port %d: Filtré ou erreur\n", port); 
        close(sock);
        return 0;
    }

    printf("Port %d: Ouvert\n", port);
    close(sock);
   return 1;
}

int scanRange(const char *ip, int start_port, int end_port) {
    if (start_port < 0 || start_port > MAX_PORTS || end_port < 1 || end_port > MAX_PORTS) {
        perror("Erreur : plage de ports invalide <0-65535>\n");
        return -1;
    }

    printf("Scan des ports sur %s de %d à %d\n", ip, start_port, end_port);
    for (int port = start_port; port <= end_port; port++) {
        scanPort(ip, port);
    }
    return 0; 
}
// A finir, le mettre sur toutes les plages du switch, temps de latence important 
int openPort(const char *ip, int start_port, int end_port) {
    if (start_port < 0 || start_port > MAX_PORTS || end_port < 1 || end_port > MAX_PORTS) {
        perror("Erreur : plage de ports invalide <0-65535>\n");
        return -1;
    }

    printf("Ports ouvert sur %s de %d à %d :", ip, start_port, end_port);
    for (int port = start_port; port <= end_port; port++) {
        int sock;
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Erreur de création de socket");
            return -1;
        }

        struct timeval tv;
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;      
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        inet_pton(AF_INET, ip, &server.sin_addr);

        int result = connect(sock, (struct sockaddr *)&server, sizeof(server));
        if (!(result < 0)) {
            printf(" %d", port);
        }
        close(sock);
    }
    printf(" \n");
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP>\n", argv[0]);
        return -1;
    }

    const char *ip = argv[1];
    int select, port, start_port, end_port; 

    printf("Options :\n• 1 : Scan d'un seul port\n• 2 : Scan d'une plage de ports\n• 3 : Scan de tous les ports\n• 4 : Scan des ports well-known\n• 5 : Scan des ports registered\n• 6 : Scan des ports dynamic/private\n• 7 : Afficher les ports ouvert d'une plage\n>>> ");
    scanf("%d", &select);

    switch (select) {
        case 1:
            printf("Quel port scanner ? ");
            scanf("%d", &port);
            scanPort(ip, port);
            break;
        case 2:
            printf("Quelle plage scanner ? ");
            scanf("%d %d", &start_port, &end_port);
            scanRange(ip, start_port, end_port);
            break;
        case 3:
            start_port = 0;
            end_port = MAX_PORTS;
            scanRange(ip, start_port, end_port);
            break;
        case 4:
            start_port = 0;
            end_port = 1023;
            scanRange(ip, start_port, end_port);
            break;
        case 5:
            start_port = 1024;
            end_port = 49151;
            scanRange(ip, start_port, end_port);
            break;
        case 6:
            start_port = 49151;
            end_port = MAX_PORTS;
            scanRange(ip, start_port, end_port);
            break;
        case 7:
            printf("Quelle plage scanner ? ");
            scanf("%d %d", &start_port, &end_port);
            openPort(ip, start_port, end_port);
            break;
        default: 
            fprintf(stderr, "Erreur : options non reconnues: %d <1-6>\n", select);
            return -1;
    }
    return 0;
}
