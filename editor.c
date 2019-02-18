#include <stdio.h>
#include <stdlib.h>

#include "editor.h"
#include "views.h"

int running = 1;

void game_loop()
{
	while(running)
	{
		set_start_of_frame_clock();

		view_canvas();
		show_ui();
		handle_input();
	}
}

void main()
{
	init_sequence();
	game_loop();
}