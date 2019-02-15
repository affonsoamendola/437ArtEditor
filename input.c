
#include <stdio.h>
#include <stdlib.h>

#include "cga.h"
#include "keyb.h"
#include "list.h"
#include "input.h"
#include "views.h"
#include "canvas.h"

LIST * active_buttons;

int CURSOR_ENABLED = 1;
int CURSOR_MOVABLE = 1;

extern unsigned char far * canvas;

int EDGE_EVENT_ENABLED = 1;

int current_mode = MODE_DRAWING;

int cursor_x = 10;
int cursor_y = 10;

unsigned char selected_char = 'A';

unsigned char selected_fore_color = 0x0C;
unsigned char selected_back_color = 0x00;
unsigned char selected_blink 	  = 0x00;

int line_start_x = 0;
int line_start_y = 0;
int line_start_selected = 0;

int get_current_mode()
{
	return current_mode;
}

int get_cursor_x()
{
	return cursor_x;
}

int get_cursor_y()
{
	return cursor_y;
}

unsigned char get_selected_attribute()
{

	return (selected_fore_color | (selected_back_color << 4) | (selected_blink << 7));
}


void edge_event(int delta_x, int delta_y)
{
	if(get_current_view() == VIEW_CANVAS && EDGE_EVENT_ENABLED)
	{
		move_viewport(delta_x, delta_y);
	}
}

void move_cursor(int delta_x, int delta_y)
{
	if(CURSOR_ENABLED)
	{
		CURSOR_MOVABLE = 1;

		if(cursor_x + delta_x < 1)
		{
			edge_event(delta_x, 0);
		}
		else if(cursor_x + delta_x > SCREEN_SIZE_X-2)
		{
			edge_event(delta_x, 0);
		}
		else if(cursor_y + delta_y < 1)
		{
			edge_event(0, delta_y);
		}
		else if(cursor_y + delta_y > SCREEN_SIZE_Y-2)
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

int inside_rect(int x, int y, RECT rect)
{
	if(	x >= rect.x && x < rect.x + rect.size_x &&
		y >= rect.y && y < rect.y + rect.size_y)
	{
		return 1;
	}
	else
	{
		return 0;
	}
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
	int i;

	for(i = 0; i < len_list(active_buttons); i++)
	{
		if(inside_rect(cursor_x, cursor_y, ((BUTTON *)get_list_at(active_buttons, i))->clickable_area))
		{
			((BUTTON *)get_list_at(active_buttons, i))->on_click();
		}
	}

	if(get_current_view() == VIEW_CANVAS)
	{
		if(get_current_mode() == MODE_DRAWING)
		{
			draw_char_on_rect_at(	canvas,
									get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
									rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y),
									selected_char, get_selected_attribute());
		}

		if(get_current_mode() == MODE_PAINT_FORE)
		{
			buffer = get_attribute_on_rect_at( 	canvas,
												get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
												rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y));

			buffer &= 0xF0;
			buffer |= selected_fore_color;

			set_attribute_on_rect_at( 	canvas,
										get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
										rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y),
										buffer);
		}

		if(get_current_mode() == MODE_PAINT_BACK)
		{
			buffer = get_attribute_on_rect_at( 	canvas,
												get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
												rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y));

			buffer &= 0x8F;
			buffer |= (selected_back_color << 4);

			set_attribute_on_rect_at( 	canvas,
										get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
										rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y),
										buffer);
		}

		if(get_current_mode() == MODE_CHANGE_CHAR)
		{
			set_char_on_rect_at( 		canvas,
										get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
										rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y),
										selected_char);
		}

		if(get_current_mode() == MODE_LINE)
		{
			if(line_start_selected == 0)
			{
				line_start_x = get_viewport_x() + cursor_x;
				line_start_y = get_viewport_y() + cursor_y;
				line_start_selected = 1;
			}
			else
			{
				draw_line_on_rect_at(	canvas, 
										line_start_x, line_start_y, 
										get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
										rect(0,0, CANVAS_SIZE_X, CANVAS_SIZE_Y), 
										selected_char, get_selected_attribute());

				line_start_selected = 0;
			}
		}
	}
	
	if(get_current_view() == VIEW_BRUSH_SELECT)
	{
		brush_select_screen_cursor_action();
	}	
}

void input_cursor()
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

void input_brush_select()
{
	if(Get_Key_Once(MAKE_F1) | Get_Key_Once(MAKE_ESC))
	{
		set_current_view(VIEW_CANVAS);
	}
}

void input_canvas_view()
{
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

	if(Get_Key_Once(MAKE_F1))
	{
		set_current_view(VIEW_BRUSH_SELECT);
	}

	if(Get_Key(MAKE_CTRL) && Get_Key_Once(MAKE_N))
	{
		show_dialog_yn(	rect(SCREEN_SIZE_X/2-10, SCREEN_SIZE_X/2+10, SCREEN_SIZE_Y/2-3, SCREEN_SIZE_Y/2+3),
						"Are you sure you want to clear the canvas?", clear_canvas, NULL);
	}

	input_cursor();
}

void input_ui()
{
}

void input_ui_peek()
{
}

void handle_input()
{
	input_cursor();

	if(get_current_view() == VIEW_CANVAS)
	{
		input_canvas_view();
	}

	if(get_current_view() == VIEW_BRUSH_SELECT)
	{
		input_brush_select();
	}

	if(get_current_view() == VIEW_UI)
	{
		input_ui();
	}

	if(get_current_view() == VIEW_UI_PEEK)
	{
		input_ui_peek();
	}
}

void init_ui()
{
	active_buttons = create_list();
}


