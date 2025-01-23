#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>

typedef struct {
    char *hour;
    char *note;
    char *date;
} AgendaData;

typedef struct {
    AgendaData *agenda;
    size_t agenda_count;
    char **history;
    size_t history_count;
    char *note;
} DatabaseData;

extern DatabaseData databaseData;

int syncDatabase();

#endif