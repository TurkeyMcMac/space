#ifndef _CMDOPT_H

#define _CMDOPT_H

#include <stddef.h>
#include <stdio.h>

struct cmdopt {
	char short_name;
	const char *long_name;
	const char * const *description;
	int (*fn)(const struct cmdopt *self, const char *arg, void *env);
};

#define PRINTABLE_CHAR_NUM 96

struct cmdopt_parser {
	size_t n_copt;
	const struct cmdopt *coptlist;
	signed char short_map[PRINTABLE_CHAR_NUM]; /* No more than 96 options can exist. That should
	                                              be enough. */
};

void cmdopt_parser_init(struct cmdopt_parser *self, size_t n_copt, const struct cmdopt coptlist[n_copt]);

int parse_cmdopts(const struct cmdopt_parser *parser,
		const char *arg,
		int (*err)(size_t, const char *, void *),
		void *env);

int print_cmdopt(const struct cmdopt *coptlist, int padding, FILE *dest);

#endif
