#include "ticker.h"

#include "error.h"

#include <errno.h>
#include <time.h>

int ticker_init(struct ticker *t, clockid_t clock_id, int sec, long nsec)
{
	t->interval.tv_sec = sec;
	t->interval.tv_nsec = nsec;
	t->clock_id = clock_id;

	/*if ((errno = clock_gettime(clock_id, &t->next)) != 0) {
		push_err("clock_gettime", __FILE__, __LINE__ - 1);
		return FAILURE;
	} else*/
		return 0;
}

int tick(struct ticker *t)
{
	t->next.tv_sec += t->interval.tv_sec;
	t->next.tv_nsec += t->interval.tv_nsec;
	if (t->next.tv_nsec >= 1000000000) {
		t->next.tv_nsec -= 1000000000;
		++t->next.tv_sec;
	}

	FORWARD(nanosleep,(&t->interval, NULL));
}
