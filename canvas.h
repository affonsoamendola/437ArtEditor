#ifndef CANVAS_H
#define CANVAS_H

#include "geometry.h"

#define CANVAS_SIZE_X 80
#define CANVAS_SIZE_Y 25

unsigned char far * get_canvas_address();

void init_canvas();
void clear_canvas();

RECT get_canvas_rect();

#endif
