#define F_CPU 16000000
#define BAUD 57600
#define MyUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>
#include "UART/uart.h"
#include "I2C/i2c.h"
#include "LCD/lcd.h"
#include "FPS/fps.h"


int main(void){

	lcd_init();
	_delay_ms(50);
	initUART();
	
	lcd_setCursor(0,0);
	lcd_clear();
	lcd_send_string("    Witaj w:    ");
	lcd_setCursor(0,1);
	lcd_send_string(" Zamku szyfrowym");
	_delay_ms(2000);
	lcd_clear();
	lcd_setCursor(0,0);

				
	while(1)
	{	

	}
	return 0;
}
