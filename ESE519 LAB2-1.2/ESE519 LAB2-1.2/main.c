/*
 * ESE519 LAB2-1.2.c
 *
 * Created: 2018/9/12 17:26:33
 * Author : Wang
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{
	//enable ports
	DDRD |= (1 << PORTD6);
    //initialize  interrupt
	OCR0A = 142; //440Hz
	TIMSK0 |= (1 << OCIE0A);
	TCCR0B |=  (1 << CS02); // set at 256 prescalar
	sei();
	
	
	/* Replace with your application code */
	while (1)
	{

	}
}

ISR(TIMER0_COMPA_vect)
{
	//toggle port and update OCR0A
	PORTD ^= (1 << PORTD6);
	OCR0A += 142;
}
