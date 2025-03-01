# c-nets-tools

## Contexte 

Dans le cadre de notre projet de fin de cursus en langage C avancé lors de notre seconde année de Bachelor en Génie Logiciel et Réseaux, nous avons décidé de concevoir un logiciel en langage C, intégrant une série de six outils distincts, chacun ayant pour but de répondre à des besoins spécifiques dans le domaine de la gestion et de la sécurité réseau. L'objectif global de ce projet est de démontrer notre maîtrise des concepts étudiés en cours, ainsi que notre capacité à développer des logiciels modulaires et fonctionnels.

Nous souhaitons créer une interface centralisée simple mais efficace qui permettra à l'utilisateur de naviguer et d'utiliser ces outils de manière intuitive. Les six outils que nous allons développer sont les suivants :

1. **Scanner de ports (PortEye)** : Cet outil permettra à l'utilisateur de scanner les ports ouverts sur une machine distante. Il identifiera les ports ouverts et renverra une liste de ces ports, afin de fournir une vue d'ensemble de la connectivité réseau de la machine cible.
    - Scanner une plage de ports ou un port spécifique sur une machine.
    - Afficher une liste des ports ouverts, fermés et filtrés.
    - Utilisation de sockets pour tester les connexions réseau.

2. **Analyseur de paquets (PacketSnoop)** : Cet outil interceptera et affichera les paquets réseau envoyé sur une interface réseau local. L'utilisateur pourra observer les informations contenues dans chaque paquet, telles que les en-têtes IP et les données des protocoles de transport comme TCP ou UDP.
    - Capture en temps réel des paquets sur une interface réseau.
    - Affichage des paquets (avec les informations de l'en-tête Ip et TCP/UDP).
    - Possibilité de filtrer par protocole (par exemple, afficher seulement les paquets HTTP).

3. **Chiffrement et déchiffrement de fichiers (FileSecure)** : Ce module permettra de chiffrer et de déchiffrer des fichiers à l'aide de l'algorithme AES, garantissant ainsi la confidentialité des données.
    - Lire un fichier texte ou binaire.
    - Appliquer l'algorithme AES pour chiffrer ou déchiffrer le fichier.
    - Sauvegarder le fichier chiffré et permettre sa récupération.
    - Implémentation d'un mode CSB ou ECB pour le chiffrement.

4. **Détection d'attaque par DDoS (DDoSWatcher)** : Cet outil surveillera le trafic réseau entrant pour détecter d'éventuelles attaques par déni de service en surveillant le taux de requêtes reçues et en alertant l'utilisateur en cas de détection de trafic suspect ou anormal.
    - Surveiller le nombre de paquets reçus sur un port spécifique.
    - Détecter des attaques par flood (taux élevé de requêtes en peu de temps).
    - Alerter l'utilisateur avec un message ou journaliser l'événement.

5. **Sniffer d'URL (URLSpy)** : Cet outil écoutera le trafic HTTP et extraira les URL des requêtes effectuées par les clients. Il permettra à l'utilisateur de visualiser les différents URL demandés sur le réseau, facilitant ainsi l'analyse du trafic web.
    - Intercepter les paquets HTTP.
    - Extraire les URL des requêtes GET.
    - Afficher les URL dans la console ou un ficher de log.

6. **Gestionnaire de bases de données via XML (XMLBuilder)** : Ce module utilisera un fichier XML pour générer dynamiquement une base de données MySQL en local. Il lira ensuite les informations contenues dans le fichier XML et produira un script SQL qui sera ensuite exécuté pour créer les tables et les relations dans la base de données.
    - Lire un fichier XML.
    - Générer un script SQL pour créer les tables et les relations.
    - Exécuter le script SQL sur une base de données MySQL pour la créer. 

## Prérequis, dépendances, installation, exécution 
- Système : Linux
- Compilateur : GCC
- Librairies : `libgtk-3-dev` `libxml2-dev` `libssl-dev` `libcap-dev` `libsqlite3-dev` `pkg-config`

    ```sh
    #Cloner le dépôt
    git clone https://github.com/pepe8in/c-nets-tools
    cd c-nets-tools

    #Compilation
    make

    #Exécution de l'interface principale
    sudo ./exe/c-nets-tools
    ```



## Estimation de production 

|         Tache                            | Estimation de temps  |   Responsable   |      Délais      |
|------------------------------------------|----------------------|-----------------|------------------|
| • Convention de code et git              |        1 heure       |    @pepe8in     |  avant 23/11/24  |
| • Dépôt distant et structure du projet   |        1 heure       |    @pepe8in     |  avant 23/11/24  |
| • Répartition + Trello                   |        1 heure       |    @pepe8in     |  avant 23/11/24  |
| • PortEye                                |       10 heures      |    @pepe8in     |  avant 05/01/25  |
| • PacketSnoop                            |       15 heures      |    @eafonso5    |  avant 05/01/25  |
| • FileSecure                             |       15 heures      |    @sangeric    |  avant 05/01/25  |
| • DDoSWatcher                            |       10 heures      |    @eafonso5    |  avant 05/01/25  |
| • URLSpy                                 |       15 heures      |    @pepe8in     |  avant 05/01/25  |
| • XMLBuilder                             |       15 heures      |    @sangeric    |  avant 05/01/25  |
| • Interface utilisateur avec SDL         |       20 heures      |    @pepe8in     |  avant 05/02/25  |

## Structure du projet 

    /c-net-tools
    │
    ├── /src
    │   ├── main.c               // Point d'entrée de l'application
    │   ├── interface.c          // Gestion de l'interface utilisateur
    │   ├── porteye.c            // Module Scanner de Ports
    │   ├── packetsnoop.c        // Module Analyseur de Paquets
    │   ├── filesecure.c         // Module Chiffrement/Déchiffrement
    │   ├── ddoswatcher.c        // Module Détection DDoS
    │   ├── urlspy.c             // Module Sniffer d'URL
    │   └── xmlbuilder.c         // Module Gestionnaire de Base de Données
    │
    ├── /include
    │   ├── interface.h
    │   ├── porteye.h
    │   ├── packetsnoop.h
    │   ├── filesecure.h
    │   ├── ddoswatcher.h
    │   ├── urlspy.h
    │   └── xmlbuilder.h
    │
    ├── /tests
    │   ├── test_main.c
    │   ├── test_porteye.c
    │   ├── test_packetsnoop.c
    │   ├── test_filesecure.c
    │   ├── test_ddoswatcher.c
    │   ├── test_urlspy.c
    │   └── test_xmlbuilder.c
    │
    ├── /exe
    │   ├── c-nets-tools
    │   ├── porteye
    │   ├── packetsnoop
    │   ├── filesecure
    │   ├── ddoswatcher
    │   ├── urlspy
    │   └── xmlbuilder
    │   
    ├── /.git
    ├── .gitignore
    ├── Makefile
    ├── LICENSE
    └── README.md

## Convention de code 

**Fonctions** :
- En anglais, en camelCase, avec des verbes pour indiquer l'action.
- Exemple : `void scanPorts(const char* target);`

**Variables** :
- En angais, en snake_case, avec des noms explicites décrivant leur usage.
- Exemple : `int port_count;`

**Constantes** :
- En anglais, en majuscules, avec des underscores pour séparer les mots.
- Exemple : `#define MAX_PORT 65535`

**Structures et Enums** :
- En PascalCase pour les noms de structures et d’énumérations.
- Exemple : `typedef struct NetworkConfig { ... } NetworkConfig;`

**Indentation** :
- Quatre espaces par niveau d'indentation.

**Espaces dans les fonctions et conditions** :
- Un espace après les virgules et autour des opérateurs et pas d'espace avant les parenthèses de la condition.
- Exemple :
    ```c
    if (port > 0 && port <= MAX_PORT) {
        // code
    }
    ```

**Espacement entre instructions** :
- Une ligne vide pour séparer les blocs logiques de code (par exemple, entre les déclarations de variables et les instructions de traitement).
- Exemple :
    ```c
    int a;
    int b;

    a = 5;
    b = 10;
    ```

**Pas d'espaces inutiles** :
- Éviter les espaces superflus, comme avant les points-virgules ou les accolades ouvrantes.

**Commentaires de fonction** :
- Ajouter un commentaire au-dessus de chaque fonction décrivant son but, ses paramètres, et son retour.
- Exemple :
    ```c
    /*
    * @brief Scanne les ports d'une machine cible.
    * @param target Adresse de la machine à scanner.
    * @param startPort Port de début.
    * @param endPort Port de fin.
    * @return int Retourne 0 en cas de succès, ou un code d'erreur en cas d'échec.
    */
    int scanPorts(const char* target, int startPort, int endPort);
    ```

**Commentaires Inline** :
- Utiliser des commentaires pour expliquer des parties de code complexes, mais les garder concis.
- Exemple :
    ```c
    // Vérifie si le port est valide
    if (port > 0 && port <= MAX_PORT) {
        // code
    }
    ```

**Code d'Erreur** :
- Utiliser des constantes ou des énumérations pour les codes d'erreur afin de rendre le code plus lisible.
- Exemple :
    ```c
    #define ERR_INVALID_PORT -1
    ```

## Convention git

**Branch** :
- `main` : Branche stable et prête à déployer.
- `feature/<nom>` : Pour les nouvelles fonctionnalités. Exemple : `feature/porteye`
- `bugfix/<nom>` : Pour les corrections de bugs ou corrections urgentes. Exemple : `bugfix/fix-porteye`
- `release/<version>` : Pour préparer une nouvelle version. Exemple : `release/v1.0`

**Commit** : 
- `feat` : Nouvelle fonctionnalité. Exemple : `feat(porteye): ajout du module porteye`
- `test` : Ajout/modification de tests. Exemple : `test(porteye): ajout des tests pour porteye`
- `docs` : Mise à jour de la documentation. Exemple : `docs(porteye): ajout de la documentation de l'outil sur README.md`
- `style` : Modifications de formatage ou refactorisation du code. Exemple : `style(scanner): alignement des indentations / style(scanner): simplification du code`
- `fix` : Correction de bug. Exemple : `fix(scanner): correction de la plage de ports`

## Méthodologie de développement d'un outil 

1. Création d'une branche 
2. Conception de la logique du programme
3. Développement d'un exécutable en CLI
4. Developpement de test unitaires
5. Revues de code en équipe
6. Documentation des dépendances, de la conceptualisation et du développement de l'outil
7. Merge dans la branche main

## PortEye
### Fonctions principales :
1. **Scan d'un seul port**
2. **Scan d'une plage de ports**
3. **Scan de tous les ports (0 à 65535)**
4. **Scan des ports well-known (0 à 1023)**
5. **Scan des ports registered (1024 à 49151)**
6. **Scan des ports dynamic/private (49152 à 65535)**
7. **Afficher les ports ouverts dans une plage**
### Amélioration : 
- Refacto code
- Ajout de test
- Bouton de retour au choix des options 
- Performance

## PacketSnoop
### Fonctions principales :
1. **Capture des paquets de notre machine en temps réel**  
### Amélioration : 
- Refacto code
- Ajout de test
- Problèmes dans la recupérations des données
- Filtrage
- Export des resultats 

## FileSecure
### Fonctions principales :
1. **Chiffrement de fichiers**  
2. **Déchiffrement de fichiers**  
### Amélioration : 
- Ajout de test 

## DDoSWatcher
### Fonctions principales :
**Liste des interfaces réseau disponibles**
**Filtrage du trafic par port spécifique**
**Détection d'activités suspectes basée sur un seuil de paquets**
**Blocage automatique des IP suspectes via iptables**
**Alertes et notifications en cas de trafic anormal**
### Amélioration : 
- Ajout de test 
- données en temps réel du nombre de paquets

## XMLBuilder
### Fonctions principales :
**Lis un fichier XML en génère un fichier SQL puis l'exécute**
### Amélioration : 
- Refacto code 
- Ajout de test 
- Erreur sur multi balise dans une balise parentes

## URLSpy

## Crédit
@pepe8in
@sangeric
@eafonso5
