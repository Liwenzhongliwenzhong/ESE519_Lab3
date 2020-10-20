/*
 * Partb_3.c
 *
 * Created: 2020/10/17 17:28:13
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
	//Set Timer0 to CTC
	TCCR0A &= ~(1<<WGM00);
	TCCR0A |= (1<<WGM01);
	TCCR0B &= ~(1<<WGM02);
	//Toggle OC0A on compare match
	TCCR0A |= (1<<COM0A0);
	//Set OCR0A register
	OCR0A = 70;
	sei();
}

int main(void)
{
	Initialize();
	while (1);
}

