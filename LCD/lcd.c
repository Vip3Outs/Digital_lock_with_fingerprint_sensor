#include "lcd.h"
#include <util/delay.h>

uint8_t value;

void lcd_transmit_header(char tmp){
	i2c_send_packet(value |= 0x04, SLAVE_WRITE_ADDRESS);
	_delay_us(50);
	i2c_send_packet(value | (tmp << 4), SLAVE_WRITE_ADDRESS);
	i2c_send_packet(value &= ~0x04, SLAVE_WRITE_ADDRESS);
	_delay_us(50);
}

void lcd_send_byte(char tmp){
	i2c_send_packet(value &= ~0x01, SLAVE_WRITE_ADDRESS); 
	lcd_transmit_header(tmp >> 4);
	lcd_transmit_header(tmp);
}

void lcd_send_char(char tmp){
	i2c_send_packet (value |= 0x01, SLAVE_WRITE_ADDRESS);
	lcd_transmit_header(tmp >> 4);
	lcd_transmit_header(tmp);
}

void lcd_init(){
	value = 0;
	i2c_init();
	_delay_ms(15);
	lcd_transmit_header(0b00000011);	//interfejs 4bitowy
	_delay_ms(4);
	lcd_transmit_header(0b00000011);				
	_delay_us(100);							
	lcd_transmit_header(0b00000011);
	_delay_ms(1);				
	lcd_transmit_header(0b00000010);
	_delay_ms(1);			
	lcd_send_byte(0b00101000);			//rozmiar 16x2 i 5x8dot (0x28)	
	_delay_ms(1);
	lcd_send_byte(0b00001100);			//wylaczenie kursora i miganie kursora (0x0C)
	_delay_ms(1);
	lcd_send_byte(0b00000110);			//autoinkrementacja kursora (0x06)
	_delay_ms(1);
	i2c_send_packet(value |= 0x08, SLAVE_WRITE_ADDRESS);	//podswietlenie (backlight)
	i2c_send_packet(value &=~ 0x02, SLAVE_WRITE_ADDRESS);
}

void lcd_clear(){
	lcd_send_byte(0x01);
	_delay_us(1500);
}

void lcd_setCursor(char col, char row){
	char tmp;
	tmp = 0x40 * row + col;
	tmp |= 0x80;
	lcd_send_byte(tmp);
}

void lcd_send_string(const char *str){
	while(*str != 0)
	lcd_send_char(*str++);
}

void lcd_send_info(const char *line1, const char *line2){
	lcd_clear();
	lcd_setCursor(0,0);
	lcd_send_string(line1);
	lcd_setCursor(0,1);
	lcd_send_string(line2);
}