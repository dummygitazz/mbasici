#ifndef SYMTABLE_H
#define SYMTABLE_H

typedef enum {
    NUM_TYPE,
    ARR_1D_TYPE,
    ARR_2D_TYPE,
    STR_TYPE,
    FUNC_TYPE
} sym_table_types;

struct sym {
    sym_table_types type;
    char name[4];
    int dimensions[2], addr;
    int has_arg;
};

struct sym_table {
    struct sym symbols[100];
    int size;
};

struct sym_table* sym_table_create();
struct sym* sym_table_find(struct sym_table *table, char *name);
struct sym* sym_table_insert(struct sym_table *table, char *name, sym_table_types type);
#endif