/*
 * Partd_1.c
 *
 * Created: 2020/10/19 13:59:03
 * Author : liwen
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

//Set pulse width of the trigger signal of the ultrasonic sensor
int high_time = 25;
//Keep the overflow times
volatile long long ovf = 0;
volatile int ovf_1 = 0;
volatile int ovf_2 = 0;
//High = 1, low = 0
volatile int high_or_low = 0;
//Calculate the pulse width of the echo signal of the ultrasonic sensor
volatile int width = 0;
volatile int edge = 0;
//Calculate the actual distance
volatile int distance = 0;
//Determine frequency range
volatile int OCR0A_continuous = 0;

void Initialize()
{
	cli();
	//Set PD6, PD5 as output pin and PB0, PB1 as input pin
	DDRD |= (1<<DDD6);
	DDRB &= ~(1<<DDB0);
	DDRB &= ~(1<<DDB1);
	DDRD |= (1<<DDD5);
	//Prescale Timer0 by 256
	TCCR0B |= (1<<CS00);
	TCCR0B |= (1<<CS01);
	TCCR0B &= ~(1<<CS02);
	//Set Timer0 as phase correct PWM, TOP = OCRA
	TCCR0A |= (1<<WGM00);
	TCCR0A &= ~(1<<WGM01);
	TCCR0B |= (1<<WGM02);
	//Set OCR0A and OCR0B values
	OCR0A = 10;
	OCR0B = 5;
	//Non-inverting, clear on compare match
	TCCR0A |= (1<<COM0B1);
	
	//Prescale Timer1 by 8
	TCCR1B |= (1<<CS11);
	//Set Timer0 as normal
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	//Enable output compare interrupt
	TIMSK1 |= (1<<OCIE1A);
	//Enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);
	//Initially looking for rising edge
	TCCR1B |= (1<<ICES1);
	//Enable Timer1 overflow interrupt
	TIMSK1 |= (1<<TOIE1);
	OCR1A = 100;
	//CLear interrupt flag
	TIFR1 |= (1<<OCF1A);
	sei();
}

ISR(TIMER1_COMPA_vect)
{
	//Generate the trigger signal of the ultrasonic sensor
	if (high_or_low)
	{
		//Toggle PD6, PD6 set to low
		PORTD &= ~(1<<PORTD6);
		high_or_low = 0;
		OCR1A = 0;
	}
	else
	{
		//Make sure the time interval between two pulses is long enough
		if (ovf - ovf_1 > 4)
		{
			//Toggle PD6, PD6 set to high
			PORTD |= (1<<PORTD6);
			high_or_low = 1;
			OCR1A = high_time;
			ovf_1 = ovf;
		}
	}
}

ISR(TIMER1_OVF_vect)
{
	ovf++;
}

ISR(TIMER1_CAPT_vect)
{
	//Calculate pulse width of echo signal
	width = 65536*(ovf - ovf_2) + ICR1 - edge;
	ovf_2 = ovf;
	edge = ICR1;
	//The pulse ends at falling edge
	if (!(TCCR1B & (1<<ICES1)))
	{
		//Calculate distance
		distance = width/118;
		//To ensure correct measurement, we only take distance between 0 and 100cm
		if ((distance > 0)&&(distance < 100))
		{
			//Determine frequency range
			OCR0A_continuous = distance + 58;
			if ((OCR0A_continuous < 140) && (OCR0A_continuous > 119))
			{
				OCR0A = 120;
				OCR0B = OCR0A/10;
			}
			else if ((OCR0A_continuous < 120) && (OCR0A_continuous > 106))
			{
				OCR0A = 107;
				OCR0B = OCR0A/10;
			}
			else if ((OCR0A_continuous < 107) && (OCR0A_continuous > 94))
			{
				OCR0A = 95;
				OCR0B = OCR0A/10;
			}
			else if ((OCR0A_continuous < 95) && (OCR0A_continuous > 89))
			{
				OCR0A = 90;
				OCR0B = OCR0A/10;
			}
			else if ((OCR0A_continuous < 90) && (OCR0A_continuous > 79))
			{
				OCR0A = 80;
				OCR0B = OCR0A/10;
			}
			else if ((OCR0A_continuous < 80) && (OCR0A_continuous > 70))
			{
				OCR0A = 71;
				OCR0B = OCR0A/10;
			}
			else if ((OCR0A_continuous < 71) && (OCR0A_continuous > 63))
			{
				OCR0A = 64;
				OCR0B = OCR0A/10;
			}
			else if ((OCR0A_continuous < 64) && (OCR0A_continuous > 59))
			{
				OCR0A = 60;
				OCR0B = OCR0A/10;
			}
		}
	}
	//Toggle edge trigger type
	TCCR1B ^= (1<<ICES1);
}

int main(void)
{
	Initialize();
	while (1);
}

