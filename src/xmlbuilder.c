#include "../include/xmlbuilder.h"


RelationAttributes relation_attributes[MAX_RELATIONS];
TableID id_counters[MAX_TABLES];
char *xml_file_path = NULL;
int relation_count = 0;
int id_counter_count = 0;

int isWhitespaceOnly(const char *str) {
    while (*str) {
        if (*str != ' ' && *str != '\n' && *str != '\t' && *str != '\r') {
            return 0;
        }
        str++;
    }
    return 1;
}

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

int tableExists(const char *table_name, char tables[MAX_TABLES][MAX_TABLE_NAME], int table_count) {
    for (int i = 0; i < table_count; i++) {
        if (strcmp(tables[i], table_name) == 0) {
            return 1;
        }
    }
    return 0;
}

int isForeignKey(xmlNodePtr node) {
    for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE) {
            return 1;
        }
    }
    return 0;
}

int hasChildElements(xmlNodePtr node) {
    for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE) {
            return 1;
        }
    }
    return 0;
}

int executeSqlFile(const char *db_name, const char *sql_file) {
    sqlite3 *db;
    char *err_msg = 0;
    FILE *file;
    char *sql;
    long length;

    if (sqlite3_open(db_name, &db) != SQLITE_OK) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Impossible d'ouvrir la base de données : %s", sqlite3_errmsg(db));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 1;
    }

    file = fopen(sql_file, "r");
    if (!file) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Impossible d'ouvrir le fichier SQL : %s", sql_file);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        sqlite3_close(db);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);

    sql = malloc(length + 1);
    if (sql == NULL) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Erreur : l'allocation de mémoire a échoué.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        fclose(file);
        sqlite3_close(db);
        return 1;
    }

    fread(sql, 1, length, file);
    sql[length] = '\0';
    fclose(file);

    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Execution SQL échoué : %s", err_msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        sqlite3_free(err_msg);
        free(sql);
        sqlite3_close(db);
        return 1;
    }

    free(sql);
    sqlite3_close(db);

    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Execution du fichier SQL avec succés.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return 0;
}

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

void generateSql(xmlNodePtr root) {
    if (root == NULL) {
        return;
    }

    FILE *output_file = fopen("output.sql", "w");
    if (output_file == NULL) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Erreur: Impossible d'ouvrir output.sql pour l'écrire.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
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

void parseXml(const char *filename) {
    xmlDocPtr doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL) {        
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Impossible de lire le fichier : %s\n", filename);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Erreur: Le XML est vide.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        xmlFreeDoc(doc);
        return;
    }
    
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Root element: %s\n", root->name);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    generateSql(root);

    xmlFreeDoc(doc);
    xmlCleanupParser();
}

void xmlbuilder_confirm(GtkButton *button, gpointer user_data) {
    GtkWidget *entry_file = GTK_WIDGET(user_data);

    const char *file_path = gtk_entry_get_text(GTK_ENTRY(entry_file));

    if (strstr(file_path, ".xml") != NULL) {
        //xml_file_path = file_path;
        g_free(xml_file_path);
        xml_file_path = g_strdup(file_path);

        parseXml(xml_file_path);

        const char *db_name = "my_database.db";
        const char *sql_file = "output.sql";
        executeSqlFile(db_name, sql_file);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Le fichier doit être un fichier XML.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

void xmlbuilder(GtkWidget *widget, gpointer data) {
    const char *title = gtk_button_get_label(GTK_BUTTON(widget));
    int button_index = GPOINTER_TO_INT(data);

    if (open_windows[button_index] != NULL) {
        gtk_window_present(GTK_WINDOW(open_windows[button_index]));
        return;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    open_windows[button_index] = window;

    g_signal_connect(window, "destroy", G_CALLBACK(destroyWindow), GINT_TO_POINTER(button_index));

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 10);

    GtkWidget *space1 = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space1, 0, 1, 2, 1);

    GtkWidget *label_file = gtk_label_new("Fichier xml :");
    gtk_grid_attach(GTK_GRID(grid), label_file, 0, 2, 2, 1);

    GtkWidget *entry_file = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_file), "Chemin du fichier");
    gtk_widget_set_size_request(entry_file, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), entry_file, 0, 3, 2, 1);

    GtkWidget *space2 = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space2, 0, 4, 2, 1);

    GtkWidget *xmlbuilder_button = gtk_button_new_with_label("Confirmer");
    gtk_widget_set_size_request(xmlbuilder_button, 300, 30);
    gtk_grid_attach(GTK_GRID(grid), xmlbuilder_button, 0, 5, 2, 1);

    g_signal_connect(xmlbuilder_button, "clicked", G_CALLBACK(xmlbuilder_confirm), entry_file);

    gtk_widget_show_all(window);
}

