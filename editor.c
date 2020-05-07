#include <stdio.h>
#include <stdlib.h>

#include "editor.h"
#include "views.h"

#include "list.h"

int running = 1;

void game_loop()
{
	while(running)
	{
		set_start_of_frame_clock();

		fill_screen(0, 0, 1);

		view_canvas();
		show_ui();

		blink_cursor();

		copy_page(1,0);

		handle_input();
	}
}

void main()
{
	init_sequence();
	game_loop();
	destroy_sequence();
}
