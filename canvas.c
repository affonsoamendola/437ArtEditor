#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"
#include "geometry.h"

unsigned char far * canvas;

RECT canvas_rect;

unsigned char far * get_canvas_address()
{
	return canvas;
}

RECT get_canvas_rect()
{
	return canvas_rect;
}

void init_canvas()
{
	int i;

	canvas = (unsigned char far *)malloc(sizeof(char)*CANVAS_SIZE_X*CANVAS_SIZE_Y*2);

	canvas_rect = rect(0,0,CANVAS_SIZE_X, CANVAS_SIZE_Y);

	for(i = 0; i < CANVAS_SIZE_X*CANVAS_SIZE_Y*2; i++)
	{
		*(canvas + i) = 0x00;
	}
}

void clear_canvas()
{
	int i;

	for(i = 0; i < CANVAS_SIZE_X*CANVAS_SIZE_Y*2; i++)
	{
		*(canvas + i) = 0x00;
	}
}