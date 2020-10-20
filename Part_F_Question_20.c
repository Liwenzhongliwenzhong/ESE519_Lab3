/*
 * Partf.c
 *
 * Created: 2020/10/19 17:06:57
 * Author : liwen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL

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
//Button pressed = 1, released = 0
int button_pressed = 0;

void Initialize()
{
	cli();
	//Set PD6, PD5 as output pin and PB0, PB1 as input pin
	DDRD |= (1<<DDD6);
	DDRB &= ~(1<<DDB0);
	DDRD &= ~(1<<DDD7);
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
	//Enable pin change interrupt, enable PCINT23
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT23);
	OCR1A = 100;
	//CLear interrupt flag
	TIFR1 |= (1<<OCF1A);
	
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
			OCR0A_continuous = distance + 58;
			if (!button_pressed)
			{
				//If the button is not pressed, use continuous mode
				OCR0A = OCR0A_continuous;
			}
			else
			{
				//If the button is pressed, use discrete mode
				//Determine frequency range
				if ((OCR0A_continuous < 140) && (OCR0A_continuous > 119))
				{
					OCR0A = 120;
				}
				else if ((OCR0A_continuous < 120) && (OCR0A_continuous > 106))
				{
					OCR0A = 107;
				}
				else if ((OCR0A_continuous < 107) && (OCR0A_continuous > 94))
				{
					OCR0A = 95;
				}
				else if ((OCR0A_continuous < 95) && (OCR0A_continuous > 89))
				{
					OCR0A = 90;
				}
				else if ((OCR0A_continuous < 90) && (OCR0A_continuous > 79))
				{
					OCR0A = 80;
				}
				else if ((OCR0A_continuous < 80) && (OCR0A_continuous > 70))
				{
					OCR0A = 71;
				}
				else if ((OCR0A_continuous < 71) && (OCR0A_continuous > 63))
				{
					OCR0A = 64;
				}
				else if ((OCR0A_continuous < 64) && (OCR0A_continuous > 59))
				{
					OCR0A = 60;
				}
			}
		}
	}
	//Toggle edge trigger type
	TCCR1B ^= (1<<ICES1);
}

ISR(PCINT2_vect)
{
	//Determine whether the button is pressed
	if (PIND & (1<<PIND7))
	{
		button_pressed = 1;
	}
	else
	{
		button_pressed = 0;
	}
}

ISR(ADC_vect)
{
	//Set PWM pulse width .e.g volume according to ADC value
	if (ADC < 103)
	{
		OCR0B = OCR0A/20;
	}
	else if (ADC < 205)
	{
		OCR0B = OCR0A*2/20;
	}
	else if (ADC < 307)
	{
		OCR0B = OCR0A*3/20;
	}
	else if (ADC < 409)
	{
		OCR0B = OCR0A*4/20;
	}
	else if (ADC < 511)
	{
		OCR0B = OCR0A*5/20;
	}
	else if (ADC < 613)
	{
		OCR0B = OCR0A*6/20;
	}
	else if (ADC < 715)
	{
		OCR0B = OCR0A*7/20;
	}
	else if (ADC < 817)
	{
		OCR0B = OCR0A*8/20;
	}
	else if (ADC < 919)
	{
		OCR0B = OCR0A*9/20;
	}
	else if (ADC < 1020)
	{
		OCR0B = OCR0A*10/20;
	}
}

int main(void)
{
	Initialize();
	while (1);
}










