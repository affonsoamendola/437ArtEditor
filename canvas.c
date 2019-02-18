#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"

unsigned char * canvas;

unsigned char * get_canvas_address()
{
	return canvas;
}

void init_canvas()
{
	int i;

	canvas = (unsigned char far *)malloc(sizeof(char)*CANVAS_SIZE_X*CANVAS_SIZE_Y*2);

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