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
/*
	LIST * test = create_list();

	int * a;
	int * b;

	a = malloc(sizeof(int));
	b = malloc(sizeof(int));

	*a = 10;
	*b = 20;

	append_list(test, a);
	append_list(test, b);

	printf("Element 10 found at: %d\n", get_list_index(test, a));
	printf("Element 20 found at: %d\n", get_list_index(test, b));
	
	remove_list(test, a);

	printf("Element 20 found at: %d\n", get_list_index(test, b));

	printf("\n%d", len_list(test));*/

	init_sequence();
	game_loop();
}