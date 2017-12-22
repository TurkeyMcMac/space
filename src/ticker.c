#include "ticker.h"

#include <time.h>

void ticker_init(struct ticker *t, clockid_t clock_id, int sec, long nsec)
{
	clock_gettime(clock_id, &t->next);
	t->interval.tv_sec = sec;
	t->interval.tv_nsec = nsec;
	t->clock_id = clock_id;
}

int tick(struct ticker *t)
{
	t->next.tv_sec += t->interval.tv_sec;
	t->next.tv_nsec += t->interval.tv_nsec;
	if (t->next.tv_nsec >= 1000000000) {
		t->next.tv_nsec -= 1000000000;
		++t->next.tv_sec;
	}
	return clock_nanosleep(t->clock_id, TIMER_ABSTIME, &t->next, NULL);
}
