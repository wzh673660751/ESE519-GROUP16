/*
 * ESE519 LAB2-3.2.c
 *
 * Created: 2018/9/18 20:03:13
 * Author : Wang
 */ 

#include <avr/io.h>


#define F_CPU 16000000
#define BAUDRATE 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char buffer [8];
int l,k = 0;
unsigned int flag1 = 0,flag2 = 0,width = 0,sum = 0;

int main(void)
{
	//enable serial communication
	UCSR0A = (1<<U2X0); // double speed mode
	UBRR0H = (unsigned char) ((F_CPU/8/BAUDRATE-1) >> 8);
	UBRR0L = (unsigned char) (F_CPU/8/BAUDRATE-1);
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // enable rx and tx
	
	//enable ports
	DDRB |= (1 << PORTB1);
	DDRB |= (1 << PORTB2);
	DDRB |= (1 << PORTB3);
	DDRB |= (1 << PORTB4);	
	PORTB |= (1 << PORTB1);
	
	//initialize  interrupt
	//TCCR1B |= (1 << WGM12);
	OCR1A = TCNT1 + 10;
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS11); // 8
	
	TCCR1B |= (1 << ICES1);
	TIMSK1 |= (1 <<(TOIE1));
	
	sei();
	
	/* Replace with your application code */
	
	while (1)
	{
		//print the width of pulses
		l=sprintf (buffer, "%d\n", width);
		while(k<l)
		{
			UDR0=buffer[k];
			++k;
			while (!(UCSR0A & 0x40));
			UCSR0A |= 0x40;
			
			//change ports output by range of pulses' width
			if ((width >= 0) & (width <= 1000))
			{
				PORTB &= ~(1 << PORTB2);
				PORTB &= ~(1 << PORTB3);
				PORTB &= ~(1 << PORTB4);
			}
			
			if ((width > 1000) & (width <= 2000))
			{
				PORTB |= (1 << PORTB2);
				PORTB &= ~(1 << PORTB3);
				PORTB &= ~(1 << PORTB4);
			}
			if ((width > 2000) & (width <= 3000))
			{
				PORTB &= ~(1 << PORTB2);
				PORTB |= (1 << PORTB3);
				PORTB &= ~(1 << PORTB4);
			}
			
			if ((width > 3000) & (width <= 4000))
			{
				PORTB &= ~(1 << PORTB2);
				PORTB &= ~(1 << PORTB3);
				PORTB |= (1 << PORTB4);
			}
			if ((width > 4000) & (width <= 5000))
			{
				PORTB |= (1 << PORTB2);
				PORTB |= (1 << PORTB3);
				PORTB &= ~(1 << PORTB4);
			}
			
			if ((width > 5000) & (width <= 6000))
			{
				PORTB |= (1 << PORTB2);
				PORTB &= ~(1 << PORTB3);
				PORTB |= (1 << PORTB4);
			}

			if ((width > 6000) & (width <= 7000))
			{
				PORTB &= ~(1 << PORTB2);
				PORTB |= (1 << PORTB3);
				PORTB |= (1 << PORTB4);
			}
			
			if ((width > 7000))
			{
				PORTB |= (1 << PORTB2);
				PORTB |= (1 << PORTB3);
				PORTB |= (1 << PORTB4);
			}
			
		}
			
		k=0;
		
		
	}
	
}


ISR(TIMER1_COMPA_vect)
{
	//generate pulse
	PORTB &= ~(1 << PORTB1);
	//disable output compare, enable input capture
	TIMSK1 &= ~(1 << OCIE1A);
	TIMSK1 |= (1 << ICIE1);

}

ISR(TIMER1_CAPT_vect)
{
	if(TCCR1B &(1<< ICES1))
	{
		flag1 = ICR1;
		TCCR1B &= ~(1 << ICES1);//falling edge
		//TIFR1 |= (1<<ICF1);
	}
	else
	{
		flag2 = ICR1;
		TCCR1B |= (1 << ICES1);//raising edge
		//disable input capture, enable output compare
		TIMSK1 &= ~(1 << ICIE1);
		TIMSK1 |= (1 << OCIE1A);
		//record the width of echo
		width = flag2 - flag1;
		//_delay_us(300);
		TCNT1 = 0x0000;
		PORTB |= (1 << PORTB1);

	}
}

ISR(TIMER1_OVF_vect)
{
	//reset cycle for unexpected overflow
	TIMSK1 &= ~(1 << ICIE1); 
	TIMSK1 |= (1 << OCIE1A); 
	PORTB |= (1<<PORTB1); 
	TCNT1 = 0x0000;
}

