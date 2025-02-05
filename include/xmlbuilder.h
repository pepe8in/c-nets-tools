#ifndef XMLBUILDER_H
#define XMLBUILDER_H

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <sqlite3.h>
#include <gtk/gtk.h>
#include "interface.h"

#define MAX_TABLE_NAME 256
#define MAX_TABLES 100
#define MAX_RELATIONS 100

typedef struct {
    char table_1[256];
    char table_2[256];
    char attributes[10][256];
    int attr_count;
} RelationAttributes;

typedef struct {
    char table_name[256];
    int current_id;
} TableID;

extern char *xml_file_path;
extern int relation_count;
extern int id_counter_count;

int isWhitespaceOnly(const char *str);
void escapeSqlString(const char *input, char *output, size_t max_size);
int getNewId(const char *table_name);
int tableExists(const char *table_name, char tables[MAX_TABLES][MAX_TABLE_NAME], int table_count);
int isForeignKey(xmlNodePtr node);
int hasChildElements(xmlNodePtr node);
int executeSqlFile(const char *db_name, const char *sql_file);
void addRelation(const char *table1, const char *table2, xmlNodePtr relation_node);
void createTable(FILE *output_file, xmlNodePtr node, char tables[MAX_TABLES][MAX_TABLE_NAME], int *table_count);
void insertData(FILE *output_file, xmlNodePtr node, int parent_id, const char *parent_table);
void generateSql(xmlNodePtr root);
void parseXml(const char *filename);
void xmlbuilder_confirm(GtkButton *button, gpointer user_data);
void xmlbuilder(GtkWidget *widget, gpointer data);

#endif
