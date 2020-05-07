#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "cga.h"

int shown_page = 0;

unsigned char far * draw_start_offset = (unsigned char far *)PAGE_0;
RECT draw_rect;

int sign(int value)
{
	if(value >= 0)
	{
		return +1;
	}
	else
	{
		return -1;
	}
}

unsigned char far * get_page_address(int page_number)
{
	unsigned char far * base_address;

	base_address = (unsigned char far*)(PAGE_0 + page_number * PAGE_SIZE);

	return base_address;
}

void show_page(int page_number)
{
	long address;

	unsigned char low_address;
	unsigned char high_address;

	address = page_number*(PAGE_SIZE/2);

	low_address = 	(unsigned char)(address & 0x00FF);
	high_address = 	(unsigned char)((address & 0xFF00)>>8);

	_asm {
			mov dx, 0x03d4
			mov al, 0x0D
			xor ah, ah
			out dx, al
			mov dx, 0x03d5
			mov al, low_address
			xor ah, ah
			out dx, al

			mov dx, 0x03d4
			mov al, 0x0C
			xor ah, ah
			out dx, al
			mov dx, 0x03d5
			mov al, high_address
			xor ah, ah
			out dx, al
		 }

	shown_page = page_number;
}

void set_graphics_mode()
{

	_asm {
			mov ax, 0x03
			xor ah, ah
			int 10h
		 }

	show_page(0);
}

void fill_screen(unsigned char character, unsigned char attribute, int page)
{
	int i;
	long int base_address;

	base_address = PAGE_0 + page * PAGE_SIZE;

	for(i = 0; i < SCREEN_SIZE_X * SCREEN_SIZE_Y; i++)
	{
		*((unsigned char far *)base_address + (i * 2))  = character;
		*((unsigned char far *)base_address + (i * 2) + 1)  = attribute;
	}
}

void set_draw_start_offset(unsigned char far * draw_location)
{
	draw_start_offset = draw_location;
} 

void set_draw_rect(RECT rect)
{
	draw_rect = rect;
}

void set_char(int posX, int posY, unsigned char character)
{

	*((unsigned char far *)draw_start_offset + ((posX + draw_rect.x) * 2) + ((posY + draw_rect.y) * (draw_rect.size_x * 2))    )  = character;
}

void set_attribute(int posX, int posY, unsigned char attribute)
{

	*((unsigned char far *)draw_start_offset+ ((posX + draw_rect.x) * 2) + ((posY + draw_rect.y) * (draw_rect.size_x * 2)) + 1)  = attribute;	
}

void set_screen_element(int posX, int posY, unsigned char character, unsigned char attribute)
{
	set_char(posX, posY, character);
	set_attribute(posX, posY, attribute);
}

void draw_line(int start_x, int start_y, int end_x, int end_y, unsigned char character, unsigned char attribute)
{
	float delta_x;
	float delta_y;
	float delta_error;
	float error;

	int temp;
	int y;
	int x;

	if(start_x > end_x)
	{
		temp = start_x;
		start_x = end_x;
		end_x = temp;

		temp = start_y;
		start_y = end_y;
		end_y = temp;
	}

	delta_x = (float)(end_x - start_x);
	delta_y = (float)(end_y - start_y);

	if(fabs(delta_x) > fabs(delta_y))
	{
		delta_error = fabs(delta_y/delta_x);

		error = 0.0f;

		y = start_y;

		for(x = start_x; x <= end_x ; x++)
		{
			set_screen_element(x, y, character, attribute);
			error = error + delta_error;
			if(error >= 0.5f)
			{
				y = y + sign(delta_y);
				error -= 1.0f;
			}
		} 	
	}
	else
	{
		delta_error = fabs(delta_x/delta_y);

		error = 0.0f;

		x = start_x;

		for(y = start_y; y < end_y; y++)
		{
			set_screen_element(x, y, character, attribute);
			error = error + delta_error;
			if(error >= 0.5f)
			{
				x = x + sign(delta_x);
				error -= 1.0f;
			}
		} 
	}	
}

void print_string(int posX, int posY, char* string, unsigned char attribute)
{
	int len;
	int i;

	len = strlen(string);

	for(i = 0; i < len; i++)
	{
		set_screen_element(posX + i, posY, string[i], attribute);
	}
}

void print_int(int posX, int posY, int value, unsigned char attribute)
{
	char buffer [16];

	print_string(posX, posY, itoa(value, buffer, 10), attribute);
}

unsigned char get_char(int posX, int posY)
{
	return *((unsigned char far *)draw_start_offset + (posX * 2) + (posY * (draw_rect.size_x * 2)));
}

unsigned char get_attribute(int posX, int posY)
{
	return *((unsigned char far *)draw_start_offset + (posX * 2) + (posY * (draw_rect.size_x * 2)) + 1);
}

void copy_area_from_image(	unsigned char far * source, unsigned char far * destination, 
							RECT area_source, RECT image_source, RECT area_dest, RECT image_dest)
{
	int x, y;
	unsigned char current_element;
	for(x = 0; x < area_source.size_x; x++)
	{
		for(y = 0; y < area_source.size_y; y++)
		{
			current_element = *(source + (area_source.x + x)*2 + (area_source.y + y)*(image_source.size_x)*2);
			*(destination + (area_dest.x + x)*2 + (area_dest.y + y)*(image_dest.size_x)*2) = current_element;

			current_element = *(source + (area_source.x + x)*2 + (area_source.y + y)*(image_source.size_x)*2 + 1);
			*(destination + (area_dest.x + x)*2 + (area_dest.y + y)*(image_dest.size_x)*2 + 1) = current_element;
		}
	}
}

void copy_page(int page_source, int page_dest)
{
	copy_area_from_image(	get_page_address(page_source), get_page_address(page_dest), 
							rect(0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y), 
							rect(0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y),
							rect(0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y),
							rect(0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y) );

}
