/*
 * LAB2-0.2.c
 *
 * Created: 2018/9/8 12:21:51
 * Author : Wang
 */ 
#define F_CPU  20000000L

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{   //enable ports
	DDRB = 0x20;
    PORTB = 0x01;
    /* Replace with your application code */
    while (1) 
    {
		//toggle the LED
		if(!(PINB & 0x01))
		{
			PORTB |= 0x20;
		}
		else
		{
			PORTB &= 0xDF;
		}
    }
}

