#ifndef _KEYS_H

#define _KEYS_H

#include <stddef.h>
#include <termios.h>

struct terminal_info {
	struct termios termios;
	int flags;
	int fd;
};

int set_single_key_input(struct terminal_info *old_info);

int reset_single_key_input(const struct terminal_info *old_info);

char last_key(char *buf, size_t len, const struct terminal_info *info);

#endif
