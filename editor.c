#include <stdio.h>
#include <stdlib.h>

#include "cga.h"
#include "keyb.h"
#include "list.h"
#include "views.h"

int running = 1;

void game_loop()
{
	while(running)
	{
		set_start_of_frame_clock();

		if(get_current_view() == VIEW_CANVAS)
		{
			view_canvas();
		}
		else if(get_current_view() == VIEW_BRUSH_SELECT)
		{
			view_brush_select();
		}

		handle_input();
	}
}

void main()
{
	init_sequence();
	game_loop();
}