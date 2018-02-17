#include "error.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct error {
	const char *fn;
	const char *file;
	int line;
};

static __thread struct error *errors;
static __thread size_t len = 0;
static __thread size_t cap = 0;

#define INITIAL_CAP 2

void push_err(const char *fn, const char *file, int line)
{
	if (cap > 0) {
		if (len == cap) {
			cap = cap * 3 / 2;
			errors = realloc(errors, cap * sizeof(struct error));
		}
	} else {
		errors = malloc((cap = INITIAL_CAP) * sizeof(struct error));
	}
	errors[len++] = (struct error) { fn, file, line };
}

int print_errs(FILE *dest)
{
	int printed = 0;
	int last_print = fprintf(dest, "%s. Cause:\n", strerror(errno));
	if (last_print == FAILURE)
		return FAILURE;
	else
		printed += last_print;
	size_t i = len;
	while (i-- > 0) {
		struct error *e = &errors[i];
		last_print = fprintf(dest, "\t%s (\"%s\" line %d)\n", e->fn, e->file, e->line);
		if (last_print == FAILURE)
			return FAILURE;
		else
			printed += last_print;
	}
	len = 0;
	return printed;
}

void drop_err_buf(void)
{
	if (errors != NULL) {
		free(errors);
		errors = NULL;
	}
}
