#include <stdio.h>
#include <stdlib.h>

#include "ui_elem.h"
#include "list.h"
#include "cga.h"
#include "canvas.h"
#include "keyb.h"

WINDOW * show_dialog_2b(RECT bounds, char * text,
						char * button0_text,
						char * button1_text,
						void (* button0_action)(), 
						void (* button1_action)())
{
	WINDOW * dialog;

	dialog = window(bounds, 0x07, 0x05, 0x05);

	add_textbox_to_window(	textbox(	rect(bounds.x + 1, bounds.y + 1, bounds.size_x - 2, bounds.size_y - 4), 
										text,
										0), 	
							dialog);
	

	add_button_to_window(	button(		rect(bounds.x + 1, bounds.y + bounds.size_y-2, 3, 1), 				
										0x07, 0x07, 0x07, 
										button0_text, 
										button0_action, 1),		
							dialog);

	add_button_to_window(	button(		rect(bounds.x + bounds.size_x - 4, bounds.y + bounds.size_y-2, 3, 1), 
										0x07, 0x07, 0x07, 
										button1_text,
										button1_action, 1),		
							dialog); 

	return dialog;
}

void show_confirm_clear()
{
	WINDOW * dialog = show_dialog_2b(	rect(SCREEN_SIZE_X/2-7, SCREEN_SIZE_Y/2-2, 14, 4), 
										"Are you sure you want to clear the screen?",
										"Yes",
										"No",
										clear_canvas,
										clear_canvas);

	new_hotkey(MAKE_Y, -1, -1, clear_canvas, -1, dialog->hotkey_list);
	new_hotkey(MAKE_N, -1, -1, clear_canvas, -1, dialog->hotkey_list);
}
