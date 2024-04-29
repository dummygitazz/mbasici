#ifndef EXPR_H
#define EXPR_H

struct ast_literal {
    enum { INT_LITERAL, STR_LITERAL, FLOAT_LITERAL } type;
    union {
        int int_val;
        float float_val;
        char *str_val;
    };
};

struct ast_binary_op {
    enum { BINARY_ADD, BINARY_SUB, BINARY_MUL, BINARY_DIV, BINARY_POW } type;
    struct ast_expr *left, *right;
};

struct ast_var {
    enum { AST_VAR_NUM, AST_VAR_ARR_1D, AST_VAR_ARR_2D, AST_VAR_STR } type;
    char *name;
    struct ast_expr *arr_args[2];
};

struct ast_func_call {
    char *name;
    struct ast_expr *param;
};

struct ast_expr {
    enum { AST_LITERAL, AST_BINARY_OP, AST_VAR, AST_FUNC_CALL } type;
    union {
        struct ast_literal literal;
        struct ast_binary_op binary_op;
        struct ast_var var;
        struct ast_func_call func_call;
    };
};  

struct ast_expr* ast_create_literal(int type);
struct ast_expr* ast_create_binary_op(int type, struct ast_expr *left, struct ast_expr *right);
struct ast_expr* ast_create_var(int type, char *name, struct ast_expr *arg1, struct ast_expr *arg2);
struct ast_expr* ast_create_func_call(char *name, struct ast_expr *param);

#endif