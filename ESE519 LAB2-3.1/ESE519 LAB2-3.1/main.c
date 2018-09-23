/*
 * ESE519 LAB2-3.1.c
 *
 * Created: 2018/9/16 16:39:40
 * Author : Wang
 */ 
#define F_CPU 16000000
#define BAUDRATE 9600 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

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
	ADCSRA |= (1<<ADSC); 

}
uint16_t adc_read()
{
    //ADCSRA |= (1<<ADSC);// wait for conversion to complete. 
	//while(ADCSRA & (1<<ADSC));
	return(ADC);
	}

int main(void)
{
	//enable serial communication
 	UCSR0A = (1<<U2X0); // double speed mode
 	UBRR0H = (unsigned char) ((F_CPU/8/BAUDRATE-1) >> 8);
 	UBRR0L = (unsigned char) (F_CPU/8/BAUDRATE-1);
 	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // enable rx and tx
	uint16_t adc_result;
	adc_init();// initialize adc
	_delay_ms(50);
	while(1)
	{
		//print adc output
		adc_result = adc_read();
		putnum(adc_result);
		putstring(" \r\n");
		_delay_ms(50);
		//sprintf("%d\n",adc_result);
	}
    
}

