#include <stdio.h>
#include "err.h"
void compile_error(const char *s, int line_number, char **lines, ...){
    char ESC=27;
    printf("On line %d: %s\n", line_number, lines[line_number]);
    printf("Syntax error: ");
    va_list args;
    va_start(args, lines);
    vprintf(s, args);
    va_end(args);
    printf("\n\n"); 
}