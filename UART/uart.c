#include "uart.h"
#include <avr/io.h>


void initUART(void){
	DDRD |= (1 << PD1);
	DDRD &= ~ (1 << PD0);

	UBRR0H = ((_UBRR) & 0xF00);
	UBRR0L = (uint8_t) ((_UBRR) & 0xFF);

	TX_START();
	RX_START();

	UCSR0C = (0x03 << UCSZ00);

}

uint8_t getByte(void){
	//sprawdz czy zostalo cos otrzymane
	while (!(UCSR0A & (1 << RXC0)));
	return (uint8_t) UDR0;
}

void putByte(unsigned char data){
	//zostan az rejestr nie bedzie pusty
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}
