#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "timer.h"

clock_t frame_start_clock = 0;

void set_start_of_frame_clock()
{
	frame_start_clock = clock();
}

clock_t get_start_of_frame_clock()
{
	return frame_start_clock;
}
