#ifndef UI_ELEM_H
#define UI_ELEM_H

#include "geometry.h"
#include "list.h"

#define WINDOW_TOP_RIGHT 0xBB
#define WINDOW_BOT_RIGHT 0xBC
#define WINDOW_BOT_LEFT 0xC8
#define WINDOW_TOP_LEFT 0xC9

#define WINDOW_HOR 0xCD
#define WINDOW_VER 0xBA

#define BLOCK 0xDB

#define BLINK_RATE 1.0f

typedef struct WINDOW_
{
	RECT bounds;
	int border_color_fore;
	int border_color_back;
	int fill_color;
	LIST * textbox_list;
	LIST * button_list;
}
WINDOW;

typedef struct BUTTON_
{
	WINDOW * parent_window;

	RECT on_click_area;
	void (* on_click)();
	int close_window;
}
BUTTON;

typedef struct TEXTBOX_
{
	RECT text_area;
	char * text;
}
TEXTBOX;

LIST * get_active_windows();

int get_active_windows_amount();

WINDOW * window(RECT rect, int border_color_fore, int border_color_back, int fill_color);

void add_button_to_window(BUTTON * button, WINDOW * window);

void add_textbox_to_window(TEXTBOX * textbox, WINDOW * window);

BUTTON * button(RECT on_click_area, void (* on_click)(), int close_window);

TEXTBOX * textbox(RECT bounds, char * text);

void show_dialog_yn(RECT bounds, char * text, void (* yes_button_action)(), void (* no_button_action)());

void draw_shadow(int screen_pos_x, int screen_pos_y);

void draw_window(WINDOW * window);

void draw_window_shadowed(WINDOW * window);

void blink_cursor();

void init_ui();

#endif