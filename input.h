#ifndef INPUT_H
#define INPUT_H

#include "geometry.h"

#define MODE_DRAWING 1
#define MODE_LINE 2
#define MODE_PAINT_BACK 3
#define MODE_PAINT_FORE 4
#define MODE_CHANGE_CHAR 5

typedef struct HOTKEY_
{
	int keys[3];
	int enabled;

	void (* hotkey_action)(int option);
	int hotkey_action_option;
}
HOTKEY;

typedef int HOTKEY_INDEX; 

int get_current_mode();

int get_cursor_x();
int get_cursor_y();

unsigned char get_selected_attribute();

void edge_event(int delta_x, int delta_y);
void move_cursor(int delta_x, int delta_y);

int inside_rect(int x, int y, RECT rect);

void brush_select_screen_cursor_action();
void change_mode(int mode);

void cursor_action();

void input_cursor();
void input_brush_select();
void input_canvas_view();
void input_ui();
void input_ui_peek();

void handle_input();

#endif