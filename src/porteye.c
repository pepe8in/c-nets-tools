#include "../include/porteye.h"

int checkIp(const char *ip) {
    const char *regex_pattern = "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$";
    regex_t regex;
    int return_input;

    // Compiler l'expression régulière pour vérifier le format IPv4
    return_input = regcomp(&regex, regex_pattern, REG_EXTENDED);
    if (return_input) {
        fprintf(stderr, "Erreur de compilation de la regex\n");
        return -1;
    }

    // Tester si l'IP correspond à l'expression régulière
    return_input = regexec(&regex, ip, 0, NULL, 0);
    if (return_input != 0) {
        printf("Adresse IPv4 invalide : %s\n", ip);
        return -1;
    }

    regfree(&regex);
    return 0;
}

int checkPort(int start_port, int end_port) {
    if (start_port < 0 || start_port > MAX_PORTS || end_port < 1 || end_port > MAX_PORTS) {
        fprintf(stderr, "Erreur : plage de ports invalide <0-65535>\n");
        return -1;
    }

    if (start_port > end_port) {
        long int temp = start_port;
        start_port = end_port;
        end_port = temp;
    }
    return 0;
}

int createSocket(const char *ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erreur de création de socket");
        return -1;
    }

    // Définir un délai d'attente pour la connexion (timeout)
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

    // Préparer l'adresse du serveur à connecter
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);  // Convertir le port en format réseau
    inet_pton(AF_INET, ip, &server.sin_addr);  // Convertir l'IP en format binaire

    // Tenter de se connecter au port
    int result = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (result < 0) {
        close(sock);  // Fermer la socket en cas d'erreur
        return result;
    }

    return sock;  // Connexion réussie, retourner la socket ouverte
}

int scanPort(const char *ip, int port) {
    if (port < 0 || port > MAX_PORTS) {
        perror("Port inexistant");
        return -1;
    }

    int sock = createSocket(ip, port);
    if (sock < 0) {
        printf("Port %d: Fermé ou erreur\n", port);
        return 1;
    }

    printf("Port %d: Ouvert\n", port);
    close(sock);
    return 0;
}

int scanRange(const char *ip, int start_port, int end_port) {
    if (checkPort(start_port, end_port) == -1) {
        return -1;
    }

    printf("Scan des ports sur %s de %d à %d\n", ip, start_port, end_port);
    for (int port = start_port; port <= end_port; port++) {
        scanPort(ip, port);
    }
    return 0;
}

int scanOpenPort(const char *ip, int start_port, int end_port) {
    if (checkPort(start_port, end_port) == -1) {
        return -1;
    }

    printf("Ports ouverts sur %s de %d à %d : ", ip, start_port, end_port);
    for (int port = start_port; port <= end_port; port++) {
        int sock = createSocket(ip, port);
        if (sock >= 0) {
            printf(" %d", port);
            close(sock);
        }
    }
    printf("\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP>\n", argv[0]);
        return -1;
    }

    const char *ip = argv[1];
    if (checkIp(ip) == -1) {
        return -1;
    }

    int select, port, start_port, end_port; 

    printf("Options :\n• 1 : Scan d'un seul port\n• 2 : Scan d'une plage de ports\n• 3 : Scan de tous les ports\n• 4 : Scan des ports well-known\n• 5 : Scan des ports registered\n• 6 : Scan des ports dynamic/private\n• 7 : Afficher les ports ouverts d'une plage\n>>> ");
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
            scanOpenPort(ip, start_port, end_port);
            break;
        default: 
            fprintf(stderr, "Erreur : options non reconnues: %d <1-6>\n", select);
            return -1;
    }
    return 0;
}
