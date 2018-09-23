/*
 * Lab2_1.1.c
 *
 * Created: 2018/9/9 14:18:05
 * Author : Wang
 */ 

#define F_CPU 20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//

int main(void)
{
	//enable ports
	DDRB = 0x20;
	PORTB = 0x02;
	
    //initialize interrupt
	TIMSK1 |= (1 << ICIE1);
	//TCCR1B = (1 << CS10);
	
	//initial state judgment
		if(PINB & 0x02)
		{
		//	PORTB |= 0x20;
			TCCR1B &= (0 << ICES1);
		}
		else
		{
		//	PORTB &= 0xDF;
			TCCR1B |= (1 << ICES1);
		}
		
	sei();
	
    /* Replace with your application code */
    while (1) 
    {

    }
}

ISR(TIMER1_CAPT_vect)
{
	
	//toggle LED and change detecting edge of input capture
		if(!(TCCR1B))
	{
		PORTB |= 0x20;
		TCCR1B |= (1 << ICES1);
	}
	
	else 
	{
		PORTB &= 0xDF;
		TCCR1B &= (0 << ICES1);
	}
	 
	
	
	
}

