#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compile.h"
#include "instr.h"
#include "ast/expr.h"
#include "ast/stmt.h"
#include "symtable.h"
#include "util/err.h"

struct compiler* compile(struct ast_stmt_list *list, char **lines){
    struct compiler *ret = malloc(sizeof(struct compiler));
    ret->table = sym_table_create();
    ret->instr = instr_list_create();
    ret->goto_len = 0;
    ret->lines = lines;
    ret->error = 0;
    build_sym_table(ret, list);

    if(ret->error){
        return ret;
    }
    for(int i = 0; i < ret->table->size; i++){
        switch(ret->table->symbols[i].type){
            case NUM_TYPE:
                instr_list_push(ret->instr, decl_instr(0));
                break;
            case ARR_1D_TYPE:
                instr_list_push(ret->instr, push_instr(110000));
                instr_list_push(ret->instr, decl_instr(1));
                break;
            case ARR_2D_TYPE:
                instr_list_push(ret->instr, push_instr(11));
                instr_list_push(ret->instr, push_instr(11));
                instr_list_push(ret->instr, decl_instr(2));
                break;
            case STR_TYPE:
                instr_list_push(ret->instr, decl_instr(3));
                break;
            default:
                instr_list_push(ret->instr, decl_instr(0));
        }
    }

    compile_stmt_list(ret, list);

    for(int i = 0; i < ret->goto_len; i++){
        struct ast_stmt *stmt = ret->stmts[i];
        int jmp_to;
        if(stmt->type == IF_STMT){
            jmp_to = stmt->if_stmt.args->value;
        }else if(stmt->type == GO_STMT){
            jmp_to = stmt->go_stmt.args->value;
        }
        for(struct ast_stmt_list *body = stmt->parent; body != NULL; body = body->parent){
            for(struct ast_stmt_list *to = body; to != NULL; to = to->next){
                if(to->stmt->line == jmp_to){
                    ret->instr->instr[ret->goto_instrs[i]].op = to->stmt->addr;
                }
            }
        }
        
    }
    return ret;
}

void build_sym_table(struct compiler *c, struct ast_stmt_list *list){
    for(struct ast_stmt_list *l = list; l != NULL; l = l->next){
        struct ast_stmt *stmt = l->stmt;
        c->current_line = stmt->line;
        switch(stmt->type){
            case LET_STMT:
                build_sym_table_let(c, stmt);
                break;
            case DEF_STMT:
                build_sym_table_def(c, stmt);
                break;
            case IF_STMT:
                build_sym_table_if(c, stmt);
                break;
            case FOR_STMT:
                build_sym_table_for(c, stmt);
                break;
            case GO_STMT:
                build_sym_table_go(c, stmt);
                break;
            default:
                break;
        }
    }
}

void build_sym_table_for(struct compiler *c, struct ast_stmt *stmt){
    build_sym_table_expr(c, stmt->for_stmt.var);
    build_sym_table_expr(c, stmt->for_stmt.init);
    build_sym_table_expr(c, stmt->for_stmt.limit);
    build_sym_table_expr(c, stmt->for_stmt.increment);
    if(stmt->for_stmt.body){
        build_sym_table(c, stmt->for_stmt.body);
    }
}

void build_sym_table_let(struct compiler *c, struct ast_stmt *stmt){
    build_sym_table_expr(c, stmt->let_stmt.var);
    build_sym_table_expr(c, stmt->let_stmt.expr);
}

void build_sym_table_if(struct compiler *c, struct ast_stmt *stmt){
    build_sym_table_expr(c, stmt->if_stmt.expr_l);
    build_sym_table_expr(c, stmt->if_stmt.expr_r);
}

void build_sym_table_def(struct compiler *c, struct ast_stmt *stmt){
    build_sym_table_expr(c, stmt->def_stmt.expr); // expr first so calling self will error

    struct sym *sym = sym_table_find(c->table, stmt->def_stmt.name);
    if(sym == NULL){
        sym = sym_table_insert(c->table, stmt->def_stmt.name, FUNC_TYPE);
    }else{
        compile_error("Function '%s' is already defined", c->current_line, c->lines, stmt->def_stmt.name);
        c->error = 1;
    }
    if(stmt->def_stmt.param != NULL){
        sym->has_arg = 1;
        build_sym_table_expr(c, stmt->def_stmt.param);
    }
}

void build_sym_table_go(struct compiler *c, struct ast_stmt *stmt){ 
    if(stmt->go_stmt.type == AST_ON_GOTO){
        build_sym_table_expr(c, stmt->go_stmt.expr);
    }
}

void build_sym_table_expr(struct compiler *c, struct ast_expr *expr){
    if(expr == NULL) return;
    switch(expr->type){
        case AST_VAR: {
            struct sym *sym = sym_table_find(c->table, expr->var.name);
            if(sym == NULL){
                sym = sym_table_insert(c->table, expr->var.name, expr->var.type);
            }
            if(sym->type != expr->var.type){
                printf("ERROR dimension");
                //TODO type conflict
            }else{
                build_sym_table_expr(c, expr->var.arr_args[0]);
                build_sym_table_expr(c, expr->var.arr_args[1]);
            }
            break;
        }
        case AST_FUNC_CALL: {
            struct sym *sym = sym_table_find(c->table, expr->func_call.name);
            if(sym == NULL || sym->type != FUNC_TYPE){
                compile_error("Function '%s' is not defined", c->current_line, c->lines, expr->func_call.name);
                c->error = 1;
            }else{
                int arg = expr->func_call.param != NULL;
                if(arg != sym->has_arg){
                    compile_error("Function '%s' expects %d argument(s). Found %d instead", c->current_line, c->lines, expr->func_call.name, sym->has_arg, arg);
                    c->error = 1;
                }
                build_sym_table_expr(c, expr->func_call.param);
            }
            break;
        }
        case AST_BINARY_OP: {
            build_sym_table_expr(c, expr->binary_op.left);
            build_sym_table_expr(c, expr->binary_op.right);
            break;
        }
        default:
            break;
    }
}

void compile_binary_op(struct compiler *c, struct ast_expr *expr){
    compile_expr(c, expr->binary_op.right);
    compile_expr(c, expr->binary_op.left);
    switch(expr->binary_op.type){
        case BINARY_ADD:
            instr_list_push(c->instr, add_instr());
            break;
        case BINARY_SUB:
            instr_list_push(c->instr, sub_instr());
            break;      
        case BINARY_MUL:
            instr_list_push(c->instr, mul_instr());
            break;
        case BINARY_DIV:
            instr_list_push(c->instr, div_instr());
            break;         
        case BINARY_POW:
            instr_list_push(c->instr, pow_instr());
            break; 
        default:
            break;
    }
}

void compile_literal(struct compiler *c, struct ast_expr *expr){
    switch(expr->literal.type){
        case INT_LITERAL:
            instr_list_push(c->instr, makeint_instr(expr->literal.int_val));
            break;
        case STR_LITERAL:
            int len = strlen(expr->literal.str_val);
            for(int i = len - 1; i >= 0; i--){
                instr_list_push(c->instr, push_instr(expr->literal.str_val[i]));
            }
            instr_list_push(c->instr, makestr_instr(len));
            break;
        case FLOAT_LITERAL:
            instr_list_push(c->instr, makeflt_instr(expr->literal.float_val));
            break;
        default:
            break;
    }
}

void compile_var(struct compiler *c, struct ast_expr *expr, struct instr (*load_store_instr)()){
    int sym_idx = sym_table_find(c->table, expr->var.name) - c->table->symbols;

    switch(expr->var.type){
        case AST_VAR_STR:
        case AST_VAR_NUM:
            instr_list_push(c->instr, retrieve_instr(sym_idx));
            break;
        case AST_VAR_ARR_1D:
            compile_expr(c, expr->var.arr_args[0]);
            instr_list_push(c->instr, retrieve_instr(sym_idx));
            instr_list_push(c->instr, load_instr());
            instr_list_push(c->instr, subscript_instr());
            break;
        case AST_VAR_ARR_2D:
            compile_expr(c, expr->var.arr_args[1]);
            compile_expr(c, expr->var.arr_args[0]);
            instr_list_push(c->instr, retrieve_instr(sym_idx));
            instr_list_push(c->instr, load_instr());
            instr_list_push(c->instr, subscript_instr());
            instr_list_push(c->instr, load_instr());
            instr_list_push(c->instr, subscript_instr());           
            break;
        default:
            break;
    }
    if(load_store_instr){
        instr_list_push(c->instr, load_store_instr());
    }
}

void compile_func_call(struct compiler *c, struct ast_expr *expr){
    struct sym *sym = sym_table_find(c->table, expr->func_call.name);
    if(expr->func_call.param){
        compile_expr(c, expr->func_call.param);
    }
    instr_list_push(c->instr, call_instr(sym->addr));
}

void compile_expr(struct compiler *c, struct ast_expr *expr){
    switch(expr->type){
        case AST_LITERAL:
            compile_literal(c, expr);
            break;
        case AST_BINARY_OP:
            compile_binary_op(c, expr);
            break;
        case AST_VAR:
            compile_var(c, expr, load_instr);
            break;
        case AST_FUNC_CALL:
            compile_func_call(c, expr);
            break;
        default:
            break;
    }
}

void compile_stmt_let(struct compiler *c, struct ast_stmt *stmt){
    compile_expr(c, stmt->let_stmt.expr);
    compile_var(c, stmt->let_stmt.var, store_instr);
}

void compile_stmt_if(struct compiler *c, struct ast_stmt *stmt){
    compile_expr(c, stmt->if_stmt.expr_r);
    compile_expr(c, stmt->if_stmt.expr_l);
    switch(stmt->if_stmt.type){
        case COND_LEQ:
            instr_list_push(c->instr, leq_instr());
            break;
        case COND_LESS:
            instr_list_push(c->instr, less_instr());
            break;
        case COND_EQUAL:
            instr_list_push(c->instr, equal_instr());
            break;
        case COND_NEQ:
            instr_list_push(c->instr, neq_instr());
            break;
        default:
            break;
    }
    c->goto_instrs[c->goto_len] = instr_list_push(c->instr, condjump_instr(-1));
    c->stmts[c->goto_len++] = stmt;
}

void compile_stmt_def(struct compiler *c, struct ast_stmt *stmt){
    int jmp = instr_list_push(c->instr, uncondjump_instr(-1));
    sym_table_find(c->table, stmt->def_stmt.name)->addr = c->instr->len;
    if(stmt->def_stmt.param){
        compile_var(c, stmt->def_stmt.param, load_instr); // load old argument value
        instr_list_push(c->instr, retrieve_instr(0));
        instr_list_push(c->instr, store_instr()); // store to temp
        compile_var(c, stmt->def_stmt.param, store_instr); // load function argument to memory
        instr_list_push(c->instr, retrieve_instr(0));
        instr_list_push(c->instr, load_instr()); // load temp 
    }
    compile_expr(c, stmt->def_stmt.expr);
    if(stmt->def_stmt.param){
        instr_list_push(c->instr, retrieve_instr(0));
        instr_list_push(c->instr, store_instr()); // store function result to temp
        compile_var(c, stmt->def_stmt.param, store_instr); // store old argument value to memory
        instr_list_push(c->instr, retrieve_instr(0));
        instr_list_push(c->instr, load_instr()); // load function result from temp
    }
    instr_list_push(c->instr, return_instr());
    c->instr->instr[jmp].op = c->instr->len;
}

void compile_stmt_for(struct compiler *c, struct ast_stmt *stmt){
    compile_expr(c, stmt->for_stmt.init);
    compile_var(c, stmt->for_stmt.var, store_instr);

    int loop_start = c->instr->len;
    instr_list_push(c->instr, makeint_instr(0));
    compile_expr(c, stmt->for_stmt.increment);
    instr_list_push(c->instr, less_instr());
    int jmp_neg = instr_list_push(c->instr, condjump_instr(-1));
    compile_expr(c, stmt->for_stmt.limit);
    compile_expr(c, stmt->for_stmt.var);
    instr_list_push(c->instr, less_instr());
    int jmp_body_start_1 = instr_list_push(c->instr, condjump_instr(-1));
    int jmp_end_1 = instr_list_push(c->instr, uncondjump_instr(-1));
    c->instr->instr[jmp_neg].op = c->instr->len;

    compile_expr(c, stmt->for_stmt.var);
    compile_expr(c, stmt->for_stmt.limit);
    instr_list_push(c->instr, less_instr());
    int jmp_body_start_2 = instr_list_push(c->instr, condjump_instr(-1));
    int jmp_end_2 = instr_list_push(c->instr, uncondjump_instr(-1));

    c->instr->instr[jmp_body_start_1].op = c->instr->instr[jmp_body_start_2].op = c->instr->len;
    compile_stmt_list(c, stmt->for_stmt.body);

    compile_expr(c, stmt->for_stmt.increment);
    compile_expr(c, stmt->for_stmt.var);
    instr_list_push(c->instr, add_instr());

    compile_var(c, stmt->for_stmt.var, store_instr);
    
    instr_list_push(c->instr, uncondjump_instr(loop_start));
    c->instr->instr[jmp_end_1].op = c->instr->instr[jmp_end_2].op = c->instr->len;
}

void compile_stmt_go(struct compiler *c, struct ast_stmt *stmt){
    switch(stmt->go_stmt.type){
        case AST_GOTO:
            c->goto_instrs[c->goto_len] = instr_list_push(c->instr, uncondjump_instr(-1));
            c->stmts[c->goto_len++] = stmt;
            break;
        case AST_GOSUB:
            c->goto_instrs[c->goto_len] = instr_list_push(c->instr, call_instr(-1));
            c->stmts[c->goto_len++] = stmt;
            break;
    }
}

void compile_stmt_print(struct compiler *c, struct ast_stmt *stmt){
    int arg_count = 0;
    for(struct ast_expr_arg_list *arg_list = stmt->print_stmt.args; arg_list != NULL; arg_list = arg_list->next){
        compile_expr(c, arg_list->expr);
        arg_count++;
    }
    instr_list_push(c->instr, push_instr(arg_count));
    instr_list_push(c->instr, syscall_instr(0));
}

void compile_stmt_input(struct compiler *c, struct ast_stmt *stmt){
    int arg_count = 0;
    for(struct ast_expr_arg_list *arg_list = stmt->print_stmt.args; arg_list != NULL; arg_list = arg_list->next){
        compile_var(c, arg_list->expr, NULL);
        arg_count++;
    }
    instr_list_push(c->instr, push_instr(arg_count));
    instr_list_push(c->instr, syscall_instr(1));    
}

void compile_stmt_list(struct compiler *c, struct ast_stmt_list *list){
    for(struct ast_stmt_list *l = list; l != NULL; l = l->next){
        struct ast_stmt *stmt = l->stmt;
        stmt->addr = c->instr->len;
        stmt->parent = list;
        switch(stmt->type){
            case LET_STMT:
                compile_stmt_let(c, stmt);
                break;
            case IF_STMT:
                compile_stmt_if(c, stmt);
                break;
            case DEF_STMT:
                compile_stmt_def(c, stmt);
                break;
            case FOR_STMT:
                compile_stmt_for(c, stmt);
                stmt->for_stmt.body->parent = list;
                break;
            case GO_STMT:
                compile_stmt_go(c, stmt);
                break;
            case PRINT_STMT:
                compile_stmt_print(c, stmt);
                break;
            case INPUT_STMT:
                compile_stmt_input(c, stmt);
                break;
            case RETURN_STMT:
                instr_list_push(c->instr, return_instr());
                break;
            case EMPTY_STMT:
                stmt->addr = c->instr->len;
                break;

        }
    }
}