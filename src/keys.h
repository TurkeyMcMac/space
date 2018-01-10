#ifndef _KEYS_H

#define _KEYS_H

#include <stddef.h>
#include <termios.h>

int set_single_key_input(struct termios *old_settings);

int reset_single_key_input(const struct termios *old_settings);

char last_key(char *buf, size_t len);

#endif
