#include "ticker.h"

#include "error.h"

#include <errno.h>
#include <time.h>

int ticker_init(struct ticker *t, clockid_t clock_id, long interval)
{
	t->interval = interval;
	t->clock_id = clock_id;
/*
	if ((errno = clock_gettime(clock_id, &t->next)) != 0) {
		push_err("clock_gettime", __FILE__, __LINE__ - 1);
		return FAILURE;
	} else
		return 0;
*/
	return 0;
}

int tick(struct ticker *t)
{
	t->next.tv_nsec += t->interval;
	t->next.tv_sec += t->next.tv_nsec / 1e9;
	t->next.tv_nsec %= (long)1e9;

	if ((errno = clock_nanosleep(t->clock_id, TIMER_ABSTIME, &t->next, NULL)) != 0) {
		push_err("clock_nanosleep", __FILE__, __LINE__ - 1);
		return FAILURE;
	} else
		return 0;
}
