#include <stdlib.h>
#include "obj.h"

struct obj* obj_create_num_int(int i){
    struct obj *ret = malloc(sizeof(struct obj));
    ret->type = OBJ_NUM_TYPE;
    ret->num.type = INTEGER_TYPE;
    ret->num.int_val = i;

    return ret;
}

struct obj* obj_create_num_float(float f){
    struct obj *ret = malloc(sizeof(struct obj));
    ret->type = OBJ_NUM_TYPE;
    ret->num.type = FLOAT_TYPE;
    ret->num.float_val = f;
    return ret;
}

float obj_get_num(struct obj *o){
    if(o->num.type == FLOAT_TYPE) return o->num.float_val;
    return o->num.int_val;
}

struct obj* obj_create_arr(int len){
    struct obj *ret = malloc(sizeof(struct obj));
    ret->type = OBJ_ARR_TYPE;
    ret->arr.len = len;
    ret->arr.arr = malloc(len * sizeof(struct obj*));
    return ret;
}

struct obj* obj_create_str(const char *s){
    struct obj *ret = malloc(sizeof(struct obj));
    ret->type = OBJ_STR_TYPE;
    ret->str.str = s;
    return ret;
}