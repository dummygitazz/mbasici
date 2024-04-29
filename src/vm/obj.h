#ifndef OBJ_H
#define OBJ_H

struct obj;

struct obj_num {
    enum { INTEGER_TYPE, FLOAT_TYPE } type;
    union {
        int int_val;
        float float_val;
    };
};

struct obj_arr {
    int len;
    struct obj **arr;
};

struct obj_str {
    const char *str;
};

struct obj {
    enum { OBJ_NUM_TYPE, OBJ_ARR_TYPE, OBJ_STR_TYPE } type;
    union {
        struct obj_num num;
        struct obj_arr arr;
        struct obj_str str;
    };
};

struct obj* obj_create_num_int(int i);
struct obj* obj_create_num_float(float f);
float obj_get_num(struct obj *o);
struct obj* obj_create_arr(int len);
struct obj* obj_create_str(const char *s);

#endif