#ifndef _ERROR_H

#include <stdio.h>

#define _ERROR_H

#define FAILURE (-1)

#define CATCH(fn, args) ((fn args) == FAILURE ? (push_err(#fn, __FILE__, __LINE__), 1) : 0)

#define CATCH_TO(dest, fn, args) ((dest = (fn args)) == FAILURE ? (push_err(#fn, __FILE__, __LINE__), 1) : 0)

#define FORWARD(fn, args) do { int res = (fn args); if (res == FAILURE) push_err(#fn, __FILE__, __LINE__); return res; } while (0)

void push_err(const char *fn, const char *file, int line);

int print_errs(FILE *dest);

#endif
