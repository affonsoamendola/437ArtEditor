#ifndef CGA_H
#define CGA_H

#define PAGE_0 			0xB8000000L
#define PAGE_1			0xB8000FA0L
#define PAGE_2 			0xB8001F40L
#define PAGE_3			0xB8002EE0L

#define PAGE_SIZE		4000

#define END_OF_VMEM 	0xB8003E80L

#define SCREEN_SIZE_X 80
#define SCREEN_SIZE_Y 25

typedef struct RECT_
{
	int x;
	int y;
	int size_x;
	int size_y;
} 
RECT;

RECT rect(int x, int y, int size_x, int size_y);

#endif