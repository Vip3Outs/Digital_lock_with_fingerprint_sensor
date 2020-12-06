#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>


/* Settings */
#define F_CPU			16000000
#define _BAUD			57600					    // Baud rate
#define _UBRR			(F_CPU/16)/_BAUD - 1		// Used for UBRRL and UBRRH
#define RX_BUFF			10

/* Useful macros */
#define TX_START()		UCSR0B |= _BV(TXEN0)	// Enable TX
#define TX_STOP()		UCSR0B &= ~_BV(TXEN0)	// Disable TX
#define RX_START()		UCSR0B |= _BV(RXEN0)	// Enable RX
#define RX_STOP()		UCSR0B &= ~_BV(RXEN0)	// Disable RX
#define COMM_START()	TX_START(); RX_START()	// Enable communications

/* Interrupt macros; Remember to set the GIE bit in SREG before using (see datasheet) */
#define RX_INTEN()		UCSR0B |= _BV(RXCIE0)	// Enable interrupt on RX complete
#define RX_INTDIS()		UCSR0B &= ~_BV(RXCIE0)	// Disable RX interrupt
#define TX_INTEN()		UCSR0B |= _BV(TXCIE0)	// Enable interrupt on TX complete
#define TX_INTDIS()		UCSR0B &= ~_BV(TXCIE0)	// Disable TX interrupt

/* Prototypes */
void initUART(void);
uint8_t getByte(void);
void putByte(unsigned char data);
void writeString(char *str);
const char* readString(void);
bool checkEmptyRegister(void);

#endif /* UART_H_ */