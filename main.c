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
			lcd_send_string("KURWA DZIALA XD");
			lcd_clear();
			lcd_setCursor(0,0);
			
			id = 1;
				
	while(1)
	{	
		getFingerprintEnroll();
		_delay_ms(5000);
	}
	return 0;
}

uint8_t getFingerprintEnroll (){
	uint8_t p = -1;
	printf("Waiting for valid finger to enroll as #\n"); printf(id);
	 while (p != FINGERPRINT_OK) {
		 p = getImage();
		 switch (p) {
			 case FINGERPRINT_OK:
			 printf("Image taken\n");
			 break;
			 case FINGERPRINT_NOFINGER:
			 printf(".");
			 break;
			 case FINGERPRINT_PACKETRECIEVEERR:
			 printf("Communication error\n");
			 break;
			 case FINGERPRINT_IMAGEFAIL:
			 printf("Imaging error\n");
			 break;
			 default:
			 printf("Unknown error\n");
			 break;
		 }
	 }
	 
	 p = image2Tz(1);
	  switch (p) {
		  case FINGERPRINT_OK:
		  printf("Image converted\n");
		  break;
		  case FINGERPRINT_IMAGEMESS:
		  printf("Image too messy\n");
		  return p;
		  case FINGERPRINT_PACKETRECIEVEERR:
		  printf("Communication error\n");
		  return p;
		  case FINGERPRINT_FEATUREFAIL:
		  printf("Could not find fingerprint features\n");
		  return p;
		  case FINGERPRINT_INVALIDIMAGE:
		  printf("Could not find fingerprint features\n");
		  return p;
		  default:
		  printf("Unknown error\n");
		  return p;
	  }
	  
	  printf("Remove finger\n");
	  _delay_ms(2000);
	  p = 0;
	  
	  while (p != FINGERPRINT_NOFINGER) {
		  p = getImage();
	  }
	  p = -1;
	  printf("Place same finger again\n");
	  while (p != FINGERPRINT_OK) {
		  p = getImage();
		  switch (p) {
			  case FINGERPRINT_OK:
			  printf("Image taken\n");
			  break;
			  case FINGERPRINT_NOFINGER:
			  printf(".");
			  break;
			  case FINGERPRINT_PACKETRECIEVEERR:
			  printf("Communication error\n");
			  break;
			  case FINGERPRINT_IMAGEFAIL:
			  printf("Imaging error\n");
			  break;
			  default:
			  printf("Unknown error\n");
			  break;
		  }
	  }

	  // OK success!

	  p = image2Tz(2);
	  switch (p) {
		  case FINGERPRINT_OK:
		  printf("Image converted\n");
		  break;
		  case FINGERPRINT_IMAGEMESS:
		  printf("Image too messy\n");
		  return p;
		  case FINGERPRINT_PACKETRECIEVEERR:
		  printf("Communication error\n");
		  return p;
		  case FINGERPRINT_FEATUREFAIL:
		  printf("Could not find fingerprint features\n");
		  return p;
		  case FINGERPRINT_INVALIDIMAGE:
		  printf("Could not find fingerprint features\n");
		  return p;
		  default:
		  printf("Unknown error\n");
		  return p;
	  }

	  // OK converted!
	  printf("Creating model for #\n");

	  p = createModel();
	  if (p == FINGERPRINT_OK) {
		  printf("Prints matched!\n");
		  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
		  printf("Communication error\n");
		  return p;
		  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
		  printf("Fingerprints did not match\n");
		  return p;
		  } else {
		  printf("Unknown error\n");
		  return p;
	  }

	  p = storeModel(id);
	  if (p == FINGERPRINT_OK) {
		  printf("Stored!\n");
		  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
		  printf("Communication error\n");
		  return p;
		  } else if (p == FINGERPRINT_BADLOCATION) {
		  printf("Could not store in that location\n");
		  return p;
		  } else if (p == FINGERPRINT_FLASHERR) {
		  printf("Error writing to flash\n");
		  return p;
		  } else {
		  printf("Unknown error\n");
		  return p;
	  }

	  return true;
	
}

uint8_t readNumber(void){
	uint8_t num = 0;
	printf("Pass in ID:\n ");
	scanf("%s", num);
	return num;
}