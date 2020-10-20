/*
 * Partb_4.c
 *
 * Created: 2020/10/17 18:48:02
 * Author : liwen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void Initialize()
{
	cli();
	//Set PD5 as output port
	DDRD |= (1<<DDD5);
	//Prescale the Timer0 by 256
	TCCR0B &= ~(1<<CS00);
	TCCR0B &= ~(1<<CS01);
	TCCR0B |= (1<<CS02);
	//Set Timer0 as phase correct PWM, TOP = OCRA
	TCCR0A |= (1<<WGM00);
	TCCR0A &= ~(1<<WGM01);
	TCCR0B |= (1<<WGM02);
	//Enable output compare interrupt
	OCR0A = 71;
	OCR0B = 35;
	//Non-inverting, clear on compare match
	TCCR0A |= (1<<COM0B1);
	sei();
}

int main(void)
{
	Initialize();
	while (1);
}


