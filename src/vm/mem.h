#ifndef MEM_H
#define MEM_H

struct obj;

struct mem_stack_type{
    enum { INT_TYPE, OBJ_TYPE, PTR_OBJ_TYPE } type;
    union {
        int int_val;
        struct obj *obj;
        struct obj **ptr_to_obj;
    };
};

struct mem_stack {
    struct mem_stack_type *stk;
    int len, cap;
};

struct mem_table {
    struct obj *vals[100];
    int len;
};

#define ADDR_STACK_MAX 1000

struct addr_stack {
    int stk[ADDR_STACK_MAX];
    int len;
};

struct mem_stack* mem_stack_create();
struct mem_stack_type mem_stack_pop(struct mem_stack *mem_stack);




void mem_stack_push_int(struct mem_stack *mem_stack, int i);
void mem_stack_push_obj(struct mem_stack *mem_stack, struct obj *obj);
void mem_stack_push_ptr_to_obj(struct mem_stack *mem_stack, struct obj **ptr_to_obj);

struct mem_table* mem_table_create();
void mem_table_insert(struct mem_table *mem_table, int type, int dim1, int dim2);
struct obj** mem_table_access(struct mem_table *mem_table, int idx);

struct addr_stack* addr_stack_create();
void addr_stack_push(struct addr_stack *addr_stack, int addr);
int addr_stack_pop(struct addr_stack *addr_stack);

#endif