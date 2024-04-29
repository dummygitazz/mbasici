#include <stdlib.h>
#include "stmt.h"

struct ast_stmt* ast_create_stmt_let(struct ast_expr *var, struct ast_expr *expr){
    struct ast_stmt *ret = malloc(sizeof(struct ast_stmt));
    ret->type = LET_STMT;
    ret->let_stmt.var = var;
    ret->let_stmt.expr = expr;
    return ret;
}

struct ast_stmt* ast_create_stmt_def(char *name, struct ast_expr *param, struct ast_expr *expr){
    struct ast_stmt *ret = malloc(sizeof(struct ast_stmt));
    ret->type = DEF_STMT;
    ret->def_stmt.name = name;
    ret->def_stmt.param = param;
    ret->def_stmt.expr = expr;
    return ret;
}

struct ast_go_arg_list* ast_create_go_arg_list(int value){
    struct ast_go_arg_list *ret = malloc(sizeof(struct ast_go_arg_list));
    ret->value = value;
    ret->next = NULL;
    return ret;
}

struct ast_stmt* ast_create_stmt_if(int type, struct ast_expr *expr_l, struct ast_expr *expr_r, struct ast_go_arg_list *arg){
    struct ast_stmt *ret = malloc(sizeof(struct ast_stmt));
    ret->type = IF_STMT;
    ret->if_stmt.type = type;
    ret->if_stmt.expr_l = expr_l;
    ret->if_stmt.expr_r = expr_r;
    ret->if_stmt.args = arg;
    return ret;
}

struct ast_stmt* ast_create_stmt_go(int type, struct ast_expr *expr, struct ast_go_arg_list *args){
    struct ast_stmt *ret = malloc(sizeof(struct ast_stmt));
    ret->type = GO_STMT;
    ret->go_stmt.type = type;
    ret->go_stmt.expr = expr;
    ret->go_stmt.args = args;
    return ret;
}

struct ast_stmt_list* ast_create_stmt_list(struct ast_stmt *stmt, struct ast_stmt_list *next){
    struct ast_stmt_list* ret = malloc(sizeof(struct ast_stmt_list));
    ret->stmt = stmt;
    ret->next = next;
    ret->parent = NULL;
    return ret;
}

struct ast_stmt* ast_create_stmt_for(struct ast_expr *var, struct ast_expr *init, struct ast_expr *limit, 
                                        struct ast_expr *increment, struct ast_stmt_list *body){
    struct ast_stmt *ret = malloc(sizeof(struct ast_stmt));
    ret->type = FOR_STMT;
    ret->for_stmt.var = var;
    ret->for_stmt.init = init;
    ret->for_stmt.limit = limit;
    ret->for_stmt.body = body;
    if(increment == NULL){
        increment = ast_create_literal(INT_LITERAL);
        increment->literal.int_val = 1;
    }
    ret->for_stmt.increment = increment;
    return ret;
}

struct ast_stmt* ast_create_stmt_next(struct ast_expr *var){
    struct ast_stmt *ret = malloc(sizeof(struct ast_stmt));
    ret->next_stmt.var = var;    
    return ret;
}

struct ast_expr_arg_list* ast_create_expr_arg_list(struct ast_expr *expr){
    struct ast_expr_arg_list *ret = malloc(sizeof(struct ast_expr_arg_list));
    ret->expr = expr;
    ret->next = NULL;
    return ret;
}

struct ast_stmt* ast_create_stmt_print(struct ast_expr_arg_list *args){
    struct ast_stmt *ret = malloc(sizeof(struct ast_stmt));
    ret->type = PRINT_STMT;
    ret->print_stmt.args = args;
    return ret;
}

struct ast_stmt* ast_create_stmt_input(struct ast_expr_arg_list *args){
    struct ast_stmt *ret = malloc(sizeof(struct ast_stmt));
    ret->type = INPUT_STMT;
    ret->input_stmt.args = args;
    return ret;
}

struct ast_stmt* ast_create_stmt_empty(){
    struct ast_stmt *ret = malloc(sizeof(struct ast_stmt));
    ret->type = EMPTY_STMT;
    return ret;    
}