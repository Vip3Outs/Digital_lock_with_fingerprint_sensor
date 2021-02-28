#include "uart.h"
#include <avr/io.h>


void uart_init(){
	DDRD |= (1 << PIND0);
	DDRD &= ~ (1 << PIND1);
	UBRR0H = (uint8_t)(UBRR >> 8);			//ustawienie baud rate
	UBRR0L =  (uint8_t)(UBRR);	
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);	//wlaczenie linii RX i TX
	UCSR0C = (0x03 << UCSZ00);				//format ramki 8bitowy z 1 bitem stopu
}

uint8_t uart_getByte(){
	//sprawdz czy zostalo cos otrzymane
	while (!(UCSR0A & (1 << RXC0)));
	return (uint8_t) UDR0;
}

void uart_putByte(uint8_t data){
	//zostan az rejestr nie bedzie pusty
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}
