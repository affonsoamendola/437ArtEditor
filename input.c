#include <stdio.h>
#include <stdlib.h>

#include "input.h"
#include "views.h"
#include "cga.h"
#include "ui_elem.h"
#include "ui.h"
#include "canvas.h"
#include "keyb.h"
#include "list.h"

int CURSOR_ENABLED = 1;
int CURSOR_MOVABLE = 1;

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

LIST * default_hotkeys_list;

LIST * active_hotkey_stack;

void set_all_hotkeys_enabled(int value)
{
	int  i;

	LIST * hotkeys_list = (LIST *)get_list_last(active_hotkey_stack);

	for(i = 0; i < len_list(hotkeys_list); i++)
	{
		((HOTKEY *)get_list_at(hotkeys_list, i))->enabled = value;
	}
}

LIST * get_active_hotkey_list()
{
	return (LIST *)get_list_last(active_hotkey_stack);
}

void set_hotkey_enabled(int value, int hotkey_index)
{
	((HOTKEY *)get_list_at(get_active_hotkey_list(), hotkey_index))->enabled = value;
}

HOTKEY_INDEX new_hotkey(int key_1, 
						int key_2, 
						int key_3, 
						void (* hotkey_action)(int), 
						int hotkey_action_option,
						LIST * hotkey_list)
{
	HOTKEY * hotkey;

	hotkey = malloc(sizeof(HOTKEY));

	hotkey->keys[0] = key_1;
	hotkey->keys[1] = key_2;
	hotkey->keys[2] = key_3;

	hotkey->enabled = 1;

	hotkey->hotkey_action = hotkey_action;
	hotkey->hotkey_action_option = hotkey_action_option;

	return append_list(hotkey_list, hotkey);
}

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

void change_mode(int mode)
{
	line_start_selected = 0;

	current_mode = mode;
}

void cursor_action()
{
	unsigned char buffer;
	int i, j;

	WINDOW * current_window;
	BUTTON * current_button;

	for(i = 0; i < get_active_windows_amount(); i++)
	{
		current_window = (WINDOW *)get_list_at((LIST *)get_active_windows(), i);

		for(j = 0; j < len_list(current_window->button_list); j++)
		{
			current_button = (BUTTON *)get_list_at(current_window->button_list, j);

			if(inside_rect(cursor_x, cursor_y, current_button->on_click_area))
			{
				button_click(current_button);
				break;
			}
		}
	}

	set_draw_start_offset(get_canvas_address());
	set_draw_rect(get_canvas_rect());

	if(get_current_view() == VIEW_CANVAS)
	{
		if(get_current_mode() == MODE_DRAWING)
		{
			set_screen_element	(	get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
									selected_char, get_selected_attribute());
		}

		if(get_current_mode() == MODE_PAINT_FORE)
		{
			buffer = get_attribute 	( get_viewport_x() + cursor_x, get_viewport_y() + cursor_y);

			buffer &= 0xF0;
			buffer |= selected_fore_color;

			set_attribute( 	get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
							buffer);
		}

		if(get_current_mode() == MODE_PAINT_BACK)
		{
			buffer = get_attribute( get_viewport_x() + cursor_x, get_viewport_y() + cursor_y);

			buffer &= 0x8F;
			buffer |= (selected_back_color << 4);

			set_attribute( 	get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
							buffer);
		}

		if(get_current_mode() == MODE_CHANGE_CHAR)
		{
			set_char( 	get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
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
				draw_line(	line_start_x, line_start_y, 
							get_viewport_x() + cursor_x, get_viewport_y() + cursor_y,
							selected_char, get_selected_attribute());

				line_start_selected = 0;
			}
		}
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

void handle_input()
{
	HOTKEY * current_hotkey;
	int i, j;
	int combination_pressed;
	LIST * hotkeys_list = (LIST *)get_list_last(active_hotkey_stack);

	for(i = 0; i < len_list(hotkeys_list); i++)
	{
		current_hotkey = (HOTKEY *)get_list_at(hotkeys_list, i);

		combination_pressed = 1;

		if( current_hotkey->enabled == 1)
		{
			for(j = 0; j < 3; j++)
			{
				if( current_hotkey->keys[j] != -1)
				{
					if(	current_hotkey->keys[j] == MAKE_LEFT_SHIFT || 
						current_hotkey->keys[j] == MAKE_CTRL ||
						current_hotkey->keys[j] == MAKE_ALT ||
						current_hotkey->keys[j] == MAKE_RIGHT_SHIFT )
					{
						if(!Get_Key(current_hotkey->keys[j]))
						{
							combination_pressed = 0;
						}
					}
					else
					{
						if(!Get_Key_Once(current_hotkey->keys[j]))
						{
							combination_pressed = 0;
						}
					}
				}
			}

			if(combination_pressed)
			{
				current_hotkey->hotkey_action(current_hotkey->hotkey_action_option);
			}
		}
	}

	input_cursor();
}

void init_input()
{
	default_hotkeys_list = create_list();
	active_hotkey_stack = create_list();

	append_list(active_hotkey_stack, (void *)default_hotkeys_list);
	
	new_hotkey(MAKE_CTRL, MAKE_L, -1, change_mode, MODE_LINE, default_hotkeys_list);
	new_hotkey(MAKE_CTRL, MAKE_D, -1, change_mode, MODE_DRAWING, default_hotkeys_list );
	new_hotkey(MAKE_CTRL, MAKE_F, -1, change_mode, MODE_PAINT_FORE, default_hotkeys_list);
	new_hotkey(MAKE_CTRL, MAKE_B, -1, change_mode, MODE_PAINT_BACK, default_hotkeys_list);
	new_hotkey(MAKE_CTRL, MAKE_H, -1, change_mode, MODE_CHANGE_CHAR, default_hotkeys_list);
	//new_hotkey(MAKE_F1, -1	, -1, show_brush_select, NULL, default_hotkeys_list);
	new_hotkey(MAKE_CTRL, MAKE_N, -1, show_confirm_clear, -1, default_hotkeys_list);
}

void destroy_input()
{
	destroy_list(default_hotkeys_list);
	destroy_list(active_hotkey_stack);
}

void destroy_hotkey_list(LIST * hotkey_list)
{
	int i;
	for(i = 0; i < len_list(hotkey_list); i++)
	{
		free(get_list_at(hotkey_list, i));
	}

	destroy_list(hotkey_list);
}

void push_hotkey_list(LIST * hotkey_list)
{
	append_list(active_hotkey_stack, (void *)hotkey_list);
}

void pop_hotkey_list()
{
	destroy_hotkey_list(get_active_hotkey_list());
	remove_list_last(active_hotkey_stack);
}


