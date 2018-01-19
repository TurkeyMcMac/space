#include "keys.h"

#include "error.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <termios.h>
#include <unistd.h>

int set_single_key_input(struct terminal_info *old_info)
{
	if (CATCH_TO (old_info->fd, open,("/dev/tty", O_RDONLY))
	 || CATCH_TO (old_info->flags, fcntl,(old_info->fd, F_GETFL))
	 || CATCH (fcntl,(old_info->fd, F_SETFL, O_NONBLOCK | old_info->flags)))
		return FAILURE;
	if CATCH (tcgetattr,(old_info->fd, &old_info->termios))
		return FAILURE;
	tcflag_t old_c_lflag = old_info->termios.c_lflag;
	old_info->termios.c_lflag &= ~(ECHO|ICANON);
	if CATCH(tcsetattr,(old_info->fd, TCSANOW, &old_info->termios))
		return FAILURE;
	else {
		old_info->termios.c_lflag = old_c_lflag;
		return 0;
	}
}

int reset_single_key_input(const struct terminal_info *old_info)
{
	if (CATCH (fcntl,(old_info->fd, F_SETFL, old_info->flags))
	 || CATCH (tcsetattr,(old_info->fd, TCSANOW, &old_info->termios))
	 || CATCH (close,(old_info->fd)))
		return FAILURE;
	else
		return 0;
}

char last_key(char *buf, size_t len, const struct terminal_info *info)
{
	ssize_t nread = read(info->fd, buf, len);
	if (nread == FAILURE && errno != EAGAIN && errno != EWOULDBLOCK) {
		push_err("read", __FILE__, __LINE__ - 2);
		return FAILURE;
	} else if (nread > 0)
		return buf[nread - 1];
	else
		return '\0';
}
