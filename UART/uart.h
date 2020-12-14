#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>
//wstepna konfiguracja
#define F_CPU			16000000				//taktowanie procesora
#define _BAUD			57600					//baud rate
#define _UBRR			(F_CPU/16)/_BAUD - 1	//ustawienia dla UBRRH i UBRRL
#define RX_BUFF			10

//uzyteczne makra
#define TX_START()		UCSR0B |= _BV(TXEN0)	//enable TX
#define RX_START()		UCSR0B |= _BV(RXEN0)	//enable RX


void initUART();
uint8_t getByte();
void putByte(unsigned char data);


#endif /* __UART_H___ */