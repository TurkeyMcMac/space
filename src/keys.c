#include "keys.h"

#include <fcntl.h>
#include <stddef.h>
#include <termios.h>
#include <unistd.h>

void set_single_key_input(struct termios *old_settings)
{
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK | fcntl(STDIN_FILENO, F_GETFL));

	struct termios settings;
	tcgetattr(STDIN_FILENO, old_settings);
	settings = *old_settings;
	settings.c_lflag &= ~(ECHO|ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

void reset_single_key_input(const struct termios *old_settings)
{
	fcntl(STDIN_FILENO, F_SETFL, ~O_NONBLOCK & fcntl(STDIN_FILENO, F_GETFL));

	tcsetattr(STDIN_FILENO, TCSANOW, old_settings);
}

char last_key(char *buf, size_t len)
{
	ssize_t nread = read(STDIN_FILENO, buf, len);
	if (nread > 0)
		return buf[nread - 1];
	else
		return '\0';
}
