#define F_CPU 16000000
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
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
volatile uint8_t timer_count = 0;
unsigned char znak1[8] = { 0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x02 }; //a
unsigned char znak2[8] = { 0x02, 0x04, 0x0E, 0x10, 0x10, 0x10, 0x0E, 0x00 }; //c
unsigned char znak3[8] = { 0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x02 }; //e
unsigned char znak4[8] = { 0x0C, 0x04, 0x06, 0x04, 0x04, 0x04, 0x0E, 0x00 }; //l
unsigned char znak5[8] = { 0x02, 0x04, 0x0E, 0x11, 0x11, 0x11, 0x0E, 0x00 }; //o
unsigned char znak6[8] = { 0x02, 0x04, 0x0E, 0x10, 0x0E, 0x01, 0x1E, 0x00 }; //s
unsigned char znak7[8] = { 0x04, 0x00, 0x1F, 0x02, 0x04, 0x08, 0x1F, 0x00 }; //z_kropka
unsigned char znak8[8] = { 0x04, 0x08, 0x1F, 0x02, 0x04, 0x08, 0x1F, 0x00 }; //z_kreska

volatile bool scanFinger = false, enrollFinger = false, removeFinger = false, doorLocked = true, mainUser = false, getID_bool = false, firstUse = false, counter_check = false;
uint8_t id;
uint16_t mainUserID = 1;
uint8_t buffer[3];

int main(void){
	//Wstepne parametry programu
	lcd_init();
	_delay_ms(50);
	uart_init();
	
	DDRB = 0b00001000; //B0,B1,B2 - 3 przyciski; B3 - wyjscie elektrozamka
	PORTB = 0b00000111;

	TCCR1B |= (1 << CS11) | (1 << CS10) | (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	TCNT1 = 0;
	OCR1A = 1249;
	sei();
	
	//wyslanie do LCD polskie znaki
	lcd_custom_char(0, znak1);
	lcd_custom_char(1, znak2);
	lcd_custom_char(2, znak3);
	lcd_custom_char(3, znak4);
	lcd_custom_char(4, znak5);
	lcd_custom_char(5, znak6);
	lcd_custom_char(6, znak7);
	lcd_custom_char(7, znak8);
	
	lcd_send_info("    Witaj w:    ", "Zamku szyfrowym ");
	_delay_ms(2000);
	
	if(getTemplateCount() == 0){
		firstUse = true;
		while(firstUse){
			lcd_send_info("Pierwsze uzycie ", "Admin potrzebny!");
			lcd_special_char(10,0,6);
			_delay_ms(1500);
			newFinger(mainUserID);
		}
	}
	lcd_send_info("Drzwi zamkniete", "");
	lcd_special_char(12,0,2);
	
	//Glowna petla programu		
	while(1){
		if(counter_check){
		if(doorLocked){
			if(bit_is_clear(PINB, 0) && !(scanFinger)){
				scanFinger = true;
					checkFinger();
			}
			
			if(bit_is_clear(PINB, 1) && !(enrollFinger)){
				enrollFinger = true;
				lcd_send_info("Wprowadz odcisk","    dla: ADMIN   ");
				lcd_special_char(7,0,7);
				_delay_ms(1000);
				checkFinger();
				if(mainUser){
					id = getID();
					if(id == mainUserID){
						lcd_send_info("  Niedozwolona  ", "operacja!(ADMIN)");
						_delay_ms(2000);
						enrollFinger = false;
						lcd_send_info("Drzwi zamkniete", "");
						lcd_special_char(12,0,2);
						break;
					}
					else{
						newFinger(id);
					}
				}
				else{
					lcd_send_info("Palec nie nalezy", "    do: ADMIN   ");
					lcd_special_char(14,0,6);
					enrollFinger = false;
					_delay_ms(2000);
					lcd_send_info("Drzwi zamkniete", "");
					lcd_special_char(12,0,2);
				}
			}
			
			if(bit_is_clear(PINB, 2) && !(removeFinger)){
				removeFinger = true;
				lcd_send_info("Wprowadz odcisk","    dla: ADMIN   ");
				lcd_special_char(7,0,7);
				_delay_ms(1000);
				checkFinger();
				if(mainUser){
					id = getID();
					if(id == mainUserID){
						lcd_send_info("  Czy nadpisac  ", "palec dla ADMIN?");
						lcd_special_char(13,0,1);
						_delay_ms(2000);
						lcd_send_info("B1- NIE  B2- TAK", "");
						while(true){
							if(bit_is_clear(PINB, 0)){
								lcd_send_info("Drzwi zamkniete", "");
								lcd_special_char(12,0,2);
								removeFinger = false;
								mainUser = false;
								_delay_ms(200);
								break;
							}
							else if(bit_is_clear(PINB, 1)){
								newFinger(mainUserID);
								mainUser = false;
								lcd_send_info("Drzwi zamkniete", "");
								lcd_special_char(12,0,2);
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
					lcd_special_char(14,0,6);
					removeFinger = false;
					_delay_ms(2000);
					lcd_send_info("Drzwi zamkniete", "");
					lcd_special_char(12,0,2);
				}
			}
		}
		//gdy drzwi sa otwarte
		else if(!doorLocked){
			if(bit_is_clear(PINB, 0)){
				doorLocked = true;
				PORTB &= ~(1 << PINB3);
				lcd_send_info("Drzwi zamkniete", "");
				lcd_special_char(12,0,2);
				_delay_ms(1000);
			}
		}
		}
		counter_check = false;
	}
	return 0;
}//Koniec void main()


void newFinger(uint8_t id){
	lcd_send_info("  Umiesc palec  ", "Skanowanie:-----");
	lcd_special_char(6,0,5);
	lcd_special_char(7,0,1);
	fps_read_finger();
	uint8_t timer = 0;
	while(tmp[0] == 0x02){
		fps_read_finger();
		_delay_ms(100);
		timer += 1;
		if(timer == 10){
			lcd_send_info("  Umiesc palec  ", "Skanowanie: ----");
			lcd_special_char(6,0,5);
			lcd_special_char(7,0,1);
		}
		if(timer == 20){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:  ---");
			lcd_special_char(6,0,5);
			lcd_special_char(7,0,1);
		}
		if(timer == 30){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:   --");
			lcd_special_char(6,0,5);
			lcd_special_char(7,0,1);
		}
		if(timer == 40){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:    -");
			lcd_special_char(6,0,5);
			lcd_special_char(7,0,1);
		}
		if(timer == 49){
			lcd_send_info("  Brak palca na ", " czytniku linii ");
			_delay_ms(1500);
			enrollFinger = false;
			lcd_send_info("Drzwi zamkniete", "");
			lcd_special_char(12,0,2);
			break;
		}
	}
	if(tmp[0] == 0x00){
		fps_img2TZ(1);
		_delay_ms(100);
		if(tmp[0] == 0x00){
			lcd_send_info("   Raz jeszcze  ", "  umiesc palec  ");
			lcd_special_char(6,1,5);
			lcd_special_char(7,1,1);
			_delay_ms(1500);
				lcd_send_info("  Umiesc palec  ", "Skanowanie:-----");
				lcd_special_char(6,0,5);
				lcd_special_char(7,0,1);
				fps_read_finger();
				timer = 0;
				while(tmp[0] == 0x02){
					fps_read_finger();
					_delay_ms(100);
					timer += 1;
					if(timer == 10){
						lcd_send_info("  Umiesc palec  ", "Skanowanie: ----");
						lcd_special_char(6,0,5);
						lcd_special_char(7,0,1);
					}
					if(timer == 20){
						lcd_send_info("  Umiesc palec  ", "Skanowanie:  ---");
						lcd_special_char(6,0,5);
						lcd_special_char(7,0,1);
					}
					if(timer == 30){
						lcd_send_info("  Umiesc palec  ", "Skanowanie:   --");
						lcd_special_char(6,0,5);
						lcd_special_char(7,0,1);
					}
					if(timer == 40){
						lcd_send_info("  Umiesc palec  ", "Skanowanie:    -");
						lcd_special_char(6,0,5);
						lcd_special_char(7,0,1);
					}
					if(timer == 49){
						lcd_send_info("  Brak palca na ", " czytniku linii ");
						lcd_special_char(6,0,5);
						lcd_special_char(7,0,1);
						_delay_ms(1500);
						enrollFinger = false;
						lcd_send_info("Drzwi zamkniete", "");
						lcd_special_char(12,0,2);
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
							lcd_special_char(5,0,5);
							firstUse = false;
							_delay_ms(2000);
						}
						else{
							lcd_send_info("  Blad podczas  ", " zapisu do bazy ");
							lcd_special_char(3,0,3);
							lcd_special_char(4,0,0);
							_delay_ms(2000);
						}
					}
					else{
						lcd_send_info("  Blad podczas  ", "tworzenia modelu");
						lcd_special_char(3,0,3);
						lcd_special_char(4,0,0);
						_delay_ms(2000);
					}
				}
				else{
					lcd_send_info(" Blad tworzenia ", "  obrazu palca  ");
					lcd_special_char(2,0,3);
					lcd_special_char(3,0,0);
					_delay_ms(2000);
				}
			}
			else{
				lcd_send_info("   Brak palca!  ", "Sprobuj ponownie");
				lcd_special_char(3,1,4);
				_delay_ms(2000);
			}
		}
		else{
			lcd_send_info(" Blad tworzenia ", "  obrazu palca  ");
			lcd_special_char(2,0,3);
			lcd_special_char(3,0,0);
			_delay_ms(2000);
		}
	}
	else{
		lcd_send_info("   Brak palca!  ", "Sprobuj ponownie");
		lcd_special_char(3,1,4);
		_delay_ms(2000);
	}
	enrollFinger = false;
	if(!firstUse){
		lcd_send_info("Drzwi zamkniete", "");
		lcd_special_char(12,0,2);
	}	
}

void deleteFinger(uint8_t id){
	fps_deleteModel(id);
	_delay_ms(100);
	if(tmp[0] == 0x00){
		lcd_clear();
		lcd_setCursor(0,0);
		lcd_send_string(" Usunieto palec ");
		lcd_special_char(6,0,2);
		lcd_setCursor(0,1);
		lcd_send_string("o ID: ");
		lcd_setCursor(6,1);
		lcd_send_string(itoa(id, buffer, 10));
		_delay_ms(2000);
		lcd_send_info("Drzwi zamkniete", "");
		lcd_special_char(12,0,2);
	}
	else{
		lcd_send_info(" Nie udalo sie  ", "  usunac palca! ");
		lcd_special_char(8,0,3);
		lcd_special_char(13,0,2);
		lcd_special_char(5,1,0);
		lcd_special_char(6,1,1);
		_delay_ms(2000);
		lcd_send_info("Drzwi zamkniete", "");
		lcd_special_char(12,0,2);
	}
	removeFinger = false;
}

void checkFinger(){
	lcd_send_info("  Umiesc palec  ", "Skanowanie:-----");
	lcd_special_char(6,0,5);
	lcd_special_char(7,0,1);
	fps_read_finger();
	uint8_t timer = 0;
	while(tmp[0] == 0x02){
		fps_read_finger();
		_delay_ms(100);
		timer += 1;
		if(timer == 10){
			lcd_send_info("  Umiesc palec  ", "Skanowanie: ----");
			lcd_special_char(6,0,5);
			lcd_special_char(7,0,1);
		}
		if(timer == 20){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:  ---");
			lcd_special_char(6,0,5);
			lcd_special_char(7,0,1);
		}
		if(timer == 30){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:   --");
			lcd_special_char(6,0,5);
			lcd_special_char(7,0,1);
		}
		if(timer == 40){
			lcd_send_info("  Umiesc palec  ", "Skanowanie:    -");
			lcd_special_char(6,0,5);
			lcd_special_char(7,0,1);
		}
		if(timer == 50){
			lcd_send_info("  Brak palca na ", " czytniku linii ");
			lcd_special_char(6,0,5);
			lcd_special_char(7,0,1);
			_delay_ms(1500);
			scanFinger = false;
			enrollFinger = false;
			removeFinger = false;
			lcd_send_info("Drzwi zamkniete", "");
			lcd_special_char(12,0,2);
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
					lcd_special_char(7,1,5);
					lcd_special_char(8,1,1);
					lcd_setCursor(11,1);
					percentage = ceil(((float)score/255) *100);
					lcd_send_string(itoa(percentage, buffor, 10));
					lcd_setCursor(15,1);
					lcd_send_string("%");
					PORTB |= (1 << PINB3);
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
			lcd_special_char(2,0,3);
			lcd_special_char(4,0,0);
			_delay_ms(2000);
		}
	}
	else{
		lcd_send_info("   Brak palca!  ", "Sprobuj ponownie");
		lcd_special_char(3,1,4);
		_delay_ms(2000);
	}
	removeFinger = false;
	enrollFinger = false;
	scanFinger = false;
	if(doorLocked){
	lcd_send_info("Drzwi zamkniete", "");
	lcd_special_char(12,0,2);
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

ISR(TIMER1_COMPA_vect){
		TCNT1 = 0;
		counter_check = true;
}