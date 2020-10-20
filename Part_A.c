/*
 * GccApplication1.c
 *
 * Created: 2020/10/17 13:32:17
 * Author : liwen
 */ 

#include <avr/io.h>
#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)


int main(void)
{
	UART_initialize(BAUD_PRESCALER);
	char Frendly_Message[20];
    while (1) 
    {
		//Print a friendly message
		sprintf(Frendly_Message, "Hello World!\n");
		UART_putstring(Frendly_Message);
		_delay_ms(10000);
		sprintf(Frendly_Message, "Aha!\n");
		UART_putstring(Frendly_Message);
		_delay_ms(10000);
    }
}

