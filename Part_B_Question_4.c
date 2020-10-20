/*
 * Partb_2.c
 *
 * Created: 2020/10/17 16:23:40
 * Author : liwen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void Initialize()
{
	cli();
	//Set PD6 as output port
	DDRD |= (1<<DDD6);
	//Prescale Timer0 by 256
	TCCR0B &= ~(1<<CS00);
	TCCR0B &= ~(1<<CS01);
	TCCR0B |= (1<<CS02);
	//Set Timer0 as normal
	TCCR0A &= ~(1<<WGM00);
	TCCR0A &= ~(1<<WGM01);
	TCCR0B &= ~(1<<WGM02);
	//Enable output compare interrupt
	TIMSK0 |= (1<<OCIE0A);
	//Set OCR0A register
	OCR0A = 70;
	//Clear interrupt flag
	TIFR0 |= (1<<OCF0A);
	sei();
}

ISR(TIMER0_COMPA_vect)
{
	//Toggle the PD6
	PORTD ^= (1<<PORTD6);
	//Update OCR0A
	OCR0A += 70;
}

int main(void)
{
	Initialize();
	while (1);
}
