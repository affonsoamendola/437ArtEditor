#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "cga.h"

int shown_page = 0;

RECT rect(int x, int y, int size_x, int size_y)
{
	RECT rectangle;

	rectangle.x = x;
	rectangle.y = y;
	rectangle.size_x = size_x;
	rectangle.size_y = size_y;

	return rectangle;
}

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

void set_char_on_rect_at(unsigned char far * destination, int posX, int posY, RECT rect, unsigned char character)
{

	*((unsigned char far *)destination + ((posX + rect.x) * 2) + ((posY + rect.y) * (rect.size_x * 2))    )  = character;
}

void set_attribute_on_rect_at(unsigned char far * destination, int posX, int posY, RECT rect, unsigned char attribute)
{

	*((unsigned char far *)destination + ((posX + rect.x) * 2) + ((posY + rect.y) * (rect.size_x * 2)) + 1)  = attribute;	
}

void draw_char_on_rect_at(unsigned char far * destination, int posX, int posY, RECT rect, unsigned char character, unsigned char attribute)
{
	set_char_on_rect_at(destination, posX, posY, rect, character);
	set_attribute_on_rect_at(destination, posX, posY, rect, attribute);
}

void draw_char_on_page(int posX, int posY, unsigned char character, unsigned char attribute, int page)
{
	
	draw_char_on_rect_at(get_page_address(page), posX, posY, rect(0,0, SCREEN_SIZE_X, SCREEN_SIZE_Y), character, attribute);
}

void draw_line_on_rect_at(unsigned char far * destination, int start_x, int start_y, int end_x, int end_y, RECT rect, unsigned char character, unsigned char attribute)
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
			draw_char_on_rect_at(destination, x, y, rect, character, attribute);
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
			draw_char_on_rect_at(destination, x, y, rect, character, attribute);
			error = error + delta_error;
			if(error >= 0.5f)
			{
				x = x + sign(delta_x);
				error -= 1.0f;
			}
		} 
	}	
}

void print_string_on_rect_at(unsigned char far * destination, int posX, int posY, RECT rect, char* string, unsigned char attribute)
{
	int len;
	int i;

	len = strlen(string);

	for(i = 0; i < len; i++)
	{
		draw_char_on_rect_at(destination, posX + i, posY, rect, string[i], attribute);
	}
}

unsigned char get_character_on_rect_at(unsigned char far * source, int posX, int posY, RECT rect)
{
	return *((unsigned char far *)source + (posX * 2) + (posY * (rect.size_x * 2)));
}

unsigned char get_attribute_on_rect_at(unsigned char far * source, int posX, int posY, RECT rect)
{
	return *((unsigned char far *)source + (posX * 2) + (posY * (rect.size_x * 2)) + 1);
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