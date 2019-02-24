#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ui_elem.h"
#include "cga.h"

int last_blink_clock = 0;

int blink_counter = 0;
int blink_state = 0;

int current_border_color_fore = 0x0E;
int current_border_color_back = 0x01;

LIST * active_windows;

LIST * get_active_windows()
{

	return (LIST *)active_windows;
}

int get_active_windows_amount()
{
	
	return len_list(active_windows);
}

WINDOW * window(RECT rect, int border_color_fore, int border_color_back, int fill_color)
{
	WINDOW * new_window;

	new_window = (WINDOW *)malloc(sizeof(WINDOW));

	new_window->bounds = rect;
	new_window->border_color_fore = border_color_fore;
	new_window->border_color_back = border_color_back;
	new_window->fill_color		  = fill_color;
	new_window->button_list = create_list();
	new_window->textbox_list = create_list();

	append_list(active_windows, new_window);

	return new_window;
}

void close_window(WINDOW * window)
{
	destroy_list(window->button_list);
	destroy_list(window->textbox_list);
	remove_list(active_windows, window);
}

void add_button_to_window(BUTTON * button, WINDOW * window)
{
	LIST * button_list;

	button_list = window->button_list;

	append_list(button_list, button);
}

void add_textbox_to_window(TEXTBOX * textbox, WINDOW * window)
{
	LIST * textbox_list;

	textbox_list = window->textbox_list;

	append_list(textbox_list, textbox);
}

BUTTON * button(RECT on_click_area, void (* on_click)(), int close_window)
{
	BUTTON * new_button;

	new_button = (BUTTON *)malloc(sizeof(BUTTON));

	new_button->on_click_area = on_click_area;
	new_button->on_click = on_click;

	new_button->close_window = close_window;

	return new_button;
}

TEXTBOX * textbox(RECT text_area, char * text)
{
	TEXTBOX * new_textbox;

	new_textbox =(TEXTBOX *)malloc(sizeof(TEXTBOX));

	new_textbox->text_area = text_area;
	new_textbox->text = text;

	return new_textbox;
}

void draw_shadow(int screen_pos_x, int screen_pos_y)
{
	unsigned char buffer;

	buffer = 	get_attribute_on_rect_at( 	get_page_address(1),
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

	set_attribute_on_rect_at( 	get_page_address(1),
								screen_pos_x, screen_pos_y,
								rect(0,0, SCREEN_SIZE_X, SCREEN_SIZE_Y),
								buffer);
}

void draw_window(WINDOW * window)
{
	int i, j;

	draw_char_on_page(window->bounds.x + window->bounds.size_x-1, 	window->bounds.y,				  			WINDOW_TOP_RIGHT,	window->border_color_fore | (window->border_color_back<<4), 1);
	draw_char_on_page(window->bounds.x + window->bounds.size_x-1, 	window->bounds.y + window->bounds.size_y-1, WINDOW_BOT_RIGHT,	window->border_color_fore | (window->border_color_back<<4), 1);
	draw_char_on_page(window->bounds.x, 							window->bounds.y + window->bounds.size_y-1, WINDOW_BOT_LEFT,	window->border_color_fore | (window->border_color_back<<4), 1);
	draw_char_on_page(window->bounds.x, 			 	   			window->bounds.y, 				  			WINDOW_TOP_LEFT,	window->border_color_fore | (window->border_color_back<<4), 1);

	for(i = window->bounds.x + 1; i < window->bounds.x + window->bounds.size_x - 1; i++)
	{
		draw_char_on_page(i, window->bounds.y,								WINDOW_HOR,	window->border_color_fore | (window->border_color_back<<4), 1);
		draw_char_on_page(i, window->bounds.y + window->bounds.size_y - 1,	WINDOW_HOR,	window->border_color_fore | (window->border_color_back<<4), 1);
	}

	for(i = window->bounds.y + 1; i < window->bounds.y + window->bounds.size_y - 1; i++)
	{
		draw_char_on_page(window->bounds.x, 							i,	WINDOW_VER,	window->border_color_fore | (window->border_color_back<<4), 1);
		draw_char_on_page(window->bounds.x + window->bounds.size_x -1, 	i,	WINDOW_VER, window->border_color_fore | (window->border_color_back<<4), 1);
	}

	for(i = window->bounds.x + 1; i < window->bounds.x + window->bounds.size_x - 1; i++)
	{
		for(j = window->bounds.y + 1; j < window->bounds.y + window->bounds.size_y - 1; j++)
		{
			draw_char_on_page(i,  j,	BLOCK,	window->fill_color, 1);
		}
	}
}

void draw_window_shadowed(WINDOW * window)
{
	int i;

	draw_window(window);

	for(i = window->bounds.x + 1; i < window->bounds.x + window->bounds.size_x + 1; i++)
	{
		draw_shadow(i, window->bounds.y + window->bounds.size_y);
	}

	for(i = window->bounds.y + 1; i < window->bounds.y + window->bounds.size_y; i++)
	{
		draw_shadow(window->bounds.x + window->bounds.size_x, i);
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

void init_ui()
{

	active_windows = create_list();
}