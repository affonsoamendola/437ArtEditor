
#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"

unsigned char far * canvas;

unsigned char far * get_canvas_address()
{
	return (unsigned char far *)canvas;
}

void init_canvas()
{
	int i;

	canvas = (unsigned char far *)malloc(sizeof(char)*CANVAS_SIZE_X*CANVAS_SIZE_Y*2);

	for(i = 0; i < CANVAS_SIZE_X*CANVAS_SIZE_Y*2; i++)
	{
		*(canvas + i) = 0xff;
	}
}