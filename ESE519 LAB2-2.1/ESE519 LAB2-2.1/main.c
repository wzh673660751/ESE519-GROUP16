/*
 * ESE519 LAB2-2.1.c
 *
 * Created: 2018/9/13 18:05:27
 * Author : Wang
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int tempA = 0;

int main(void)
{
	DDRD |= (1 << PORTD6);
    TCNT0 = 0;
	TCCR0A |= (1 << WGM01); //Set CTC Bit
	TCCR0A |= (1 << COM0A0);
	OCR0A = 142;
	//TIMSK0 = (1 << OCIE0A);
	
	sei();
	
	TCCR0B |=  (1 << CS02); // set at 256 prescalar
	/* Replace with your application code */
	while (1)
	{
		

	}
}

//ISR(TIMER0_COMPA_vect)
//{
//	PORTD ^= (1 << PORTD6);
//}
