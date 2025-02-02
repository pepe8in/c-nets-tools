#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <sqlite3.h>

#define MAX_TABLE_NAME 256
#define MAX_TABLES 100
#define MAX_RELATIONS 100

/*
 * @brief Structure pour stocker les relations entre deux tables.
 */
typedef struct {
    char table_1[256];
    char table_2[256];
    char attributes[10][256];
    int attr_count;
} RelationAttributes;

/*
 * @brief Structure pour gérer les compteurs d'ID par table.
 */
typedef struct {
    char table_name[256];
    int current_id;
} TableID;

RelationAttributes relation_attributes[MAX_RELATIONS];
int relation_count = 0;

TableID id_counters[MAX_TABLES];
int id_counter_count = 0;

/*
 * @brief Vérifie si une chaîne ne contient que des espaces blancs.
 * @param str Chaîne à vérifier.
 * @return int Renvoie 1 si la chaîne ne contient que des espaces blancs, sinon 0.
 */
int isWhitespaceOnly(const char *str) {
    while (*str) {
        if (*str != ' ' && *str != '\n' && *str != '\t' && *str != '\r') {
            return 0;
        }
        str++;
    }
    return 1;
}

/*
 * @brief Nettoie une chaîne en remplaçant les retours à la ligne, tabulations et espaces multiples par un seul espace.
 * @param str Chaîne à nettoyer.
 */
void cleanString(char *str) {
    char *src = str, *dst = str;
    int in_space = 0;

    while (*src) {
        if (*src == '\n' || *src == '\r' || *src == '\t') {
            *src = ' ';
        }
        if (*src == ' ') {
            if (in_space) {
                src++;
                continue;
            }
            in_space = 1;
        } else {
            in_space = 0;
        }
        *dst++ = *src++;
    }
    *dst = '\0';

    char *start = str;
    while (*start == ' ') {
        start++;
    }

    char *end = dst - 1;
    while (end > start && *end == ' ') {
        end--;
    }
    *(end + 1) = '\0';

    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}

/*
 * @brief Échappe une chaîne pour une insertion sécurisée dans une requête SQL.
 * @param input Chaîne d'entrée.
 * @param output Buffer de sortie.
 * @param max_size Taille maximale du buffer de sortie.
 */
void escapeSqlString(const char *input, char *output, size_t max_size) {
    size_t j = 0;
    for (size_t i = 0; input[i] != '\0' && j < max_size - 1; i++) {
        if (input[i] == '\'') {
            if (j < max_size - 2) {
                output[j++] = '\'';
                output[j++] = '\'';
            }
        } else {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

/*
 * @brief Récupère un nouvel identifiant pour une table donnée.
 * @param table_name Nom de la table.
 * @return int Nouvel identifiant.
 */
int getNewId(const char *table_name) {
    for (int i = 0; i < id_counter_count; i++) {
        if (strcmp(id_counters[i].table_name, table_name) == 0) {
            return ++id_counters[i].current_id;
        }
    }

    strncpy(id_counters[id_counter_count].table_name, table_name, 256);
    id_counters[id_counter_count].current_id = 1;
    return id_counters[id_counter_count++].current_id;
}

/*
 * @brief Vérifie si une table existe déjà dans la liste.
 * @param table_name Nom de la table.
 * @param tables Liste des tables.
 * @param table_count Nombre de tables dans la liste.
 * @return int Renvoie 1 si la table existe, sinon 0.
 */
int tableExists(const char *table_name, char tables[MAX_TABLES][MAX_TABLE_NAME], int table_count) {
    for (int i = 0; i < table_count; i++) {
        if (strcmp(tables[i], table_name) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * @brief Détermine si un nœud XML représente une clé étrangère (contient des éléments enfants).
 * @param node Nœud XML.
 * @return int Renvoie 1 si c'est une clé étrangère, sinon 0.
 */
int isForeignKey(xmlNodePtr node) {
    for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE) {
            return 1;
        }
    }
    return 0;
}

/*
 * @brief Vérifie si un nœud XML possède des éléments enfants.
 * @param node Nœud XML.
 * @return int Renvoie 1 s'il existe au moins un enfant de type élément, sinon 0.
 */
int hasChildElements(xmlNodePtr node) {
    for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE) {
            return 1;
        }
    }
    return 0;
}

/*
 * @brief Exécute un fichier SQL sur une base de données SQLite.
 * @param db_name Nom de la base de données.
 * @param sql_file Fichier SQL à exécuter.
 * @return int Renvoie 0 en cas de succès, sinon 1.
 */
int executeSqlFile(const char *db_name, const char *sql_file) {
    sqlite3 *db;
    char *err_msg = 0;
    FILE *file;
    char *sql;
    long length;

    if (sqlite3_open(db_name, &db) != SQLITE_OK) {
        fprintf(stderr, "Error: Could not open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    file = fopen(sql_file, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open SQL file: %s\n", sql_file);
        sqlite3_close(db);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);

    sql = malloc(length + 1);
    if (sql == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        sqlite3_close(db);
        return 1;
    }

    fread(sql, 1, length, file);
    sql[length] = '\0';
    fclose(file);

    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "Error: SQL execution failed: %s\n", err_msg);
        sqlite3_free(err_msg);
        free(sql);
        sqlite3_close(db);
        return 1;
    }

    free(sql);
    sqlite3_close(db);

    printf("SQL file executed successfully!\n");
    return 0;
}

/*
 * @brief Ajoute une relation entre deux tables si elle n'existe pas déjà.
 * @param table1 Nom de la première table.
 * @param table2 Nom de la deuxième table.
 * @param relation_node Nœud XML contenant la relation.
 */
void addRelation(const char *table1, const char *table2, xmlNodePtr relation_node) {
    for (int i = 0; i < relation_count; i++) {
        if ((strcmp(relation_attributes[i].table_1, table1) == 0 &&
             strcmp(relation_attributes[i].table_2, table2) == 0) ||
            (strcmp(relation_attributes[i].table_1, table2) == 0 &&
             strcmp(relation_attributes[i].table_2, table1) == 0)) {
            return;
        }
    }

    strncpy(relation_attributes[relation_count].table_1, table1, 256);
    strncpy(relation_attributes[relation_count].table_2, table2, 256);
    relation_attributes[relation_count].attr_count = 0;

    for (xmlNodePtr attr_node = relation_node->children; attr_node != NULL; attr_node = attr_node->next) {
        if (attr_node->type == XML_ELEMENT_NODE) {
            int exists = 0;
            for (int j = 0; j < relation_attributes[relation_count].attr_count; j++) {
                if (strcmp(relation_attributes[relation_count].attributes[j], (char *)attr_node->name) == 0) {
                    exists = 1;
                    break;
                }
            }
            if (!exists && relation_attributes[relation_count].attr_count < 10) {
                strncpy(relation_attributes[relation_count].attributes[relation_attributes[relation_count].attr_count],
                        (char *)attr_node->name, 256);
                relation_attributes[relation_count].attr_count++;
            }
        }
    }
    relation_count++;
}

/*
 * @brief Crée une table SQL à partir d'un nœud XML.
 * @param output_file Fichier de sortie où écrire le SQL.
 * @param node Nœud XML représentant la table.
 * @param tables Liste des tables créées.
 * @param table_count Pointeur sur le nombre de tables créées.
 */
void createTable(FILE *output_file, xmlNodePtr node, char tables[MAX_TABLES][MAX_TABLE_NAME], int *table_count) {
    if (!hasChildElements(node)) {
        return;
    }

    if (tableExists((char *)node->name, tables, *table_count)) {
        return;
    }

    strncpy(tables[*table_count], (char *)node->name, MAX_TABLE_NAME);
    (*table_count)++;

    fprintf(output_file, "CREATE TABLE IF NOT EXISTS %s (\n", node->name);
    fprintf(output_file, "    id_%s INTEGER PRIMARY KEY", node->name);

    for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
        if (attr->name != NULL) {
            fprintf(output_file, ",\n    %s varchar(42)", attr->name);
        }
    }

    char foreign_keys[MAX_TABLES][256];
    int fk_count = 0;
    int has_columns = 0;

    for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE) {
            if (isForeignKey(child)) {
                addRelation((char *)node->name, (char *)child->name, child);
                fk_count++;
            } else if (!hasChildElements(child) && child->properties != NULL) {
                for (xmlAttrPtr attr = child->properties; attr != NULL; attr = attr->next) {
                    if (attr->name != NULL) {
                        fprintf(output_file, ",\n    %s_%s varchar(42)", child->name, attr->name);
                    }
                }
            } else {
                int duplicate = 0;
                for (xmlNodePtr check_node = node->children; check_node != child; check_node = check_node->next) {
                    if (check_node->type == XML_ELEMENT_NODE &&
                        strcmp((char *)check_node->name, (char *)child->name) == 0) {
                        duplicate = 1;
                        break;
                    }
                }
                if (!duplicate) {
                    fprintf(output_file, ",\n    %s varchar(42)", child->name);
                }
                has_columns = 1;
            }
        }
    }

    for (int i = 0; i < fk_count; i++) {
        fprintf(output_file, "%s", foreign_keys[i]);
    }

    fprintf(output_file, "\n);\n\n");

    for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE) {
            createTable(output_file, child, tables, table_count);
        }
    }
}

/*
 * @brief Insère les données d'un nœud XML dans la table SQL correspondante.
 * @param output_file Fichier de sortie où écrire le SQL.
 * @param node Nœud XML contenant les données.
 * @param parent_id Identifiant du parent (0 si aucun).
 * @param parent_table Nom de la table parente (NULL si aucun).
 */
void insertData(FILE *output_file, xmlNodePtr node, int parent_id, const char *parent_table) {
    if (node->type != XML_ELEMENT_NODE) {
        return;
    }

    int new_id = getNewId((char *)node->name);
    fprintf(output_file, "INSERT INTO %s (id_%s", node->name, node->name);

    for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
        if (attr->name != NULL) {
            fprintf(output_file, ", %s", attr->name);
        }
    }

    for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE && !isForeignKey(child)) {
            if (!hasChildElements(child) && child->properties != NULL) {
                for (xmlAttrPtr attr = child->properties; attr != NULL; attr = attr->next) {
                    if (attr->name != NULL) {
                        fprintf(output_file, ", %s_%s", child->name, attr->name);
                    }
                }
            } else {
                fprintf(output_file, ", %s", child->name);
            }
        }
    }

    fprintf(output_file, ") VALUES (%d", new_id);

    for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
        if (attr->name != NULL) {
            xmlChar *attr_value = xmlGetProp(node, attr->name);

            if (attr_value != NULL) {
                char escaped_value[512];
                escapeSqlString((char *)attr_value, escaped_value, sizeof(escaped_value));
                fprintf(output_file, ", '%s'", escaped_value);
                xmlFree(attr_value);
            } else {
                fprintf(output_file, ", NULL");
            }
        }
    }

    for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE && !isForeignKey(child)) {
            if (!hasChildElements(child) && child->properties != NULL) {
                for (xmlAttrPtr attr = child->properties; attr != NULL; attr = attr->next) {
                    xmlChar *attr_value = xmlGetProp(child, attr->name);
                    if (attr_value != NULL) {
                        char escaped_value[512];
                        escapeSqlString((char *)attr_value, escaped_value, sizeof(escaped_value));
                        fprintf(output_file, ", '%s'", escaped_value);
                        xmlFree(attr_value);
                    } else {
                        fprintf(output_file, ", NULL");
                    }
                }
            } else {
                xmlChar *content = xmlNodeGetContent(child);
                if (content != NULL) {
                    char escaped_content[512];
                    escapeSqlString((char *)content, escaped_content, sizeof(escaped_content));
                    fprintf(output_file, ", '%s'", escaped_content);
                    xmlFree(content);
                } else {
                    fprintf(output_file, ", NULL");
                }
            }
        }
    }

    fprintf(output_file, ");\n");

    if (parent_id > 0 && parent_table != NULL) {
        fprintf(output_file,
                "INSERT INTO %s_%s (id_%s, id_%s) VALUES (%d, %d);\n",
                parent_table, node->name, parent_table, node->name, parent_id, new_id);
    }

    for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE && isForeignKey(child)) {
            insertData(output_file, child, new_id, node->name);
        }
    }
}

/*
 * @brief Génère le fichier SQL à partir du document XML.
 * @param root Racine du document XML.
 */
void generateSql(xmlNodePtr root) {
    if (root == NULL) {
        return;
    }

    FILE *output_file = fopen("output.sql", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error: Couldn't open output.sql for writing\n");
        return;
    }

    char tables[MAX_TABLES][MAX_TABLE_NAME];
    int table_count = 0;

    for (xmlNodePtr node = root->children; node != NULL; node = node->next) {
        if (node->type == XML_ELEMENT_NODE) {
            createTable(output_file, node, tables, &table_count);

            for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
                if (child->type == XML_ELEMENT_NODE && isForeignKey(child)) {
                    addRelation((char *)node->name, (char *)child->name, child);
                }
            }
        }
    }

    for (int i = 0; i < relation_count; i++) {
        fprintf(output_file, "CREATE TABLE IF NOT EXISTS %s_%s (\n",
                relation_attributes[i].table_1, relation_attributes[i].table_2);
        fprintf(output_file, "    id_%s_%s INTEGER PRIMARY KEY,\n",
                relation_attributes[i].table_1, relation_attributes[i].table_2);
        fprintf(output_file, "    id_%s INTEGER,\n", relation_attributes[i].table_1);
        fprintf(output_file, "    id_%s INTEGER", relation_attributes[i].table_2);
        fprintf(output_file, ",\n    FOREIGN KEY (id_%s) REFERENCES %s(id_%s),\n",
                relation_attributes[i].table_1, relation_attributes[i].table_1, relation_attributes[i].table_1);
        fprintf(output_file, "    FOREIGN KEY (id_%s) REFERENCES %s(id_%s)\n",
                relation_attributes[i].table_2, relation_attributes[i].table_2, relation_attributes[i].table_2);
        fprintf(output_file, ");\n\n");
    }

    for (xmlNodePtr node = root->children; node != NULL; node = node->next) {
        if (node->type == XML_ELEMENT_NODE) {
            insertData(output_file, node, 0, NULL);
        }
    }
    fclose(output_file);
}

/*
 * @brief Analyse un fichier XML et déclenche la génération du SQL.
 * @param filename Nom du fichier XML.
 */
void parseXml(const char *filename) {
    xmlDocPtr doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Error: Couldn't read the file %s\n", filename);
        return;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        fprintf(stderr, "Error: The XML is empty\n");
        xmlFreeDoc(doc);
        return;
    }

    printf("Root element: %s\n", root->name);
    generateSql(root);

    xmlFreeDoc(doc);
    xmlCleanupParser();
}

/*
 * @brief Fonction principale.
 * @param argc Nombre d'arguments.
 * @param argv Tableau des arguments.
 * @return int Code de retour.
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.xml>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    parseXml(filename);

    const char *db_name = "my_database.db";
    const char *sql_file = "output.sql";
    return executeSqlFile(db_name, sql_file);
}
