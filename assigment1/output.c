#include "output.h"
#include "helper.h"
 /*

 This CODE IS TAKEN DIRECLTY FROM OSDEV.ORG, which was recommended by Professor West.
 I WENT THROUGH tHE CODE TO MAKE SURE THAT I UNDERSTOOD EVERYTHING..
 */
/* Hardware text mode color constants. */
 
uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}


static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize()
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_RED, COLOR_WHITE);
	terminal_buffer = (uint16_t*) 0xB8000;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
}
  void update_cursor(int row, int col)
 {
    unsigned short position=(row*80) + col;

    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
 }

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	 size_t index = y * VGA_WIDTH + x;
	if(index >= VGA_HEIGHT*VGA_WIDTH){


		for(int i=0;i<VGA_WIDTH*(VGA_HEIGHT-1);i++){
		 terminal_buffer[i]=terminal_buffer[i+VGA_WIDTH];
		}
		for(int i=VGA_WIDTH*(VGA_HEIGHT-1) ; i<VGA_WIDTH*VGA_HEIGHT;i++)
		{
	     terminal_buffer[i]= make_vgaentry(' ', terminal_color);
		}
		terminal_row--;
		index=index- VGA_WIDTH;
	}

	terminal_buffer[index] = make_vgaentry(c, color);

}

void terminal_putchar(char c)

{   if(c=='\n'){
    ++terminal_row;
    terminal_column=0;
}else{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if ( ++terminal_column == VGA_WIDTH )
	{
		terminal_column = 0;
		++terminal_row;

	}

}
size_t index = terminal_row* VGA_WIDTH + terminal_column;
    if(index < VGA_HEIGHT*VGA_WIDTH){
update_cursor(terminal_row,terminal_column);
}

}

void terminal_writestring(const char* data)
{
	size_t datalen = strlen(data);
	for ( size_t i = 0; i < datalen; i++ )
		terminal_putchar(data[i]);
}

char * itoa( multiboot_uint32_t value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}
void newline(){
	terminal_row++;
	terminal_column = 0;
}

void prints(const char * str){
	terminal_writestring(str);
}
