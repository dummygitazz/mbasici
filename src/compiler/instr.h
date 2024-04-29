#ifndef INSTR_H
#define INSTR_H

typedef enum {
    OP_PUSH,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_RETRIEVE,
    OP_LOAD,
    OP_STORE,
    OP_UNCONDJUMP,
    OP_CONDJUMP,
    OP_CALL,
    OP_RETURN,
    OP_LEQ,
    OP_LESS,
    OP_EQUAL,
    OP_NEQ,
    OP_DECL,
    OP_SYSCALL,
    OP_MAKESTR,
    OP_MAKEINT,
    OP_MAKEFLT,
    OP_SUBSCRIPT,
} instr_op;

struct instr {
    instr_op type;
    union {
        int op;
        float f;
    };
};

struct instr_list {
    struct instr *instr;
    int len, cap;
};

struct instr_list* instr_list_create();
int instr_list_push(struct instr_list *list, struct instr instr);
void instr_list_print(struct instr_list *list);

struct instr push_instr(int val);
struct instr add_instr();
struct instr sub_instr();
struct instr mul_instr();
struct instr div_instr();
struct instr pow_instr();
struct instr retrieve_instr(int idx);
struct instr load_instr();
struct instr store_instr();
struct instr leq_instr();
struct instr less_instr();
struct instr equal_instr();
struct instr neq_instr();
struct instr uncondjump_instr(int addr);
struct instr condjump_instr(int addr);
struct instr call_instr(int addr);
struct instr return_instr();
struct instr decl_instr(int dims);
struct instr syscall_instr(int type);
struct instr makestr_instr(int len);
struct instr makeint_instr(int i);
struct instr makeflt_instr(float f);
struct instr subscript_instr();
#endif