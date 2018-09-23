/*
 * ESE519 LAB2-2.3.c
 *
 * Created: 2018/9/18 14:27:48
 * Author : Wang
 */ 

#define F_CPU 16000000
#define BAUDRATE 9600 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
	float tick;
void putstring(char *s)
{
	while (*s)
	{
		while ( !(UCSR0A & (1<<UDRE0)) );
		UDR0 = *s++;
	}
}

void putnum(int n)
{
	char buf[16];
	itoa(n, buf, 10);
	putstring(buf);
}

void adc_init()
{
	//initialize adc
	ADMUX |= (1<<REFS0);// AREF = AVcc
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);// ADC Enable and prescaler of 128
	ADCSRA |= (1<<ADATE);//auto
	ADCSRA |= (1<<ADSC);// wait for conversion to complete. 

}
uint16_t adc_read()
{
    //ADCSRA |= (1<<ADSC);// wait for conversion to complete. 
	//while(ADCSRA & (1<<ADSC));
	return(ADC);
	}

int main(void)
{
	//enable ports
	DDRB |= (1 << PORTB1);
	
	//initialize  interrupt
	TCCR0A = (1 << WGM01); //Set CTC Bit
	TIMSK0 |= (1 << OCIE0A);
	sei();
	TCCR0B |=  (1 << CS00) | (1 << CS01);
	
	//enable serial communication
 	UCSR0A = (1<<U2X0); // double speed mode
 	UBRR0H = (unsigned char) ((F_CPU/8/BAUDRATE-1) >> 8);
 	UBRR0L = (unsigned char) (F_CPU/8/BAUDRATE-1);
 	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // enable rx and tx
	uint16_t adc_result;
	//float tick;
	adc_init();// initialize adc
	_delay_ms(50);
	while(1)
	{
		//calculate ticks by adc result
		adc_result = adc_read();
		tick = floor(((-120.0 * adc_result)/1024 + 239)/2);
		putnum(tick);
		putstring(" \r\n");
		OCR0A = tick;
		_delay_ms(50);
		//sprintf("%d\n",adc_result);
	}
    
}

ISR(TIMER0_COMPA_vect)
{
	//toggle port to generate pulses
	PORTB ^= (1 << PORTB1);
}

