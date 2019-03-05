#ifndef CGA_H
#define CGA_H

#include "geometry.h"

#define PAGE_0 			0xB8000000L
#define PAGE_1			0xB8000FA0L
#define PAGE_2 			0xB8001F40L
#define PAGE_3			0xB8002EE0L

#define PAGE_SIZE		4000

#define END_OF_VMEM 	0xB8003E80L

#define SCREEN_SIZE_X 80
#define SCREEN_SIZE_Y 25

int sign(int value);

unsigned char far * get_page_address(int page_number);

void show_page(int page_number);

void set_graphics_mode();

void fill_screen(unsigned char character, unsigned char attribute, int page);

void copy_area_from_image(	unsigned char far * source, unsigned char far * destination, 
							RECT area_source, RECT image_source, RECT area_dest, RECT image_dest);
void copy_page(int page_source, int page_dest);

#endif