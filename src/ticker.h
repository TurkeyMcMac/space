#ifndef _TICKER_H

#define _TICKER_H

#include <time.h>

struct ticker {
	struct timespec next;
	struct timespec interval;
	clockid_t clock_id;
};

void ticker_init(struct ticker *t, clockid_t clock_id, int sec, long nsec);

int tick(struct ticker *t);

#endif
