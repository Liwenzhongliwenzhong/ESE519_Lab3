#include "uart.h"
#include <avr/io.h>

void UART_initialize(int BAUD_PRESCALER)
{

	UBRR0H = (unsigned char)(BAUD_PRESCALER>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALER;

	UCSR0B |= (1<<RXEN0);
	UCSR0B |= (1<<TXEN0);

	UCSR0C |= (1<<USBS0);
	UCSR0C |= (1<<UCSZ01);
	UCSR0C |= (1<<UCSZ00);
	
}

void UART_putstring(char* StringPtr)
{
	while(*StringPtr != 0x00)
	{
		while(!(UCSR0A & (1<<UDRE0)));
		UDR0 = *StringPtr;
		StringPtr++;
	}
}