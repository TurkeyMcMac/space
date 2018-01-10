#include "keys.h"

#include "error.h"

#include <fcntl.h>
#include <stddef.h>
#include <termios.h>
#include <unistd.h>

int set_single_key_input(struct termios *old_settings)
{
	int old_opts;
	if (CATCH_TO (old_opts, fcntl,(STDIN_FILENO, F_GETFL)) ||
	    CATCH (fcntl,(STDIN_FILENO, F_SETFL, O_NONBLOCK | old_opts)))
		return FAILURE;

	struct termios settings;
	if CATCH (tcgetattr,(STDIN_FILENO, old_settings))
		return FAILURE;
	settings = *old_settings;
	settings.c_lflag &= ~(ECHO|ICANON);
	if CATCH (tcsetattr,(STDIN_FILENO, TCSANOW, &settings))
		return FAILURE;
	else
		return 0;
}

int reset_single_key_input(const struct termios *old_settings)
{
	if (CATCH (fcntl,(STDIN_FILENO, F_SETFL, ~O_NONBLOCK & fcntl(STDIN_FILENO, F_GETFL))) ||
	    CATCH (tcsetattr,(STDIN_FILENO, TCSANOW, old_settings)))
		return FAILURE;
	else
		return 0;
}

char last_key(char *buf, size_t len)
{
	ssize_t nread;
	if CATCH_TO (nread, read,(STDIN_FILENO, buf, len))
		return FAILURE;
	else if (nread > 0)
		return buf[nread - 1];
	else
		return '\0';
}
