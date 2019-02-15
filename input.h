typedef struct BUTTON_
{
	RECT clickable_area;
	void (* on_click)();
}
BUTTON;

#define MODE_DRAWING 0
#define MODE_LINE 1
#define MODE_PAINT_BACK 2
#define MODE_PAINT_FORE 3
#define MODE_CHANGE_CHAR 4