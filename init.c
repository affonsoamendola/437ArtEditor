#include <stdio.h>
#include <stdlib.h>


#include "init.h"
#include "cga.h"
#include "canvas.h"

void init_sequence()
{
	Keyboard_Install_Driver();
	set_graphics_mode();
	init_canvas();
	init_ui();
	init_input();
}
