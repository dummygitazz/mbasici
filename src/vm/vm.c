#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "obj.h"
#include "vm.h"
#include "mem.h"
#include "compiler/instr.h"
#include "syscall.h"

struct vm* vm_create(struct instr_list* instr){
    struct vm *vm = malloc(sizeof(struct vm));
    vm->mem_stack = mem_stack_create();
    vm->mem_table = mem_table_create();
    vm->addr_stack = addr_stack_create();
    vm->instr = instr;
    vm->pc = 0;
    return vm;
}

static void vm_exec_decl(struct vm *vm, struct instr instr){
    switch(instr.op){
        case 0:
        case 3:
            mem_table_insert(vm->mem_table, instr.op, 0, 0);
            break;
        case 1: {
            int col = mem_stack_pop(vm->mem_stack).int_val;
            mem_table_insert(vm->mem_table, 1, 0, col);
            break;
        }
        case 2: {
            int row = mem_stack_pop(vm->mem_stack).int_val;
            int col = mem_stack_pop(vm->mem_stack).int_val;
            mem_table_insert(vm->mem_table, 2, row, col);
            break;
        }
        default:
            break;
    }
}

static void vm_exec_push(struct vm *vm, struct instr instr){
    mem_stack_push_int(vm->mem_stack, instr.op);
}

static void vm_exec_add(struct vm *vm, struct instr instr){
    struct obj *left = mem_stack_pop(vm->mem_stack).obj;
    struct obj *right = mem_stack_pop(vm->mem_stack).obj;
    int float_res = left->num.type == FLOAT_TYPE || right->num.type == FLOAT_TYPE;
    if(!float_res){
        mem_stack_push_obj(vm->mem_stack, obj_create_num_int(left->num.int_val + right->num.int_val));
    }else{
        mem_stack_push_obj(vm->mem_stack, obj_create_num_float(obj_get_num(left) + obj_get_num(right)));        
    }
}

static void vm_exec_sub(struct vm *vm, struct instr instr){
    struct obj *left = mem_stack_pop(vm->mem_stack).obj;
    struct obj *right = mem_stack_pop(vm->mem_stack).obj;
    int float_res = left->num.type == FLOAT_TYPE || right->num.type == FLOAT_TYPE;
    if(!float_res){
        mem_stack_push_obj(vm->mem_stack, obj_create_num_int(left->num.int_val - right->num.int_val));
    }else{
        mem_stack_push_obj(vm->mem_stack, obj_create_num_float(obj_get_num(left) - obj_get_num(right)));        
    }
}

static void vm_exec_mul(struct vm *vm, struct instr instr){
    struct obj *left = mem_stack_pop(vm->mem_stack).obj;
    struct obj *right = mem_stack_pop(vm->mem_stack).obj;
    int float_res = left->num.type == FLOAT_TYPE || right->num.type == FLOAT_TYPE;
    if(!float_res){
        mem_stack_push_obj(vm->mem_stack, obj_create_num_int(left->num.int_val * right->num.int_val));
    }else{
        mem_stack_push_obj(vm->mem_stack, obj_create_num_float(obj_get_num(left) * obj_get_num(right)));        
    }
}

static void vm_exec_div(struct vm *vm, struct instr instr){
    struct obj *left = mem_stack_pop(vm->mem_stack).obj;
    struct obj *right = mem_stack_pop(vm->mem_stack).obj;
    int float_res = left->num.type == FLOAT_TYPE || right->num.type == FLOAT_TYPE;
    if(!float_res){
        mem_stack_push_obj(vm->mem_stack, obj_create_num_int(left->num.int_val / right->num.int_val));
    }else{
        mem_stack_push_obj(vm->mem_stack, obj_create_num_float(obj_get_num(left) / obj_get_num(right)));        
    }
}

static int powi(int x, int y){
    int ret = 1;
    for(int i = 0; i < y; i++){
        ret *= x;
    }
    return ret;
}

static void vm_exec_pow(struct vm *vm, struct instr instr){
    struct obj *left = mem_stack_pop(vm->mem_stack).obj;
    struct obj *right = mem_stack_pop(vm->mem_stack).obj;
    int float_res = left->num.type == FLOAT_TYPE || right->num.type == FLOAT_TYPE;
    if(!float_res && right->num.int_val >= 0){
        mem_stack_push_obj(vm->mem_stack, obj_create_num_int(powi(left->num.int_val, right->num.int_val)));
    }else{
        mem_stack_push_obj(vm->mem_stack, obj_create_num_float(powf(obj_get_num(left), obj_get_num(right))));        
    }
}

static void vm_exec_retrieve(struct vm *vm, struct instr instr){
    struct obj **p_o = mem_table_access(vm->mem_table, instr.op);
    mem_stack_push_ptr_to_obj(vm->mem_stack, p_o);
}

static void vm_exec_load(struct vm *vm, struct instr instr){
    struct obj *o = *mem_stack_pop(vm->mem_stack).ptr_to_obj;
    mem_stack_push_obj(vm->mem_stack, o);
}

static void vm_exec_store(struct vm *vm, struct instr instr){
    struct obj **p_o = mem_stack_pop(vm->mem_stack).ptr_to_obj;
    struct obj *o = mem_stack_pop(vm->mem_stack).obj;
    *p_o = o;
}


static void vm_exec_uncondjump(struct vm *vm, struct instr instr){
    vm->pc = instr.op;
}

static void vm_exec_condjump(struct vm *vm, struct instr instr){
    if(mem_stack_pop(vm->mem_stack).int_val){
        vm->pc = instr.op;
    }
}

static void vm_exec_less(struct vm *vm, struct instr instr){
    float left = obj_get_num(mem_stack_pop(vm->mem_stack).obj);
    float right = obj_get_num(mem_stack_pop(vm->mem_stack).obj);
    mem_stack_push_int(vm->mem_stack, left < right);
}

static void vm_exec_leq(struct vm *vm, struct instr instr){
    float left = obj_get_num(mem_stack_pop(vm->mem_stack).obj);
    float right = obj_get_num(mem_stack_pop(vm->mem_stack).obj);
    mem_stack_push_int(vm->mem_stack, left <= right);
}

static void vm_exec_call(struct vm *vm, struct instr instr){
    addr_stack_push(vm->addr_stack, vm->pc);
    vm->pc = instr.op;
}

static void vm_exec_return(struct vm *vm, struct instr instr){
    vm->pc = addr_stack_pop(vm->addr_stack);
}

static void vm_exec_makestr(struct vm *vm, struct instr instr){
    char *ptr = malloc(instr.op + 1);
    for(int i = 0; i < instr.op; i++){
        ptr[i] = mem_stack_pop(vm->mem_stack).int_val;
    }
    ptr[instr.op] = 0;
    mem_stack_push_obj(vm->mem_stack, obj_create_str(ptr));
}

static void vm_exec_makeint(struct vm *vm, struct instr instr){
    mem_stack_push_obj(vm->mem_stack, obj_create_num_int(instr.op));
}

static void vm_exec_makeflt(struct vm *vm, struct instr instr){
    mem_stack_push_obj(vm->mem_stack, obj_create_num_float(instr.f));
}

static void vm_exec_subscript(struct vm *vm, struct instr instr){
    struct obj *o = mem_stack_pop(vm->mem_stack).obj;
    struct obj *i = mem_stack_pop(vm->mem_stack).obj;
    mem_stack_push_ptr_to_obj(vm->mem_stack, o->arr.arr + i->num.int_val);
}

static void vm_exec_syscall(struct vm *vm, struct instr instr){
    switch(instr.op){
        case 0:
            return syscall_print(vm);
        case 1:
            return syscall_input(vm);
        default:
            break;
    }
}

void vm_exec_instr(struct vm *vm, struct instr instr){
    switch(instr.type){
        case OP_DECL: 
            return vm_exec_decl(vm, instr);
        case OP_PUSH: 
            return vm_exec_push(vm, instr);
        case OP_ADD: 
            return vm_exec_add(vm, instr);
        case OP_SUB:
            return vm_exec_sub(vm, instr);
        case OP_MUL: 
            return vm_exec_mul(vm, instr);
        case OP_DIV:
            return vm_exec_div(vm, instr);
        case OP_POW:
            return vm_exec_pow(vm, instr);
        case OP_RETRIEVE: 
            return vm_exec_retrieve(vm, instr);
        case OP_LOAD: 
            return vm_exec_load(vm, instr);
        case OP_STORE: 
            return vm_exec_store(vm, instr);            
        case OP_UNCONDJUMP:
            return vm_exec_uncondjump(vm, instr);
        case OP_CONDJUMP:
            return vm_exec_condjump(vm, instr);
        case OP_LESS:
            return vm_exec_less(vm, instr);
        case OP_LEQ:
            return vm_exec_leq(vm, instr);
        case OP_CALL:
            return vm_exec_call(vm, instr);
        case OP_RETURN:
            return vm_exec_return(vm, instr);
        case OP_MAKESTR:
            return vm_exec_makestr(vm, instr);
        case OP_MAKEINT:
            return vm_exec_makeint(vm, instr);
        case OP_MAKEFLT:
            return vm_exec_makeflt(vm, instr);
        case OP_SUBSCRIPT:
            return vm_exec_subscript(vm, instr);
        case OP_SYSCALL:
            return vm_exec_syscall(vm, instr);
        default:
            printf("UNIMPLEMENETED");
    }
}

void vm_exec(struct vm *vm){
    while(vm->pc < vm->instr->len){
        struct instr ins = vm->instr->instr[vm->pc++];
        vm_exec_instr(vm, ins);
    }
}