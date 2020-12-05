#include "lcd.h"
#include <util/delay.h>


//static struct {
	uint8_t led_pin;
//} lcd;


void putnibble(char t)
{
	t <<= 4;
	i2c_send_packet(led_pin |= 0x04, SLA_WRITE_ADDR);
	_delay_us(50);
	i2c_send_packet(led_pin | t, SLA_WRITE_ADDR);
	i2c_send_packet(led_pin &= ~0x04, SLA_WRITE_ADDR);
	_delay_us(50);
}

void lcd_send_byte(char c)
{
	char highc = 0;
	highc = c >> 4;
	i2c_send_packet(led_pin &=~ 0x01, SLA_WRITE_ADDR); 
	putnibble(highc);
	putnibble(c);
}

void lcd_send_char(char c)
{
	char highc = 0;
	highc = c >> 4;
	i2c_send_packet (led_pin |= 0x01, SLA_WRITE_ADDR);
	putnibble(highc);
	putnibble(c);
}

void lcd_init()
{
	led_pin = 0;
	i2c_init();
	_delay_ms(15);
	putnibble(0b00000011);					//Enter 4bit mode
	_delay_ms(4);
	putnibble(0b00000011);					
	_delay_us(100);							
	putnibble(0b00000011);					
	_delay_ms(1);
	putnibble(0b00000010);
	_delay_ms(1);			
	lcd_send_byte(0x28);		//Select 16x2 LCD in 4Bit mode
	_delay_ms(1);	
	lcd_send_byte(0x0C);		//Display ON Cursor off
	_delay_ms(1);
	lcd_send_byte(0x06);		//Cursor auto increment
	_delay_ms(1);
	i2c_send_packet(led_pin |= 0x08, SLA_WRITE_ADDR);	
	i2c_send_packet(led_pin &=~ 0x02, SLA_WRITE_ADDR);
}

void lcd_clear()
{
	lcd_send_byte(0x01);
	_delay_us(1500);
}

void lcd_setCursor(char col, char row)
{
	char adr;
	adr = 0x40 * row + col;
	adr |= 0x80;
	lcd_send_byte(adr);
}

void lcd_send_string(const char *str)
{
	while(*str != 0)
	lcd_send_char(*str++);
}
