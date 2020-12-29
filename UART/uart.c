#include "uart.h"
#include <avr/io.h>


void initUART(){
	//?
	DDRD |= (1 << PIND0);
	DDRD &= ~ (1 << PIND1);

	UBRR0H = ((_UBRR) & 0xF00);
	UBRR0L = (uint8_t) ((_UBRR) & 0xFF);

	TX_START();
	RX_START();

	UCSR0C = (0x03 << UCSZ00);

}

uint8_t getByte(){
	//sprawdz czy zostalo cos otrzymane
	while (!(UCSR0A & (1 << RXC0)));
	return (uint8_t) UDR0;
}

void putByte(uint8_t data){
	//zostan az rejestr nie bedzie pusty
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}
