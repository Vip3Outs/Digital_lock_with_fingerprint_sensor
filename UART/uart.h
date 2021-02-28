#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>
//wstepna konfiguracja
#define F_CPU			16000000				//taktowanie procesora
#define BAUD			57600					//baud rate
#define UBRR			(F_CPU/16)/BAUD - 1		//ustawienia dla UBRRH i UBRRL

void uart_init();
uint8_t uart_getByte();
void uart_putByte(uint8_t data);

#endif /* __UART_H___ */