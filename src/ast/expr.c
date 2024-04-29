#include <stdlib.h>
#include "expr.h"

struct ast_expr* ast_create_literal(int type){
    struct ast_expr *ret = malloc(sizeof(struct ast_expr));
    ret->type = AST_LITERAL;
    ret->literal.type = type;
    return ret;
}

struct ast_expr* ast_create_binary_op(int type, struct ast_expr *left, struct ast_expr *right){
    struct ast_expr *ret = malloc(sizeof(struct ast_expr));
    ret->type = AST_BINARY_OP;
    ret->binary_op.type = type;
    ret->binary_op.left = left;
    ret->binary_op.right = right;
    return ret;
}

struct ast_expr* ast_create_var(int type, char *name, struct ast_expr *arg1, struct ast_expr *arg2){
    struct ast_expr *ret = malloc(sizeof(struct ast_expr));    
    ret->type = AST_VAR;
    ret->var.type = type;
    ret->var.name = name;
    ret->var.arr_args[0] = arg1;
    ret->var.arr_args[1] = arg2;
    return ret;
}

struct ast_expr* ast_create_func_call(char *name, struct ast_expr *param){
    struct ast_expr *ret = malloc(sizeof(struct ast_expr));  
    ret->type = AST_FUNC_CALL;
    ret->func_call.name = name;
    ret->func_call.param = param;    
    return ret;
}