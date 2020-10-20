/*
 * Parte.c
 *
 * Created: 2020/10/18 23:55:16
 * Author : liwen
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
//Message to print
char String[20];

void Initialize()
{
	cli();
	//Clear power reduction for ADC
	PRR &= ~(1<<PRADC);
	//Select Vref = Avcc
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);
	//Set the ADC clock divided by 128, 125KHz
	ADCSRA |= (1<<ADPS0);
	ADCSRA |= (1<<ADPS1);
	ADCSRA |= (1<<ADPS2);
	//Select channel 0
	ADMUX &= ~(1<<MUX0);
	ADMUX &= ~(1<<MUX1);
	ADMUX &= ~(1<<MUX2);
	ADMUX &= ~(1<<MUX3);
	//Set to auto trigger
	ADCSRA |= (1<<ADATE);
	//Set to free running
	ADCSRB &= ~(1<<ADTS0);
	ADCSRB &= ~(1<<ADTS1);
	ADCSRB &= ~(1<<ADTS2);
	//Disable digital input buffer on ADC pin
	DIDR0 |= (1<<ADC0D);
	//Enable ADC
	ADCSRA |= (1<<ADEN);
	//Enable ADC interrupt
	ADCSRA |= (1<<ADIE);
	//Start conversion
	ADCSRA |= (1<<ADSC);
	sei();
}

ISR(ADC_vect)
{
	//Print ADC value
	sprintf(String, "%d\n", ADC);
	UART_putstring(String);
}

int main(void)
{
    Initialize();
	UART_init(BAUD_PRESCALER);
    while (1);
}

