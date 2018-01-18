#include "keys.h"

#include "error.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <termios.h>
#include <unistd.h>

int set_single_key_input(struct termios *old_settings)
{
	int tty;
	if CATCH_TO(tty, open,("/dev/tty", O_RDONLY))
		return FAILURE;
	int old_opts;
	if (CATCH_TO (old_opts, fcntl,(tty, F_GETFL)) ||
	    CATCH (fcntl,(tty, F_SETFL, O_NONBLOCK | old_opts)))
		return FAILURE;

	struct termios settings;
	if CATCH (tcgetattr,(tty, old_settings))
		return FAILURE;
	settings = *old_settings;
	settings.c_lflag &= ~(ECHO|ICANON);
	FORWARD(tcsetattr,(tty, TCSANOW, &settings));
}

int reset_single_key_input(const struct termios *old_settings)
{
	int tty = open("/dev/tty", O_RDONLY);
	if (CATCH (fcntl,(tty, F_SETFL, ~O_NONBLOCK & fcntl(tty, F_GETFL))) ||
	    CATCH (tcsetattr,(tty, TCSANOW, old_settings)))
		return FAILURE;
	else
		return 0;
}

char last_key(char *buf, size_t len)
{
	ssize_t nread = read(STDIN_FILENO, buf, len);
	if (nread == FAILURE && errno != EAGAIN && errno != EWOULDBLOCK) {
		push_err("read", __FILE__, __LINE__ - 2);
		return FAILURE;
	} else if (nread > 0)
		return buf[nread - 1];
	else
		return '\0';
}
