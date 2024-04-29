%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
%}

%code requires {
    #include "ast/expr.h"
    #include "ast/stmt.h"
    #include "util/err.h"
    struct parser {
        char **lines;
        int error;  
        struct ast_stmt_list *program;
    };
}

%param { struct parser *p }

%code provides {
    void yyerror(struct parser *p, const char* message);
    #define YY_DECL \
        int yylex(struct parser *p)
    YY_DECL;
    struct parser *parse(FILE *file); 
}

%union {
    int int_val;
    float float_val;
    char *str_val;
    struct ast_expr *expr;
    struct ast_stmt *stmt;
    struct ast_stmt_list *stmt_list;
    struct ast_go_arg_list *goto_arg_list;
    struct ast_expr_arg_list *expr_arg_list;
}

%token GOTO GOSUB ON IF THEN LET DEF FOR TO STEP NEXT PRINT INPUT RETURN;
%token NEQ LEQ GEQ 
%token <int_val> NUMBER
%token <float_val> FLOAT
%token <str_val> NUM_VAR STR_VAR FUNC_VAR STRING

%type <expr> expr var num-var simp-num-var arr-num-var str-var
%type <goto_arg_list> goto-arg-list goto-arg
%type <expr_arg_list> expr-arg-list
%type <stmt> stmt let-stmt if-stmt go-stmt for-stmt def-stmt next-stmt print-stmt input-stmt
%type <stmt_list> stmt-list

%left '+' '-'
%left '*' '/'
%left '^'
%nonassoc UMINUS


%%

program:
    stmt-list {
        p->program = $1;
    }
var:
    num-var {
        $$ = $1;
    }
    | str-var {
        $$ = $1;
    }

num-var:
    simp-num-var {
        $$ = $1;
    }
    | arr-num-var {
        $$ = $1;
    }

simp-num-var:
    NUM_VAR {
        $$ = ast_create_var(AST_VAR_NUM, $1, NULL, NULL);
    }

arr-num-var:
    NUM_VAR '(' expr ')' {
        $$ = ast_create_var(AST_VAR_ARR_1D, $1, $3, NULL);
    }
    | NUM_VAR '(' expr ',' expr ')' {
        $$ = ast_create_var(AST_VAR_ARR_2D, $1, $3, $5);
    }

str-var:
    STR_VAR {
        $$ = ast_create_var(AST_VAR_STR, $1, NULL, NULL);
    }

expr:
    expr '+' expr {
        $$ = ast_create_binary_op(BINARY_ADD, $1, $3);
    }
    | expr '-' expr {
        $$ = ast_create_binary_op(BINARY_SUB, $1, $3);
    }
    | expr '*' expr {
        $$ = ast_create_binary_op(BINARY_MUL, $1, $3);
    }
    | expr '/' expr {
        $$ = ast_create_binary_op(BINARY_DIV, $1, $3);
    } 
    | expr '^' expr {
        $$ = ast_create_binary_op(BINARY_POW, $1, $3);        
    }
    | '-' expr %prec UMINUS {
        struct ast_expr *tmp = ast_create_literal(INT_LITERAL);
        tmp->literal.int_val = 0;
        $$ = ast_create_binary_op(BINARY_SUB, tmp, $2);         
    }
    | '(' expr ')' {
        $$ = $2;
    }
    | NUMBER {
        $$ = ast_create_literal(INT_LITERAL);
        $$->literal.int_val = $1;
    }
    | FLOAT {
        $$ = ast_create_literal(FLOAT_LITERAL);
        $$->literal.float_val = $1;
    }
    | STRING {
        $$ = ast_create_literal(STR_LITERAL);
        $$->literal.str_val = $1;
    }
    | FUNC_VAR '(' expr ')' {
        $$ = ast_create_func_call($1, $3);
    }
    | FUNC_VAR {
        $$ = ast_create_func_call($1, NULL);
    }
    | num-var {
        $$ = $1;
    }
    | str-var {
        $$ = $1;
    }
stmt-list:
    stmt stmt-list {
        $1->line = @1.first_line;
        $$ = ast_create_stmt_list($1, $2);
    }
    | %empty {
        $$ = NULL;
    }
stmt:
    go-stmt '\n' {
        $$ = $1;
    }
    | if-stmt '\n' {
        $$ = $1;
    }
    | let-stmt '\n' {
        $$ = $1;
    }
    | def-stmt '\n' {
        $$ = $1;
    }
    | for-stmt '\n' {
        $$ = $1;
    }
    | print-stmt '\n' {
        $$ = $1;
    }
    | input-stmt '\n' {
        $$ = $1;
    } 
    | RETURN {
        $$ = ast_create_stmt_empty();
        $$->type = RETURN_STMT;
    }
    | '\n' {
        $$ = ast_create_stmt_empty();
    }
    | error '\n' {
        $$ = ast_create_stmt_empty();
        compile_error("Invalid statement", @$.first_line, p->lines);
    }
go-stmt:
    GOTO goto-arg {
        $$ = ast_create_stmt_go(AST_GOTO, NULL, $2);
    }
    | GOSUB goto-arg {
        $$ = ast_create_stmt_go(AST_GOSUB, NULL, $2);
    }
    | ON expr GOTO goto-arg-list {
        $$ = ast_create_stmt_go(AST_ON_GOTO, $2, $4);
    }

if-stmt:
    IF expr '<' expr THEN goto-arg {
        $$ = ast_create_stmt_if(COND_LESS, $2, $4, $6);
    }
    | IF expr LEQ expr THEN goto-arg {
        $$ = ast_create_stmt_if(COND_LEQ, $2, $4, $6);
    }
    | IF expr '>' expr THEN goto-arg {
        $$ = ast_create_stmt_if(COND_LESS, $4, $2, $6);
    }
    | IF expr GEQ expr THEN goto-arg {
        $$ = ast_create_stmt_if(COND_LESS, $4, $2, $6);
    }
    | IF expr '=' expr THEN goto-arg {
        $$ = ast_create_stmt_if(COND_EQUAL, $2, $4, $6);
    }
    | IF expr NEQ expr THEN goto-arg {
        $$ = ast_create_stmt_if(COND_NEQ, $2, $4, $6);
    }

let-stmt:
    LET var '=' expr {

        $$ = ast_create_stmt_let($2, $4);
    }

def-stmt:
    DEF FUNC_VAR '=' expr {
        $$ = ast_create_stmt_def($2, NULL, $4);
    }
    | DEF FUNC_VAR '(' simp-num-var ')' '=' expr {
        $$ = ast_create_stmt_def($2, $4, $7);
    }

for-stmt:
    FOR simp-num-var '=' expr TO expr '\n' stmt-list next-stmt {
        $$ = ast_create_stmt_for($2, $4, $6, NULL, $8);
    }
    | FOR simp-num-var '=' expr TO expr STEP expr '\n' stmt-list next-stmt {
        $$ = ast_create_stmt_for($2, $4, $6, $8, $10);
    }

next-stmt:
    NEXT simp-num-var {
        $$ = ast_create_stmt_next($2);
    }

print-stmt: 
    PRINT expr-arg-list {
        $$ = ast_create_stmt_print($2);
    }
    | PRINT {
        $$ = ast_create_stmt_print(NULL);
    }

input-stmt:
    INPUT expr-arg-list {
        $$ = ast_create_stmt_input($2);
    }
    
expr-arg-list:
    expr-arg-list ';' expr {
        $$ = ast_create_expr_arg_list($3);
        $$->next = $1;
    }
    | expr-arg-list ',' expr {
        $$ = ast_create_expr_arg_list($3);
        $$->next = $1;
    }
    | expr {
        $$ = ast_create_expr_arg_list($1);
    }

goto-arg-list: 
    goto-arg {
        $$ = $1;
    }
    | goto-arg ',' goto-arg-list {
        $$ = $1;
        $$->next = $3;
    }

goto-arg:
    NUMBER {
        $$ = ast_create_go_arg_list($1);
    }
%%

void yyerror(struct parser *p, const char *s){
    p->error = 1;
}

struct parser* parse(FILE *file){
    extern FILE *yyin;
    yyin = file;
    int num_lines = 1;
    char x;
    char buff[100] = {0};
    while((x = fgetc(file)) != EOF){
        if(x == '\n'){
            num_lines++;
        }
    }
    struct parser *p = malloc(sizeof(struct parser));
    p->lines = malloc(sizeof(char*) * (num_lines + 1));
    p->error = 0;
    fseek(file, 0, SEEK_SET);
    int line = 1;
    while(fgets(buff, sizeof(buff), file) != NULL){
        int ptr = 0;
        while(buff[ptr] == '\t' || buff[ptr] == ' '){
            ptr++;
        }
        p->lines[line] = malloc(sizeof(char) * (strlen(buff + ptr) + 1));
        buff[strcspn(buff, "\n")] = 0;
        strcpy(p->lines[line], buff + ptr);
        line++;
    }
    fseek(file, 0, SEEK_SET);
    yyparse(p);
    return p;
}