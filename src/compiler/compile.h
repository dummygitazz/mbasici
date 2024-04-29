#ifndef COMPILE_H
#define COMPILE_H

struct instr;
struct instr_list;
struct ast_expr;
struct sym_table;
struct ast_stmt_list;
struct ast_stmt;

struct compiler {
    struct instr_list *instr;
    struct sym_table *table;
    int goto_instrs[100000], goto_len;
    struct ast_stmt *stmts[100000];
    char **lines;
    int current_line;
    int error;
};

struct compiler* compile(struct ast_stmt_list *list, char **lines);

void build_sym_table(struct compiler *c, struct ast_stmt_list *list);
void build_sym_table_for(struct compiler *c, struct ast_stmt *stmt);
void build_sym_table_if(struct compiler *c, struct ast_stmt *stmt);
void build_sym_table_let(struct compiler *c, struct ast_stmt *stmt);
void build_sym_table_def(struct compiler *c, struct ast_stmt *stmt);
void build_sym_table_expr(struct compiler *c, struct ast_expr *expr);
void build_sym_table_go(struct compiler *c, struct ast_stmt *stmt);

void compile_binary_op(struct compiler *c, struct ast_expr *expr);
void compile_literal(struct compiler *c, struct ast_expr *expr);
void compile_var(struct compiler *c, struct ast_expr *expr, struct instr (*load_store_instr)());
void compile_expr(struct compiler *c, struct ast_expr *expr);


void compile_stmt_list(struct compiler *c, struct ast_stmt_list *list);
void compile_stmt_let(struct compiler *c, struct ast_stmt *stmt);
void compile_stmt_if(struct compiler *c, struct ast_stmt *stmt);
void compile_stmt_def(struct compiler *c, struct ast_stmt *stmt);
void compile_stmt_for(struct compiler *c, struct ast_stmt *stmt);
void compile_stmt_go(struct compiler *c, struct ast_stmt *stmt);
#endif