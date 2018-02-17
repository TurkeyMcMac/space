#include "cmdopt.h"

#include "error.h"
#include <limits.h>
#include <string.h>

#define CONTROL_CHAR_NUM (CHAR_MAX - PRINTABLE_CHAR_NUM + 1)

void cmdopt_parser_init(struct cmdopt_parser *self, size_t n_copt, const struct cmdopt coptlist[n_copt])
{
	self->n_copt = n_copt;
	self->coptlist = coptlist;
	memset(self->short_map, -1, PRINTABLE_CHAR_NUM);
	while (n_copt-- > 0)
		self->short_map[coptlist[n_copt].short_name - CONTROL_CHAR_NUM] = n_copt;
}

static const struct cmdopt *cmdopt_parser_find_short(const struct cmdopt_parser *self,
		char short_name)
{
	signed char idx;
	if (short_name >= CONTROL_CHAR_NUM &&
	    (idx = self->short_map[short_name - CONTROL_CHAR_NUM]) != -1)
		return &self->coptlist[(unsigned char)idx];
	else
		return NULL;
}

static int cmp_long_names(size_t len1, const char *str1, const char *str2)
{
	size_t i;
	for (i = 0; i < len1; ++i) {
		int order;
		char c1 = str1[i], c2 = str2[i];
		if (c2 == '\0' || (order = c1 - c2) > 0)
			return 1;
		else if (order < 0)
			return -1;
	}
	return -str2[i];
}

static const struct cmdopt *cmdopt_parser_find_long(const struct cmdopt_parser *self,
		size_t len,
		const char *long_name)
{
	size_t lower = 0, upper = self->n_copt; /* NOTE: Coptlist cannot be empty! */
	while (lower < upper) {
		size_t mid = (lower + upper) / 2;
		const char *other = self->coptlist[mid].long_name;
		int order;
		if (other == NULL || (order = cmp_long_names(len, long_name, other)) < 0)
			upper = mid;
		else if (order > 0)
			lower = mid + 1;
		else
			return &self->coptlist[mid];
	}
	return NULL;
}

static int parse_long_cmdopt(const struct cmdopt_parser *parser,
		const char *opt,
		int (*err)(size_t, const char *, void *),
		void *env)
{
	size_t i;
	const char *arg;
	for (i = 0, arg = NULL; opt[i] != '\0'; ++i) {
		if (opt[i] == '=') {
			arg = &opt[i + 1];
			break;
		}
	}
	const struct cmdopt *copt = cmdopt_parser_find_long(parser, i, opt);
	if (copt != NULL)
		return (*copt->fn)(copt, arg, env);
	else
		return (*err)(i, opt, env);
}

static int parse_short_cmdopts(const struct cmdopt_parser *parser,
		const char *opts,
		int (*err)(size_t, const char *, void *),
		void *env)
{
	int errnum;
	const struct cmdopt *copt;
	const char *arg;
	for (; *opts != '\0'; ++opts) {
		copt = cmdopt_parser_find_short(parser, *opts);
		if (opts[1] == '=')
			arg = &opts[2];
		else
			arg = NULL;
		if (copt != NULL)
			errnum = (*copt->fn)(copt, arg, env);
		else
			errnum = (*err)(1, opts, env);
		if (arg != NULL || errnum != 0)
			return errnum;
	}
	return 0;
}

int parse_cmdopts(const struct cmdopt_parser *parser,
		const char *arg,
		int (*err)(size_t, const char *, void *),
		void *env)
{
	if (arg[0] == '-') {
		if (arg[1] == '-')
			return parse_long_cmdopt(parser, &arg[2], err, env);
		else
			return parse_short_cmdopts(parser, &arg[1], err, env);
	} else
		return 1;
}

static int print_description(const char * const *desc, int padding, FILE *dest)
{
	int written = 0, last_write;
	if (desc[0] == NULL)
		return 0;
	if CATCH_TO (last_write, fprintf,(dest, "%s\n", desc[0]))
		return FAILURE;
	else
		written += last_write;
	for (++desc; *desc != NULL; ++desc)
		if CATCH_TO (last_write, fprintf,(dest, "\t%*c%s\n", padding, ' ', *desc))
			return FAILURE;
		else
			written += last_write;
	return written;
}

static int print_cmdopt_short_long(const struct cmdopt *copt, int padding, FILE *dest)
{
	int written = 0, last_write;
	if CATCH_TO (last_write, fprintf,(dest, "\t-%c, --%-*s",
			copt->short_name, padding - 6, copt->long_name))
		return FAILURE;
	else
		written += last_write;
	if CATCH_TO (last_write, print_description,(copt->description, padding, dest))
		return FAILURE;
	else
		written += last_write;
	return written;
}

static int print_cmdopt_short(const struct cmdopt *copt, int padding, FILE *dest)
{
	int written = 0, last_write;
	if CATCH_TO (last_write, fprintf,(dest, "\t-%-*c",
			padding - 1, copt->short_name))
		return FAILURE;
	else
		written += last_write;
	if CATCH_TO (last_write, print_description,(copt->description, padding, dest))
		return FAILURE;
	else
		written += last_write;
	return written;
}

static int print_cmdopt_long(const struct cmdopt *copt, int padding, FILE *dest)
{
	int written = 0, last_write;
	if CATCH_TO (last_write, fprintf,(dest, "\t--%-*s",
			padding - 2, copt->long_name))
		return FAILURE;
	else
		written += last_write;
	if CATCH_TO (last_write, print_description,(copt->description, padding, dest))
		return FAILURE;
	else
		written += last_write;
	return written;
}

int print_cmdopt(const struct cmdopt *copt, int padding, FILE *dest)
{
	if (copt->short_name) {
		if (copt->long_name)
			FORWARD (print_cmdopt_short_long,(copt, padding, dest));
		else
			FORWARD (print_cmdopt_short,(copt, padding, dest));
	} else if (copt->long_name)
		FORWARD (print_cmdopt_long,(copt, padding, dest));
	else
		return 0;
}
