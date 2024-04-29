#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mem.h"
#include "syscall.h"
#include "vm.h"
#include "obj.h"

void syscall_print(struct vm *vm){
    int arg_count = mem_stack_pop(vm->mem_stack).int_val;
    for(int i = 0; i < arg_count; i++){
        struct obj *o = mem_stack_pop(vm->mem_stack).obj;
        if(o->type == OBJ_NUM_TYPE){
            if(o->num.type == INTEGER_TYPE){
                printf("%d", o->num.int_val);
            }else{
                printf("%.5g", o->num.float_val);
            }
        }else{
            printf("%s", o->str.str);
        }
    }
    printf("\n");
}

static char input_buffer[1000], buffer[1000];

void syscall_input(struct vm *vm){
    int arg_count = mem_stack_pop(vm->mem_stack).int_val;
    fgets(input_buffer, sizeof(input_buffer), stdin);
    input_buffer[strcspn(input_buffer, "\n")] = ',';
    int ptr = 0;
    for(char *c = input_buffer; *c != 0; c++){
        if(*c == ','){
            buffer[ptr] = 0;
            struct obj **p_o = mem_stack_pop(vm->mem_stack).ptr_to_obj;
            if((*p_o)->type == OBJ_STR_TYPE){
                (*p_o) = obj_create_str(strdup(buffer));
            }else{
                (*p_o) = obj_create_num_int(atoi(buffer));
            }
            ptr = 0;
        }else{
            buffer[ptr++] = *c;
        }
    }
}