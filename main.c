#define F_CPU 16000000
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
uint8_t getID();
void newFinger(uint8_t id);
void deleteFinger(uint8_t id);
uint16_t getTemplateCount();


volatile bool scanFinger = false, enrollFinger = false, removeFinger = false, doorLocked = true, mainUser = false, getID_bool = false, firstUse = false;
uint8_t id;
uint16_t mainUserID = 1;
uint8_t buffer[3];
int main(void){
	//Wstepne parametry programu
	lcd_init();
	_delay_ms(50);
	initUART();
	
	DDRB = 0b00001000; //B0,B1,B2 - 3 przyciski; B3 - wyjscie elektrozamka
	PORTB = 0b00000111;
	lcd_send_info("    Witaj w:    ", "Zamku szyfrowym ");
	_delay_ms(2000);
	
	if(getTemplateCount() == 0){
		firstUse = true;
		while(firstUse){
			lcd_send_info("Pierwsze uzycie ", "Admin potrzebny!");
			_delay_ms(1500);
			newFinger(mainUserID);
		}
	}
	lcd_send_info("Drzwi zamkniete", "");
	
	//Glowna petla programu		
	while(1){
		if(doorLocked){
			if(bit_is_clear(PINB, 0) && !(scanFinger)){
				scanFinger = true;
					checkFinger();

			}
			if(bit_is_clear(PINB, 1) && !(enrollFinger)){
				enrollFinger = true;
				lcd_send_info("Wprowadz odcisk","    dla: ADMIN   ");
				_delay_ms(1000);
				checkFinger();
				if(mainUser){
					id = getID();
					if(id == mainUserID){
						lcd_send_info("  Niedozwolona  ", "operacja!(ADMIN)");
						_delay_ms(2000);
						enrollFinger = false;
						lcd_send_info("Drzwi zamkniete", "");
					}
					else{
						newFinger(id);
					}
				}
				else{
					lcd_send_info("Palec nie nalezy", "    do: ADMIN   ");
					enrollFinger = false;
					_delay_ms(2000);
					lcd_send_info("Drzwi zamkniete", "");
				}
			}
			if(bit_is_clear(PINB, 2) && !(removeFinger)){
				removeFinger = true;
				lcd_send_info("Wprowadz odcisk","    dla: ADMIN   ");
				_delay_ms(1000);
				checkFinger();
				if(mainUser){
					id = getID();
					if(id == mainUserID){
						lcd_send_info("  Czy nadpisac  ", "palec dla ADMIN?");
						_delay_ms(2000);
						lcd_send_info("B1- NIE  B2- TAK", "");
						while(true){
							if(bit_is_clear(PINB, 0)){
								lcd_send_info("Drzwi zamkniete", "");
								removeFinger = false;
								mainUser = false;
								_delay_ms(200);
								break;
							}
							else if(bit_is_clear(PINB, 2)){
								newFinger(mainUserID);
								mainUser = false;
								lcd_send_info("Drzwi zamkniete", "");
								break;
							}
						}
					}
					else{
						deleteFinger(id);
						mainUser = false;
					}
				}
				else{
					lcd_send_info("Palec nie nalezy", "    do: ADMIN   ");
					removeFinger = false;
					_delay_ms(2000);
					lcd_send_info("Drzwi zamkniete", "");
				}
			}
		}
		//gdy drzwi sa otwarte
		else if(!doorLocked){
			if(bit_is_clear(PINB, 0)){
				doorLocked = true;
				PORTB ^= 1 << PINB3;
				lcd_send_info("Drzwi zamkniete", "");
				_delay_ms(1000);
			}
		}
	}
	return 0;
}//Koniec main.c

void newFinger(uint8_t id){
	lcd_send_info("  Umiesc palec  ", "Skanowanie:-----");
	fps_read_finger();
	uint8_t timer = 0;
	while(tmp[0] == 0x02){
		fps_read_finger();
		_delay_ms(100);
		timer += 1;
		if(timer == 10){
			lcd_send_info("  Umiesc palec  ", "Skanowanie: ----");
		}
		if(timer == 20){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:  ---");
		}
		if(timer == 30){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:   --");
		}
		if(timer == 40){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:    -");
		}
		if(timer == 49){
			lcd_send_info("  Brak palca na ", " czytniku linii ");
			_delay_ms(1500);
			enrollFinger = false;
			lcd_send_info("Drzwi zamkniete", "");
			break;
		}
	}
	if(tmp[0] == 0x00){
		fps_img2TZ(1);
		_delay_ms(100);
		if(tmp[0] == 0x00){
			lcd_send_info("   Raz jeszcze  ", "  umiesc palec  ");
			_delay_ms(1500);
				lcd_send_info("  Umiesc palec  ", "Skanowanie:-----");
				fps_read_finger();
				timer = 0;
				while(tmp[0] == 0x02){
					fps_read_finger();
					_delay_ms(100);
					timer += 1;
					if(timer == 10){
						lcd_send_info("  Umiesc palec  ", "Skanowanie: ----");
					}
					if(timer == 20){
						lcd_send_info("  Umiesc palec  ", "Skanowanie:  ---");
					}
					if(timer == 30){
						lcd_send_info("  Umiesc palec  ", "Skanowanie:   --");
					}
					if(timer == 40){
						lcd_send_info("  Umiesc palec  ", "Skanowanie:    -");
					}
					if(timer == 49){
						lcd_send_info("  Brak palca na ", " czytniku linii ");
						_delay_ms(1500);
						enrollFinger = false;
						lcd_send_info("Drzwi zamkniete", "");
						break;
					}
				}
			if(tmp[0] == 0x00){
				fps_img2TZ(2);
				_delay_ms(500);
				if(tmp[0] == 0x00){
					fps_genModel();
					if(tmp[0] == 0x00){
						fps_storeModel(id);
						if(tmp[0] == 0x00){
							lcd_send_info(" Pomyslny zapis ", " palca w bazie! ");
							firstUse = false;
							_delay_ms(2000);
						}
						else{
							lcd_send_info("  Blad podczas  ", " zapisu do bazy ");
							_delay_ms(2000);
						}
					}
					else{
						lcd_send_info("  Blad podczas  ", "tworzenia modelu");
						_delay_ms(2000);
					}
				}
				else{
					lcd_send_info(" Blad tworzenia ", "  obrazu palca  ");
					_delay_ms(2000);
				}
			}
			else{
				lcd_send_info("   Brak palca!  ", "Sprobuj ponownie");
				_delay_ms(2000);
			}
		}
		else{
			lcd_send_info(" Blad tworzenia ", "  obrazu palca  ");
			_delay_ms(2000);
		}
	}
	else{
		lcd_send_info("   Brak palca!  ", "Sprobuj ponownie");
		_delay_ms(2000);
	}
	enrollFinger = false;
	if(!firstUse){
		lcd_send_info("Drzwi zamkniete", "");
	}	
}

void deleteFinger(uint8_t id){
	fps_deleteModel(id);
	_delay_ms(100);
	if(tmp[0] == 0x00){
		lcd_clear();
		lcd_setCursor(0,0);
		lcd_send_string(" Usunieto palec ");
		lcd_setCursor(0,1);
		lcd_send_string("o ID: ");
		lcd_setCursor(6,1);
		lcd_send_string(itoa(id, buffer, 10));
		_delay_ms(2000);
		lcd_send_info("Drzwi zamkniete", "");
	}
	else{
		lcd_send_info(" Nie udalo sie  ", "  usunac palca! ");
		_delay_ms(2000);
		lcd_send_info("Drzwi zamkniete", "");
	}
	removeFinger = false;
}

void checkFinger(){
	lcd_send_info("  Umiesc palec  ", "Skanowanie:-----");
	fps_read_finger();
	uint8_t timer = 0;
	while(tmp[0] == 0x02){
		fps_read_finger();
		_delay_ms(100);
		timer += 1;
		if(timer == 10){
			lcd_send_info("  Umiesc palec  ", "Skanowanie: ----");
		}
		if(timer == 20){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:  ---");
		}
		if(timer == 30){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:   --");
		}
		if(timer == 40){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:    -");
		}
		if(timer == 50){
			lcd_send_info("  Brak palca na ", " czytniku linii ");
			_delay_ms(1500);
			scanFinger = false;
			enrollFinger = false;
			removeFinger = false;
			lcd_send_info("Drzwi zamkniete", "");
			return;
		}
	}
	if(tmp[0] == 0x00){
		fps_img2TZ(1);
		_delay_ms(100);
		if(tmp[0] == 0x00){
			fps_search();
			_delay_ms(100);
			if(tmp[0] == 0x00){
				if(enrollFinger == true || removeFinger == true){
					uint16_t mainID = tmp[1] << 8;
					mainID += tmp[2];
					if(mainID == mainUserID){
						mainUser = true;
						scanFinger = false;
						removeFinger = false;
					}
					else{
						mainUser = false;
						scanFinger = false;
						removeFinger = false;
					}
				}
				else if(firstUse == true){
					
				}
				else{
					uint16_t score;
					score = tmp[3] << 8;
					score += tmp[4];
					uint8_t percentage;
					char buffor[3];
					lcd_clear();
					lcd_setCursor(0,0);
					lcd_send_string("Znaleziono palec");
					lcd_setCursor(0,1);
					lcd_send_string(" Zgodnosc: ");
					lcd_setCursor(11,1);
					percentage = ((score <<8)/(uint8_t)255) *100;
					lcd_send_string(itoa(percentage, buffor, 10));
					lcd_setCursor(15,1);
					lcd_send_string("%");
					PORTB ^= 1 << PINB3;
					_delay_ms(3000);
					lcd_send_info("--Drzwi otwarte--", "");
					scanFinger = false;
					doorLocked = false;
				}
			}
			else{
				lcd_send_info(" Nie znaleziono ", "palca w systemie");
				_delay_ms(2000);
			}
		}
		else{
			lcd_send_info(" Blad tworzenia ", "  obrazu palca  ");
			_delay_ms(2000);
		}
	}
	else{
		lcd_send_info("   Brak palca!  ", "Sprobuj ponownie");
		_delay_ms(2000);
	}
	removeFinger = false;
	enrollFinger = false;
	scanFinger = false;
	if(doorLocked){
	lcd_send_info("Drzwi zamkniete", "");
	}
	
}
	
uint8_t getID(){
	uint8_t id = 1;
	char buffor[3];
	lcd_clear();
	lcd_setCursor(0,0);
	lcd_send_string("Wybierz ID:");
	lcd_setCursor(0,11);
	lcd_send_string(itoa(id,buffor,10));
	getID_bool = true;
	while(getID_bool){
		if(bit_is_clear(PINB, 1)){
			if(id == 255){
				lcd_setCursor(0,11);
				lcd_send_string(itoa(255,buffor,10));
			}
			else{	
				id++;
				lcd_setCursor(0,11);
				lcd_send_string(itoa(id,buffor,10));
				_delay_ms(200);	
			}
		}
		else if(bit_is_clear(PINB, 2)){
			if(id == 10){
				id--;
				lcd_clear();
				lcd_setCursor(0,0);
				lcd_send_string("Wybierz ID:");
				lcd_setCursor(0,11);
				lcd_send_string(itoa(id,buffor,10));
				_delay_ms(200);
			}
			else if(id == 1){
				lcd_setCursor(0,11);
				lcd_send_string(itoa(1,buffor,10));
			}
			else{
				id--;
				lcd_setCursor(0,11);
				lcd_send_string(itoa(id,buffor,10));
				_delay_ms(200);
			}
		}
		else if(bit_is_clear(PINB, 0)){
			getID_bool = false;
			return id;
			_delay_ms(200);
		}
	}
}

uint16_t getTemplateCount(){
	uint16_t template;
	fps_templateNum();
	_delay_ms(50);
	template = tmp[1] << 8;
	template += tmp[2];
	return template;
}