#include <stdlib.h>
#include <stdio.h>
#include "instr.h"

struct instr_list* instr_list_create(){
    struct instr_list *ret = malloc(sizeof(struct instr_list));
    ret->instr = NULL;
    ret->len = 0;
    ret->cap = 0;
    return ret;
}

int instr_list_push(struct instr_list *list, struct instr instr){
    if(list->instr == NULL){
        list->instr = malloc(sizeof(struct instr));
        list->cap = 1;
    }else if(list->cap <= list->len){
        list->instr = realloc(list->instr, sizeof(instr) * list->cap * 2);
        list->cap *= 2;
    }
    list->instr[list->len++] = instr;
    return list->len - 1;
}

void instr_list_print(struct instr_list *list){
    for(int i = 0; i < list->len; i++){
        switch(list->instr[i].type){
            case OP_ADD:
                printf("ADD");
                break;
            case OP_SUB:
                printf("SUB");
                break;
            case OP_MUL:
                printf("MUL");
                break;
            case OP_DIV:
                printf("DIV");
                break;
            case OP_POW:
                printf("POW");
                break;
            case OP_PUSH:
                printf("PUSH %d", list->instr[i].op);
                break;
            case OP_LOAD:
                printf("LOAD %d", list->instr[i].op);
                break;
            case OP_STORE:
                printf("STORE %d", list->instr[i].op);
                break;
            case OP_LEQ:
                printf("LEQ");
                break;
            case OP_LESS:
                printf("LESS");
                break;
            case OP_EQUAL:
                printf("EQUAL");
                break;
            case OP_NEQ:
                printf("NEQ");
                break;
            case OP_UNCONDJUMP:
                printf("UNCONDJUMP %d", list->instr[i].op);
                break;
            case OP_CONDJUMP:
                printf("CONDJUMP %d", list->instr[i].op);
                break;
            case OP_RETURN:
                printf("RETURN");
                break;
            case OP_CALL:
                printf("CALL %d", list->instr[i].op);
                break;
            case OP_RETRIEVE:
                printf("RETRIEVE %d", list->instr[i].op);
                break;
            case OP_DECL:
                printf("DECL %d", list->instr[i].op);
                break;
            case OP_SYSCALL:
                printf("SYSCALL %d", list->instr[i].op);
                break;
            case OP_MAKESTR:
                printf("MAKESTR %d", list->instr[i].op);
                break;
        }
        printf("\n");
    }
}

struct instr push_instr(int val){
    return (struct instr) {.type = OP_PUSH, .op = val};
}
struct instr add_instr(){
    return (struct instr) {.type = OP_ADD};
}
struct instr sub_instr(){
    return (struct instr) {.type = OP_SUB};
}
struct instr mul_instr(){
    return (struct instr) {.type = OP_MUL};
}
struct instr div_instr(){
    return (struct instr) {.type = OP_DIV};
}
struct instr pow_instr(){
    return (struct instr) {.type = OP_POW};
}
struct instr retrieve_instr(int idx){
    return (struct instr) {.type = OP_RETRIEVE, .op = idx};
}
struct instr load_instr(){
    return (struct instr) {.type = OP_LOAD};
}
struct instr store_instr(){
    return (struct instr) {.type = OP_STORE};
}
struct instr leq_instr(){
    return (struct instr) {.type = OP_LEQ};
}
struct instr less_instr(){
    return (struct instr) {.type = OP_LESS};
}
struct instr equal_instr(){
    return (struct instr) {.type = OP_EQUAL};
}
struct instr neq_instr(){
    return (struct instr) {.type = OP_NEQ};    
}
struct instr uncondjump_instr(int addr){
    return (struct instr) {.type = OP_UNCONDJUMP, .op = addr};
}
struct instr condjump_instr(int addr){
    return (struct instr) {.type = OP_CONDJUMP, .op = addr};
}
struct instr call_instr(int addr){
    return (struct instr) {.type = OP_CALL, .op = addr};
}
struct instr return_instr(){
    return (struct instr) {.type = OP_RETURN};
}
struct instr decl_instr(int dims){
    return (struct instr) {.type = OP_DECL, .op = dims};
}
struct instr syscall_instr(int type){
    return (struct instr) {.type = OP_SYSCALL, .op = type};
}
struct instr makestr_instr(int len){
    return (struct instr) {.type = OP_MAKESTR, .op = len};
}
struct instr makeint_instr(int i){
    return (struct instr) {.type = OP_MAKEINT, .op = i};
}
struct instr makeflt_instr(float f){
    return (struct instr) {.type = OP_MAKEFLT, .f = f};
}
struct instr subscript_instr(){
    return (struct instr) {.type = OP_SUBSCRIPT};
}