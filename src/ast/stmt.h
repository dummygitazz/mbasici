#ifndef STMT_H
#define STMT_H

#include "stmt.h"
#include "expr.h"

struct ast_stmt_let {
    struct ast_expr *var, *expr;
};

struct ast_stmt_def {
    char *name;
    struct ast_expr *param, *expr;
};

struct ast_go_arg_list {
    int value;
    struct ast_go_arg_list *next;
};

struct ast_stmt_if {
    enum { COND_LEQ, COND_LESS, COND_EQUAL, COND_NEQ } type;
    struct ast_expr *expr_l, *expr_r;
    struct ast_go_arg_list *args;
};

struct ast_stmt_go {
    enum { AST_GOTO, AST_GOSUB, AST_ON_GOTO } type;
    struct ast_expr *expr;
    struct ast_go_arg_list *args;
};

struct ast_stmt_for {
    struct ast_expr *var, *init, *limit, *increment;
    struct ast_stmt_list *body;
};

struct ast_stmt_next {
    struct ast_expr *var;
};

struct ast_expr_arg_list {
    struct ast_expr *expr;
    struct ast_expr_arg_list *next;
};

struct ast_stmt_print {
    struct ast_expr_arg_list *args;
};

struct ast_stmt_input {
    struct ast_expr_arg_list *args;    
};

struct ast_stmt_list;

struct ast_stmt {
    enum { LET_STMT, DEF_STMT, IF_STMT, GO_STMT, FOR_STMT, NEXT_STMT, PRINT_STMT, INPUT_STMT, RETURN_STMT, EMPTY_STMT } type;
    union {
        struct ast_stmt_let let_stmt;
        struct ast_stmt_def def_stmt;
        struct ast_stmt_if if_stmt;
        struct ast_stmt_go go_stmt;
        struct ast_stmt_for for_stmt;
        struct ast_stmt_next next_stmt;
        struct ast_stmt_print print_stmt;
        struct ast_stmt_input input_stmt;
    };
    int line, addr;
    struct ast_stmt_list *parent;
};

struct ast_stmt_list {
    struct ast_stmt *stmt;
    struct ast_stmt_list *next, *parent;
};

struct ast_stmt* ast_create_stmt_let(struct ast_expr *var, struct ast_expr *expr);
struct ast_stmt* ast_create_stmt_def(char *name, struct ast_expr *param, struct ast_expr *expr);
struct ast_go_arg_list* ast_create_go_arg_list(int value);
struct ast_stmt* ast_create_stmt_if(int type, struct ast_expr *expr_l, struct ast_expr *expr_r, struct ast_go_arg_list *arg);
struct ast_stmt* ast_create_stmt_go(int type, struct ast_expr *expr, struct ast_go_arg_list *args);
struct ast_stmt* ast_create_stmt_for(struct ast_expr *var, struct ast_expr *init, struct ast_expr *limit, 
                                        struct ast_expr *increment, struct ast_stmt_list *body);
struct ast_stmt* ast_create_stmt_next(struct ast_expr *var);
struct ast_stmt* ast_create_stmt_print(struct ast_expr_arg_list *args);
struct ast_stmt* ast_create_stmt_input(struct ast_expr_arg_list *args);
struct ast_stmt* ast_create_stmt_empty();

struct ast_stmt_list* ast_create_stmt_list(struct ast_stmt *stmt, struct ast_stmt_list *next);

struct ast_expr_arg_list* ast_create_expr_arg_list(struct ast_expr *expr);

#endif