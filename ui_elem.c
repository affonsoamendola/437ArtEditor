#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cga.h"
#include "ui_elem.h"

#define BLINK_RATE 1.0f

int last_blink_clock = 0;

int blink_counter = 0;
int blink_state = 0;

int current_border_color_fore = 0x0E;
int current_border_color_back = 0x01;

void draw_shadow(int screen_pos_x, int screen_pos_y)
{
	unsigned char buffer;

	buffer = 	get_attribute_on_rect_at( 	PAGE_1,
											screen_pos_x, screen_pos_y,
											rect(0,0, SCREEN_SIZE_X, SCREEN_SIZE_Y));

	if(buffer & 0x08)
	{
		buffer &= 0x87;
	}
	else
	{
		buffer &= 0x80;
	}

	set_attribute_on_rect_at( 	PAGE_1,
								screen_pos_x, screen_pos_y,
								rect(0,0, SCREEN_SIZE_X, SCREEN_SIZE_Y),
								buffer);
}

void draw_window(RECT bounds, unsigned char border_color_fore, unsigned char border_color_back, unsigned char fill_color)
{
	int i, j;

	draw_char_on_page(bounds.x + bounds.size_x-1, 	bounds.y,				  	WINDOW_TOP_RIGHT,	border_color_fore | (border_color_back<<4), 1);
	draw_char_on_page(bounds.x + bounds.size_x-1, 	bounds.y + bounds.size_y-1, WINDOW_BOT_RIGHT,	border_color_fore | (border_color_back<<4), 1);
	draw_char_on_page(bounds.x, 					bounds.y + bounds.size_y-1, WINDOW_BOT_LEFT,	border_color_fore | (border_color_back<<4), 1);
	draw_char_on_page(bounds.x, 			 	   	bounds.y, 				  	WINDOW_TOP_LEFT,	border_color_fore | (border_color_back<<4), 1);

	for(i = bounds.x + 1; i < bounds.x + bounds.size_x - 1; i++)
	{
		draw_char_on_page(i, bounds.y,						WINDOW_HOR,	border_color_fore | (border_color_back<<4), 1);
		draw_char_on_page(i, bounds.y + bounds.size_y - 1,	WINDOW_HOR,	border_color_fore | (border_color_back<<4), 1);
	}

	for(i = bounds.y + 1; i < bounds.y + bounds.size_y - 1; i++)
	{
		draw_char_on_page(bounds.x, 					i,	WINDOW_VER,	border_color_fore | (border_color_back<<4), 1);
		draw_char_on_page(bounds.x + bounds.size_x -1, 	i,	WINDOW_VER,	border_color_fore | (border_color_back<<4), 1);
	}

	for(i = bounds.x + 1; i < bounds.x + bounds.size_x - 1; i++)
	{
		for(j = bounds.y + 1; j < bounds.y +bounds.size_y - 1; j++)
		{
			draw_char_on_page(i,  j,	BLOCK,	fill_color, 1);
		}
	}
}

void draw_window_shadowed(RECT bounds, unsigned char border_color_fore, unsigned char border_color_back, unsigned char fill_color)
{
	int i;

	draw_window(bounds, border_color_fore, border_color_back, fill_color);

	for(i = bounds.x + 1; i < bounds.x + bounds.size_x + 1; i++)
	{
		draw_shadow(i, bounds.y + bounds.size_y);
	}

	for(i = bounds.y + 1; i < bounds.y + bounds.size_y; i++)
	{
		draw_shadow(bounds.x + bounds.size_x, i);
	}
}

void blink_cursor()
{
	blink_counter = clock() - last_blink_clock;

	if(blink_counter > BLINK_RATE)
	{
		blink_state = !blink_state;
		last_blink_clock = clock();
	}

	if(blink_state)
	{
		draw_char_on_page(get_cursor_x(), get_cursor_y(), (unsigned char)219, 0x7F, 1);
	}
}
