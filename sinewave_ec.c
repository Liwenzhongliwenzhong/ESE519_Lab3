/*
 * Partg.c
 *
 * Created: 2020/10/19 19:40:54
 * Author : liwen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL

//Lookup table size determines the frequency of the sine wave
const int size = 72;
int sine_table[72] = {128, 139, 150, 161, 172, 182, 192, 201, 210, 219, 226, 233, 239, 244, 248, 252, 254, 255, 
					   255, 254, 252, 248, 244, 239, 233, 226, 219, 210, 201, 192, 182, 172, 161, 150, 139, 128, 
					   128, 117, 106, 95, 84, 74, 64, 55, 46, 37, 30, 23, 17, 12, 8, 4, 2, 0, 
					   0, 2, 4, 8, 12, 17, 23, 30, 37, 46, 55, 64, 74, 84, 95, 106, 117, 128};
volatile int lookup_count = 0;

void Initialize()
{
	cli();
	//Set PB1 (OC1A) as output pin
	DDRB |= (1<<DDB1);
	//Prescale Timer1 by 8
	TCCR1B |= (1<<CS10);
	//Set to phase correct mode. Count to TOP
	TCCR1A |= (1<<WGM10);
	//Initialize OCR1A
	OCR1A = 128;
	//Enable overflow interrupt
	TIMSK1 |= (1<<TOIE1);
	TIFR1 |= (1<<TOV1);
	//Non-inverting mode. Clear on compare match
	TCCR1A |= (1<<COM1A1);
	sei();
}

ISR(TIMER1_OVF_vect)
{
	if (lookup_count < size)
	{
		//Set PWM duty cycle according to lookup table
		OCR1A = sine_table[lookup_count];
		lookup_count ++;
	}
	else
	{
		lookup_count = 0;
	}
}

int main(void)
{
    Initialize();
    while (1);
}

