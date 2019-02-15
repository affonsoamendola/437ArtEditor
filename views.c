
#include <stdio.h>
#include <stdlib.h>

#include "views.h"
#include "cga.h"
#include "canvas.h"

int viewport_x = 0;
int viewport_y = 0;

extern unsigned char selected_char; 

extern unsigned char selected_fore_color;
extern unsigned char selected_back_color;
extern unsigned char selected_blink;

extern int current_border_color_fore;
extern int current_border_color_back;

int UI_SHOW_EXIT = 0;

int current_view = VIEW_CANVAS;

int get_viewport_x()
{
	return viewport_x;
}

int get_viewport_y()
{
	return viewport_y;
}

int get_current_view()
{
	return current_view;
}

void set_current_view(int view)
{
	current_view = view;
}

void view_canvas()
{
	RECT area_source;
	RECT image_source;
	RECT area_dest;
	RECT image_dest;

	fill_screen(0,0,1);

	area_source = rect( viewport_x, viewport_y, SCREEN_SIZE_X, SCREEN_SIZE_Y);
	image_source = rect( 0, 0, CANVAS_SIZE_X, CANVAS_SIZE_Y);
	area_dest = rect( 0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y);
	image_dest = rect( 0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y);

	copy_area_from_image(get_canvas_address(), PAGE_1,
						 area_source,
						 image_source,
						 area_dest,
						 image_dest);

	draw_window_shadowed(rect(10,10, 10, 10), 10, 5, 5);

	blink_cursor();
	copy_page(1, 0);
}

void view_brush_select()
{
	int i;

	fill_screen(0,0,1);

	for(i = 0; i < 256; i++)
	{
		if(i != selected_char)
		{
			draw_char_on_page(i, 0, (unsigned char)i, get_selected_attribute(), 1);
		}
		else
		{
			draw_char_on_page(i, 0, (unsigned char)i, get_selected_attribute() ^ 0x80, 1);
		}
	}

	for(i = 0; i < 16; i++)
	{
		if(i != selected_fore_color)
		{
			draw_char_on_page(i, 7, 219, i, 1);
		}
		else
		{
			draw_char_on_page(i, 7, 219, i ^ 0x80, 1);
		}
		
	}
	for(i = 0; i < 8; i++)
	{
		if(i != selected_back_color)
		{
			draw_char_on_page(i, 9, 219, i, 1);
		}
		else
		{
			draw_char_on_page(i, 9, 219, i ^ 0x80, 1);
		}
	}

	blink_cursor();
	copy_page(1, 0);
}

void view_ui()
{
	if(UI_SHOW_EXIT)
	{
		draw_window_shadowed(rect(SCREEN_SIZE_X/2 - 20, SCREEN_SIZE_Y - 10, 40, 20), current_border_color_fore, current_border_color_back, current_border_color_back);
	}
}

void view_ui_peek()
{
}

void move_viewport(int delta_x, int delta_y)
{
	if(viewport_x + delta_x < 0)
	{
		viewport_x = 0;
	}
	else if((viewport_x + SCREEN_SIZE_X) + delta_x > CANVAS_SIZE_X)
	{
		viewport_x = CANVAS_SIZE_X - SCREEN_SIZE_X;
	}
	else if(viewport_y + delta_y < 0)
	{
		viewport_y = 0;
	}
	else if((viewport_y + SCREEN_SIZE_Y) + delta_y > CANVAS_SIZE_Y)
	{
		viewport_y = CANVAS_SIZE_Y - SCREEN_SIZE_Y;
	}
	else
	{
		if(viewport_x + delta_x >= 0 && (viewport_x + SCREEN_SIZE_X) + delta_x <= CANVAS_SIZE_X)
		{
			viewport_x += delta_x;
		}
		else if(viewport_y + delta_y >= 0 && (viewport_y + SCREEN_SIZE_Y) + delta_y <= CANVAS_SIZE_Y)
		{
			viewport_y += delta_y;
		}
	}
}