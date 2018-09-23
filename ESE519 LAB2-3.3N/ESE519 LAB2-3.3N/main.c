/*
 * ESE519 LAB2-3.3N.c
 *
 * Created: 2018/9/20 17:16:10
 * Author : Wang
 */ 

#define F_CPU 16000000
#define BAUDRATE 9600 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdbool.h>

char buffer [8];
int l,k=0;
float tick;
uint16_t adc_result;
unsigned int flag1 = 0,flag2 = 0,width = 0;
int q = 0;
bool button = 0;


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
	return(ADC);
	}

int main(void)
{
	//enable serial communication
 	UCSR0A = (1<<U2X0); // double speed mode
 	UBRR0H = (unsigned char) ((F_CPU/8/BAUDRATE-1) >> 8);
 	UBRR0L = (unsigned char) (F_CPU/8/BAUDRATE-1);
 	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // enable rx and tx

	//enable ports 
	DDRB |= (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3) | (1 << PORTB4);
	DDRD |= (1 << PORTD6);
	PORTB |= (1 << PORTB1);
	
	//initialize PCINT
	PCICR |= (1 << PCIE2);
	PCMSK2 |= (1 << PCINT23);
	
	//initialize  interrupt
	TCCR0A = (1 << WGM01); //Set CTC Bit
	TIMSK0 |= (1 << OCIE0A);
	TCCR0B |=  (1 << CS00) | (1 << CS01);
	
	OCR1A = TCNT1 + 10;
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS11); // 8?prescaler
	 
	TCCR1B |= (1 << ICES1);
	TIMSK1 |= (1 <<(TOIE1));
	sei();
	 
	adc_init();// initialize adc
	_delay_ms(50);
	
	while(1)
	{
		
?????//print the width of pulses
		adc_result = adc_read();
		l=sprintf (buffer, "%d\n", width);
		//l=sprintf (buffer, "%d\n", adc_result);
		while(k<l)
		{
			UDR0=buffer[k];
			++k;
			while (!(UCSR0A & 0x40));
			UCSR0A |= 0x40;
			
		}
		k=0;
		//change volume by range of adc result (light sensor)	
		if ((adc_result > 600))
		{
			PORTB &= ~(1 << PORTB2);
			PORTB &= ~(1 << PORTB3);
			PORTB &= ~(1 << PORTB4);
		}
		
		if ((adc_result > 550) & (adc_result <= 600))
		{
			PORTB |= (1 << PORTB2);
			PORTB &= ~(1 << PORTB3);
			PORTB &= ~(1 << PORTB4);
		}
		if ((adc_result > 500) & (adc_result <= 550))
		{
			PORTB &= ~(1 << PORTB2);
			PORTB |= (1 << PORTB3);
			PORTB &= ~(1 << PORTB4);
		}
		
		if ((adc_result > 450) & (adc_result <= 500))
		{
			PORTB &= ~(1 << PORTB2);
			PORTB &= ~(1 << PORTB3);
			PORTB |= (1 << PORTB4);
		}
		if ((adc_result > 400) & (adc_result <= 450))
		{
			PORTB |= (1 << PORTB2);
			PORTB |= (1 << PORTB3);
			PORTB &= ~(1 << PORTB4);
		}
		
		if ((adc_result > 350) & (adc_result <= 400))
		{
			PORTB |= (1 << PORTB2);
			PORTB &= ~(1 << PORTB3);
			PORTB |= (1 << PORTB4);
		}

		if ((adc_result > 300) & (adc_result <= 350))
		{
			PORTB &= ~(1 << PORTB2);
			PORTB |= (1 << PORTB3);
			PORTB |= (1 << PORTB4);
		}
		
		if ((adc_result > 250) & (adc_result <= 300))
		{
			PORTB |= (1 << PORTB2);
			PORTB |= (1 << PORTB3);
			PORTB |= (1 << PORTB4);
		}
		
		if(button)//continuous mode
		{
			//change pitch by pulse width of range finder
			tick = floor(((-12.0 * width)/800 + 239)/2);
			OCR0A = tick;
		}
		
		else//discrete mode
		{
			//change pitch by pulse width of range finder
			if(width<=1000){
				OCR0A = 120;
			}
			if((width<=2000) & (width>1000)){
				OCR0A = 107;
			}
			if((width<=3000) & (width>2000)){
				OCR0A = 95;
			}
			if((width<=4000) & (width>3000)){
				OCR0A = 90;
			}
			if((width<=5000) & (width>4000)){
				OCR0A = 80;
			}
			if((width<=6000) & (width>5000)){
				OCR0A = 71;
			}
			if((width<=7000) & (width>6000)){
				OCR0A = 64;
			}
			if(width>7000){
				OCR0A = 60;
			}	
		}
		
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
	if(TCCR1B &(1<< ICES1))//falling edge
	{
		flag1 = ICR1;
		TCCR1B &= ~(1 << ICES1);
		//TIFR1 |= (1<<ICF1);
	}
	else
	{
		flag2 = ICR1;
		TCCR1B |= (1 << ICES1);//raising edge
		//disable input capture, enable output compare
		TIMSK1 &= ~(1 << ICIE1);
		TIMSK1 |= (1 << OCIE1A);
		width = flag2 - flag1;//record the width of echo
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

ISR(TIMER0_COMPA_vect)
{
	//toggle port to generate pulses
	PORTD ^= (1 << PORTD6);
}


ISR(PCINT2_vect)
{
	//change flag of mode every 2 interrupt
	if(q==1)
	{
		button = !button;
		q = 0;
	}
	q++;
}



