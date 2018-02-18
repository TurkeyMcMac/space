#include "ticker.h"

#include "error.h"

#include <errno.h>
#include <time.h>

int ticker_init(struct ticker *t, clockid_t clock_id, long interval)
{
	t->interval.tv_sec = interval / (long)1e9;
	t->interval.tv_nsec = interval % (long)1e9;
	return 0;
}

int tick(struct ticker *t)
{
	FORWARD (nanosleep,(&t->interval, NULL));
}
