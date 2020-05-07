#ifndef VIEWS_H
#define VIEWS_H

#define VIEW_CANVAS 0
#define VIEW_UI 2
#define VIEW_UI_PEEK 3
#define VIEW_BRUSH_SELECT 1

int get_viewport_x();
int get_viewport_y();

int get_current_view();

void set_current_view(int view);

void show_ui();

void view_canvas();

void view_brush_select();

void move_viewport(int delta_x, int delta_y);

#endif
