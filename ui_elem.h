#define WINDOW_TOP_RIGHT 0xBB
#define WINDOW_BOT_RIGHT 0xBC
#define WINDOW_BOT_LEFT 0xC8
#define WINDOW_TOP_LEFT 0xC9

#define WINDOW_HOR 0xCD
#define WINDOW_VER 0xBA

#define BLOCK 0xDB

typedef struct BUTTON_
{
	RECT clickable_area;
	void (* on_click)();
}
BUTTON;

typedef struct TEXTBOX_
{
	RECT bounds;
	char * text;
}
TEXTBOX;

typedef struct WINDOW_
{
	RECT bounds;
	int color_border_fore;
	int color_border_back;
	int color_fill;
	LIST textbox_list;
	LIST button_list;
}
WINDOW;