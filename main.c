/*
 * arduino_bootloader.c
 *
 * Created: 11/1/2020 10:04:45 AM
 * Author : Karol
 */ 

#define F_CPU 16000000
#define BAUD 57600
#define MyUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "UART/uart.h"
#include "I2C/i2c.h"
#include "LCD/lcd.h"
#include "FPS/fps.h"

uint8_t id;

uint8_t getFingerprintEnroll();
uint8_t readNumber();

int main(void){

	lcd_init();
	initUART();
	
			lcd_setCursor(0,0);
			lcd_clear();
			lcd_send_string("Hello world!");
			_delay_ms(2000);
			lcd_setCursor(0,1);
			lcd_send_string("---DZIALA---");
			lcd_clear();
			lcd_setCursor(0,0);

				
	while(1)
	{	

	}
	return 0;
}
