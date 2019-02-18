#include <stdio.h>
#include <stdlib.h>

#include "ui_elem.h"
#include "list.h"

void show_dialog_yn(RECT bounds, char * text, void (* yes_button_action)(), void (* no_button_action)())
{
	WINDOW * dialog;

	dialog = window(bounds, 0x07, 0x05, 0x05);

	add_textbox_to_window(	textbox(	rect(bounds.x + 1, 					bounds.y + 1, 					bounds.size_x - 2, 	bounds.size_y - 4), text), dialog);
	add_button_to_window(	button(		rect(bounds.x + 1, 					bounds.y + bounds.size_y-2, 	3, 					1), 				yes_button_action), dialog); 
	add_button_to_window(	button(		rect(bounds.x + bounds.size_x - 4, 	bounds.y + bounds.size_y-2, 	3, 					1), 				no_button_action), dialog); 

	append_list(get_active_windows(), dialog);
}
