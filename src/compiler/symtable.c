#include <stdlib.h>
#include <string.h>
#include "symtable.h"

struct sym_table* sym_table_create(){
    struct sym_table *ret = malloc(sizeof(struct sym_table));
    ret->size = 0;
    sym_table_insert(ret, "TMP", NUM_TYPE);
    return ret;
}

struct sym* sym_table_find(struct sym_table *table, char *name){
    for(int i = 0; i < table->size; i++){
        if(!strcmp(table->symbols[i].name, name)){
            return &table->symbols[i];
        }
    }
    return NULL;
}

struct sym* sym_table_insert(struct sym_table *table, char *name, sym_table_types type){
    int idx = table->size;
    table->symbols[idx].type = type;
    strcpy(table->symbols[idx].name, name);
    table->size++;
    return &table->symbols[idx];
}