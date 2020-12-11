#define F_CPU 16000000
#define BAUD 57600
#define MyUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>
#include "UART/uart.h"
#include "I2C/i2c.h"
#include "LCD/lcd.h"
#include "FPS/fps.h"

void checkFinger();
void newFinger(uint8_t id);
void deleteFinger(uint8_t id);
uint8_t getID();
uint16_t getTemplateCount();

bool scanFinger = false, enrollFinger = false, removeFinger = false, doorLocked = true, mainUser = false, getID_bool = false;
uint8_t id;
uint8_t proby = 0;
uint16_t mainUserID = 1;
uint8_t tmp[5];
int main(void){
	/*
	*Wstepne parametry programu
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
	lcd_send_string(" Drzwi zamkniete");
	

				
	while(1)
	{
		if(doorLocked){
			if(bit_is_clear(PINB, 3) && !(scanFinger)){
				scanFinger = true;
				proby = 0;
				while(scanFinger){
					checkFinger();
				}
			}
			if(bit_is_clear(PINB, 4) && !(enrollFinger)){
				id = getID();
				newFinger(id);
			}
			if(bit_is_clear(PINB, 5) && !(removeFinger)){
				id = getID();
				deleteFinger(id);
			}
		}
		else{
			if(bit_is_clear(PINB, 3)){
				doorLocked = true;
				lcd_setCursor(0,0);
				lcd_send_string(" Drzwi zamkniete");
			}
		}
	}
	return 0;
}

void newFinger(uint8_t id){
	
}

void deleteFinger(uint8_t id){
	
}

void checkFinger(){
	fps_read_finger();
	if(tmp[0] == 0x00){
		fps_img2TZ(1);
		if(tmp[0] == 0x00){
			fps_search();
			if(tmp[0] == 0x00){
				if(enrollFinger){
					uint16_t mainID = tmp[1] << 8;
					mainID += tmp[2];
					if(mainID == mainUserID){
						mainUser = true;
						scanFinger = false;
					}
					else{
						enrollFinger = false;
						mainUser = false;
						scanFinger = false;
					}
				}
				else{
				uint16_t score;
				score = tmp[3] << 8;
				score += tmp[4];
				
				lcd_clear();
				lcd_setCursor(0,0);
				lcd_send_string("Znaleziono palec");
				lcd_setCursor(0,1);
				lcd_send_string(" Zgodnosc: ");
				lcd_setCursor(11,1);
				lcd_send_string((char*) score);
				lcd_setCursor(15,1);
				lcd_send_string("%");
				//TODO- otwieranie elektrozamka
				_delay_ms(5000);
				lcd_clear();
				lcd_setCursor(0,0);
				lcd_send_string("--Drzwi otwarte--");
				scanFinger = false;
				doorLocked = false;
				}
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
			_delay_ms(2000);
			scanFinger = false;
		}
	}
}

	
uint8_t getID(){
	uint8_t id = 1;
	
	lcd_clear();
	lcd_setCursor(0,0);
	lcd_send_string("Wybierz ID:");
	lcd_setCursor(0,11);
	lcd_send_string((char*) id);
	while(getID_bool){
		if(bit_is_clear(PINB, 3)){
			if(id == 255){
				lcd_setCursor(0,11);
				lcd_send_string((char*) 255);
			}
			else{	
				id++;
				lcd_setCursor(0,11);
				lcd_send_string((char*) id);	
			}
		}
		else if(bit_is_clear(PINB, 4)){
			if(id == 10){
				id--;
				lcd_clear();
				lcd_setCursor(0,0);
				lcd_send_string("Wybierz ID:");
				lcd_setCursor(0,11);
				lcd_send_string((char*) id);
			}
			else if(id == 0){
				lcd_setCursor(0,11);
				lcd_send_string((char*) 0);
			}
			else{
				id--;
				lcd_setCursor(0,11);
				lcd_send_string((char*) id);
			}
		}
		else if(bit_is_clear(PINB, 5)){
			getID_bool = false;
			return id;
		}
	}

}

uint16_t getTemplateCount(){
	uint16_t template;
	fps_templateNum();
	template = tmp[1] << 8;
	template += tmp[2];
	return template;
}
