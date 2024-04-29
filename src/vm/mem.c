#include <stdlib.h>
#include <stdio.h>  
#include <assert.h>    
#include "mem.h"
#include "obj.h"

struct mem_stack* mem_stack_create(){
    struct mem_stack *ret = malloc(sizeof(struct mem_stack));
    ret->stk = NULL;
    ret->cap = ret->len = 0;
    return ret;
}

static void mem_stack_push(struct mem_stack *mem_stack, struct mem_stack_type val){
    if(mem_stack->stk == NULL){
        mem_stack->stk = malloc(sizeof(struct mem_stack_type));
        mem_stack->cap = 1;
    }else if(mem_stack->cap <= mem_stack->len){
        struct mem_stack_type *_stk = malloc(sizeof(struct mem_stack_type) * mem_stack->cap * 2);
        for(int i = 0; i < mem_stack->len; i++){
            _stk[i] = mem_stack->stk[i];
        }
        mem_stack->stk = _stk;
        mem_stack->cap *= 2;
    }
    mem_stack->stk[mem_stack->len++] = val;
}

struct mem_stack_type mem_stack_pop(struct mem_stack *mem_stack){
    assert(mem_stack->len != 0);
    return mem_stack->stk[--mem_stack->len];
}

void mem_stack_push_int(struct mem_stack *mem_stack, int i){
    struct mem_stack_type v;
    v.type = INT_TYPE;
    v.int_val = i;
    mem_stack_push(mem_stack, v);
}

void mem_stack_push_obj(struct mem_stack *mem_stack, struct obj *obj){
    struct mem_stack_type v;
    v.type = OBJ_TYPE;
    v.obj = obj;
    mem_stack_push(mem_stack, v);
}

void mem_stack_push_ptr_to_obj(struct mem_stack *mem_stack, struct obj **ptr_to_obj){
    struct mem_stack_type v;
    v.type = PTR_OBJ_TYPE;
    v.ptr_to_obj = ptr_to_obj;
    mem_stack_push(mem_stack, v);
}

struct mem_table* mem_table_create(){
    struct mem_table *ret = malloc(sizeof(struct mem_table));
    ret->len = 0;
    return ret;
}

void mem_table_insert(struct mem_table *mem_table, int type, int dim_row, int dim_col){
    struct obj **val = &mem_table->vals[mem_table->len++];
    switch(type){
        case 0:
            *val = obj_create_num_int(0);
            break;
        case 1:
            *val = obj_create_arr(dim_col);
            for(int i = 0; i < dim_col; i++){
                (*val)->arr.arr[i] = obj_create_num_int(0);
            }
            break;
        case 2:
            *val = obj_create_arr(dim_row);
            for(int i = 0; i < dim_row; i++){
                (*val)->arr.arr[i] = obj_create_arr(dim_col);
                for(int j = 0; j < dim_col; j++){
                    (*val)->arr.arr[i]->arr.arr[i] = obj_create_num_int(0);
                }
            }
            break;
        case 3:
            *val = obj_create_str(NULL);
            break;
        default:
            break;
    }
}


struct obj** mem_table_access(struct mem_table *mem_table, int idx){
    return &mem_table->vals[idx];
}

struct addr_stack* addr_stack_create(){
    struct addr_stack *ret = malloc(sizeof(struct addr_stack));
    ret->len = 0;
    return ret; 
}

void addr_stack_push(struct addr_stack *addr_stack, int addr){
    addr_stack->stk[addr_stack->len++] = addr;
}

int addr_stack_pop(struct addr_stack *addr_stack){
    return addr_stack->stk[--addr_stack->len];
}