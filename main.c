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

void newFinger(uint16_t id);
void deleteFinger(uint16_t id);
void checkFinger();
uint16_t getID();

bool scanFinger = false, enrollFinger = false, removeFinger = false, doorLocked = true;
uint16_t id;
uint8_t proby = 0;
uint8_t tmp [3];
int main(void){
	/*
	*Wst?pne parametry programu
	*/

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
		if(doorLocked){
			if(PORTB1 == 0 && !(scanFinger)){
				scanFinger = true;
				proby = 0;
				while(scanFinger){
					checkFinger();
				}
			}
			if(PORTB2 == 0 && !(enrollFinger)){
				id = getID();
				newFinger(id);
			}
			if(PORTB3 == 0 && !(removeFinger)){
				id = getID();
				deleteFinger(id);
			}
		}
		else{

		}
	}
	return 0;
}

void newFinger(uint16_t id){
	
}

void deleteFinger(uint16_t id){
	
}

void checkFinger(){
	fps_read_finger();
	if(tmp[0] == 0x00){
		fps_img2TZ(1);
		if(tmp[0] == 0x00){
			fps_search();
			if(tmp[0] == 0x00){
				uint16_t score;
				score = tmp[1] << 8;
				score += tmp[2];
				
				lcd_clear();
				lcd_setCursor(0,0);
				lcd_send_string("Znaleziono palec");
				lcd_setCursor(0,1);
				lcd_send_string(" Zgodnosc: ");
				lcd_setCursor(11,1);
				lcd_send_string(score);
				lcd_setCursor(15,1);
				lcd_send_string("%");
				_delay_ms(2000);
				//TODO- otwieranie elektrozamka
				scanFinger = false;
				doorLocked = false;
			}
			else{
				lcd_clear();
				lcd_setCursor(0,0);
				lcd_send_string(" Nie znaleziono ");
				lcd_setCursor(0,1);
				lcd_send_string("palca w systemie");
				_delay_ms(3000);
				return;
			}
		}
		else{
			lcd_clear();
			lcd_setCursor(0,0);
			lcd_send_string(" Blad tworzenia ");
			lcd_setCursor(0,1);
			lcd_send_string("  obrazu palca  ");
			_delay_ms(3000);
			return;
		}
	}
	else{
		lcd_clear();
		lcd_setCursor(0,0);
		lcd_send_string("   Brak palca!  ");
		lcd_setCursor(0,1);
		lcd_send_string("Sprobuj ponownie");
		_delay_ms(1500);
		proby++;
		if(proby == 4){
			lcd_clear();
			lcd_setCursor(0,0);
			lcd_send_string("Zbyt wiele prob!");
			lcd_setCursor(0,1);
			lcd_send_string("****************");
			_delay_ms(1500);
			scanFinger = false;
		}
	}
}
	

uint16_t getID(){
	uint16_t id = 0;
	
	return id;
}

