#include <stdio.h>
#include <stdlib.h>

#define PAGE_0 			0xB8000000L
#define PAGE_SIZE		4000
#define END_OF_VMEM 	0xB8003E80L
#define SCREEN_SIZE_X 80
#define SCREEN_SIZE_Y 25

unsigned char far * base_address = (unsigned char far *)PAGE_0;
int shown_page = 0;

void set_graphics_mode()
{
	{
		_asm {
				mov ax, 0x03
				xor ah, ah
				int 10h
			 }
	}
}

void show_page(int page_number)
{
	int address;

	unsigned char low_address;
	unsigned char high_address;

	address = page_number*PAGE_SIZE;

	low_address = 	(unsigned char)(address & 0x00FF);
	high_address = 	(unsigned char)((address & 0xFF00)>>8);

	_asm {
			mov dx, 0x03d4
			mov al, 0x0D
			xor ah, ah
			out dx, al
			mov dx, 0x03d5
			mov al, low_address
			out dx, al

			mov dx, 0x03d4
			mov al, 0x0C
			xor ah, ah
			out dx, al
			mov dx, 0x03d5
			mov al, high_address
			out dx, al
		 }

	shown_page = page_number;
}

void set_character(int posX, int posY, unsigned char character, unsigned char attribute, int page)
{
	*(base_address + (posX * 2) + (long)(posY * (SCREEN_SIZE_X * 2)) + (long)(page * PAGE_SIZE)) 	  = character;
	*(base_address + (posX * 2) + (long)(posY * (SCREEN_SIZE_X * 2)) + (long)(page * PAGE_SIZE) + 1)   = attribute;
}

void main()
{
	set_graphics_mode();
	set_character(0, 0, 'X', 0x0E, 1);
	getch();
	show_page(1);
	getch();
}