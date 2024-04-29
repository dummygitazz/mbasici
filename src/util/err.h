#ifndef ERR_H
#define ERR_H

#include <stdarg.h>

void compile_error(const char *s, int line_number, char **lines, ...);

#endif