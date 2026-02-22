#include "clock.h"

#include <stdlib.h>

Clock *CreateClock(int fps) {
    Clock *clock = (Clock *)malloc(sizeof(Clock));

    clock->current_frame = 0;
    clock->last_frame = 0;
    clock->delta_time = 0.0f;
    clock->fps = fps;

    return clock;
}

void DestroyClock(Clock *clock) {
    free(clock);
}

void TickClock(Clock *clock) {
    clock->current_frame = SDL_GetTicksNS();
    Uint64 time_diff = clock->current_frame - clock->last_frame;
    clock->last_frame = clock->current_frame;
    float frame_time = 1000000000.0f/(float)(clock->fps);
    if (time_diff < frame_time) {
        SDL_DelayNS(frame_time-time_diff);
        clock->delta_time = 1.0f;
    } else {
        clock->delta_time = (float)(clock->last_frame)/(float)(frame_time);
    }
}

/**

while (running) {
    // A lot of time computing stuff...

    current_frame = SDL_GetTicks();
    time_diff = current_frame - last_frame;
    last_frame = current_frame;
    if (time_diff < fps) {
        delay();
    }

    // continue...
}

*/