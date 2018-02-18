#ifndef _TICKER_H

#define _TICKER_H

#include <time.h>

struct ticker {
	struct timespec next;
	long interval;
	clockid_t clock_id;
};

int ticker_init(struct ticker *t, clockid_t clock_id, long interval);

int tick(struct ticker *t);

#endif
