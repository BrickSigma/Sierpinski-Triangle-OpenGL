#ifndef CLOCK_H
#define CLOCK_H

#include <SDL3/SDL.h>

typedef struct Clock {
	Uint64 last_frame;
	Uint64 current_frame;
	float delta_time;
	int fps;
} Clock;

Clock *CreateClock(int fps);

void DestroyClock(Clock *clock);

void TickClock(Clock *clock);

#endif // CLOCK_H