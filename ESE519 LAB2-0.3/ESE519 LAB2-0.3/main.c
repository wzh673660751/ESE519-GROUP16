/*
 * ESE519 LAB2-0.3.c
 *
 * Created: 2018/9/12 17:37:49
 * Author : Wang
 */ 

#define F_CPU 20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
double dutyCycle = 0;

int main(void)
{
	//enable ports
	DDRD = (1 << PORTD6);

    //initialize interrupt(PWM)
	TCCR0A = (1 << COM0A1) | (1 << WGM00) | (1 << WGM01); //Set FAST PWM mode
	TIMSK0 = (1 << TOIE0);
	OCR0A = (dutyCycle/100.0)*255.0;
	sei();
	TCCR0B = (1 << CS00) | (1 << CS02); // set at 1 prescalar
	/* Replace with your application code */
	while (1)
	{
		//change duty cycle
		_delay_ms(20);
		
		dutyCycle += 1;
		
		if(dutyCycle >100)
		{
			dutyCycle = 0;
		}
	}
}

ISR(TIMER0_OVF_vect)
{
	//update OCR0A
	OCR0A = (dutyCycle/100) * 255;
}