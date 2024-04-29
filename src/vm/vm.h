#ifndef VM_H
#define VM_H

#include "compiler/instr.h"
#include "mem.h"

struct vm {
    struct mem_stack *mem_stack;
    struct mem_table *mem_table;
    struct addr_stack *addr_stack;
    struct instr_list *instr;
    int pc;
};

struct vm* vm_create(struct instr_list* instr);
void vm_exec(struct vm *vm);

#endif