/*
 * Partb.c
 *
 * Created: 2020/10/17 15:14:47
 * Author : liwen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void Initialize()
{
	cli();
	//Set PD6 as output port
	DDRD |= (1<<DDD6);
	//Set Timer0 as normal
	TCCR0A &= ~(1<<WGM00);
	TCCR0A &= ~(1<<WGM01);
	TCCR0B &= ~(1<<WGM02);
	//Enable timer overflow interrupt
	TIMSK0 |= (1<<TOIE0);
	sei();
}

ISR(TIMER0_OVF_vect)
{
	//Toggle the PD6
	PORTD ^= (1<<PORTD6);
}

int main(void)
{
	Initialize();
    while (1);
}

