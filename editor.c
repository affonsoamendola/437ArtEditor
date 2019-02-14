#include <stdio.h>
#include <stdlib.h>

#include "cga.h"
#include "keyb.h"

#define CANVAS_SIZE_X 80
#define CANVAS_SIZE_Y 25

#define BLINK_RATE 1.0f

#define MODE_DRAWING 0
#define MODE_LINE 1
#define MODE_PAINT_BACK 2
#define MODE_PAINT_FORE 3
#define MODE_CHANGE_CHAR 4

#define WINDOW_TOP_RIGHT 0xBB
#define WINDOW_BOT_RIGHT 0xBC
#define WINDOW_BOT_LEFT 0xC8
#define WINDOW_TOP_LEFT 0xC9

#define WINDOW_HOR 0xCD
#define WINDOW_VER 0xBA

#define BLOCK 0xDB

#define VIEW_CANVAS 0
#define VIEW_BRUSH_SELECT 1

unsigned char far * canvas;

int cursor_x = 10;
int cursor_y = 10;

int viewport_x = 0;
int viewport_y = 0;

int current_mode = MODE_DRAWING;
int current_view = VIEW_CANVAS;

int EDGE_EVENT_ENABLED = 1;
int CURSOR_ENABLED = 1;
int CURSOR_MOVABLE = 1;

unsigned char selected_char = 'A';

unsigned char selected_fore_color = 0x0C;
unsigned char selected_back_color = 0x00;
unsigned char selected_blink 	  = 0x00;

int last_blink_clock;
int blink_counter;
int blink_state = 0;

int line_start_x = 0;
int line_start_y = 0;
int line_start_selected = 0;

int running = 1;

void move_viewport(int delta_x, int delta_y);
void move_cursor(int delta_x, int delta_y);

unsigned char get_selected_attribute()
{
	return (selected_fore_color | (selected_back_color << 4) | (selected_blink << 7));
}

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
		draw_char_on_page(cursor_x, cursor_y, (unsigned char)219, 0x7F, 1);
	}
}

void draw_canvas()
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

	copy_area_from_image(canvas, PAGE_1,
						 area_source,
						 image_source,
						 area_dest,
						 image_dest);

	draw_window_shadowed(rect(2,2,20,10), 14, 6, 3);

	blink_cursor();
	copy_page(1, 0);
}

void draw_brush_select()
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

void brush_select_screen_cursor_action()
{
	int linear_cursor_position;

	if(cursor_y >= 0 && cursor_y < 4)
	{
		linear_cursor_position = cursor_x + cursor_y*SCREEN_SIZE_X;

		if(linear_cursor_position >= 0 && linear_cursor_position < 256)
		{
			selected_char = linear_cursor_position;
		}
	}

	if(cursor_y == 7)
	{
		if(cursor_x >= 0 && cursor_x < 16)
		{
			selected_fore_color = cursor_x;
		}
	}
	else if(cursor_y == 9)
	{
		if(cursor_x >= 0 && cursor_x < 8)
		{
			selected_back_color = cursor_x;
		}
	}
}

void change_mode(int mode)
{
	line_start_selected = 0;

	current_mode = mode;
}

void cursor_action()
{
	unsigned char buffer;

	if(current_view == VIEW_CANVAS)
	{
		if(current_mode == MODE_DRAWING)
		{
			draw_char_on_rect_at(	canvas,
									viewport_x + cursor_x, viewport_y + cursor_y,
									rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y),
									selected_char, get_selected_attribute());
		}

		if(current_mode == MODE_PAINT_FORE)
		{
			buffer = get_attribute_on_rect_at( 	canvas,
												viewport_x + cursor_x, viewport_y + cursor_y,
												rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y));

			buffer &= 0xF0;
			buffer |= selected_fore_color;

			set_attribute_on_rect_at( 	canvas,
										viewport_x + cursor_x, viewport_y + cursor_y,
										rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y),
										buffer);
		}

		if(current_mode == MODE_PAINT_BACK)
		{
			buffer = get_attribute_on_rect_at( 	canvas,
												viewport_x + cursor_x, viewport_y + cursor_y,
												rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y));

			buffer &= 0x8F;
			buffer |= (selected_back_color << 4);

			set_attribute_on_rect_at( 	canvas,
										viewport_x + cursor_x, viewport_y + cursor_y,
										rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y),
										buffer);
		}

		if(current_mode == MODE_CHANGE_CHAR)
		{
			set_char_on_rect_at( 		canvas,
										viewport_x + cursor_x, viewport_y + cursor_y,
										rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y),
										selected_char);
		}

		if(current_mode == MODE_LINE)
		{
			if(line_start_selected == 0)
			{
				line_start_x = viewport_x + cursor_x;
				line_start_y = viewport_y + cursor_y;
				line_start_selected = 1;
			}
			else
			{
				draw_line_on_rect_at(	canvas, 
										line_start_x, line_start_y, 
										viewport_x + cursor_x, viewport_y + cursor_y,
										rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y), 
										selected_char, get_selected_attribute());

				line_start_selected = 0;
			}
		}
	}
	
	if(current_view == VIEW_BRUSH_SELECT)
	{
		brush_select_screen_cursor_action();
	}	
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

void edge_event(int delta_x, int delta_y)
{
	if(current_view == VIEW_CANVAS)
	{
		move_viewport(delta_x, delta_y);
	}
}

void move_cursor(int delta_x, int delta_y)
{
	if(CURSOR_ENABLED)
	{
		CURSOR_MOVABLE = 1;

		if(cursor_x + delta_x < 1 && EDGE_EVENT_ENABLED)
		{
			edge_event(delta_x, 0);
		}
		else if(cursor_x + delta_x > SCREEN_SIZE_X-2 && EDGE_EVENT_ENABLED)
		{
			edge_event(delta_x, 0);
		}
		else if(cursor_y + delta_y < 1 && EDGE_EVENT_ENABLED)
		{
			edge_event(0, delta_y);
		}
		else if(cursor_y + delta_y > SCREEN_SIZE_Y-2 && EDGE_EVENT_ENABLED)
		{
			edge_event(0, delta_y);
		}
		
		if(CURSOR_MOVABLE)
		{
			if(cursor_x + delta_x < 0)
			{
				cursor_x = 0;
			}
			else if(cursor_x + delta_x > SCREEN_SIZE_X-1)
			{
				cursor_x = SCREEN_SIZE_X-1;
			}
			else if(cursor_y + delta_y < 0)
			{
				cursor_y = 0;
			}
			else if(cursor_y + delta_y > SCREEN_SIZE_Y-1)
			{
				cursor_y = SCREEN_SIZE_Y-1;
			}
			else
			{
				cursor_x += delta_x;
				cursor_y += delta_y;
			}
		}
	}
}

void handle_input()
{
	int speed;
	if(Get_Key(MAKE_LEFT_SHIFT))
	{
		speed = 5;
	}
	else
	{
		speed = 1;
	}

	if(Get_Key(MAKE_CTRL) && Get_Key_Once(MAKE_L))
	{
		change_mode(MODE_LINE);
	}

	if(Get_Key(MAKE_CTRL) && Get_Key_Once(MAKE_D))
	{
		change_mode(MODE_DRAWING);
	}

	if(Get_Key(MAKE_CTRL) && Get_Key_Once(MAKE_F))
	{
		change_mode(MODE_PAINT_FORE);
	}

	if(Get_Key(MAKE_CTRL) && Get_Key_Once(MAKE_B))
	{
		change_mode(MODE_PAINT_BACK);
	}

	if(Get_Key(MAKE_CTRL) && Get_Key_Once(MAKE_H))
	{
		change_mode(MODE_CHANGE_CHAR);
	}

	if(Get_Key_Once(MAKE_ESC))
	{
		if(current_view == VIEW_BRUSH_SELECT)
		{
			current_view = VIEW_CANVAS;
		}
	}

	if(Get_Key_Once(MAKE_F1))
	{
		if(current_view == VIEW_CANVAS)
		{
			current_view = VIEW_BRUSH_SELECT;
		}
	}

	if(Get_Key_Once(MAKE_LEFT))
	{
		move_cursor(-speed, 0);
	}
	if(Get_Key_Once(MAKE_RIGHT))
	{
		move_cursor(speed, 0);
	}
	if(Get_Key_Once(MAKE_UP))
	{
		move_cursor(0, -speed);
	}
	if(Get_Key_Once(MAKE_DOWN))
	{
		move_cursor(0, speed);
	}
	if(Get_Key_Once(MAKE_ENTER))
	{
		cursor_action();
	}
}

void init_canvas()
{
	int i;

	canvas = malloc(sizeof(char)*CANVAS_SIZE_X*CANVAS_SIZE_Y*2);

	for(i = 0; i < CANVAS_SIZE_X*CANVAS_SIZE_Y*2; i++)
	{
		*(canvas + i) = 0;
	}
}

void game_loop()
{
	last_blink_clock = clock();

	while(running)
	{
		if(current_view == VIEW_CANVAS)
		{
			draw_canvas();
		}
		else if(current_view == VIEW_BRUSH_SELECT)
		{
			draw_brush_select();
		}

		handle_input();
	}
}

void main()
{
	Keyboard_Install_Driver();
	set_graphics_mode();
	init_canvas();
	game_loop();
}