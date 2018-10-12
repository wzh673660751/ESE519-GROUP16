#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lcd.h"


#define F_CPU 16000000

#include <util/delay.h>

#define HIGH 1
#define LOW 0
#define BUFFER 1024
#define BLACK 0x000001

char displayChar = 0;
int8_t xa = 0,ya = 0;
uint8_t x = 0,y = 0;
uint16_t adc_result;
uint8_t pad_c0 = 32,pad_c1 = 32;

uint8_t X[128];
uint8_t Y[128];
uint8_t length;
float ptr[4] ;
int count1 = 0,count2 = 0;
float slope;
int dr, kk;


void detect(uint8_t *buff,uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint8_t color) {
		
	if(abs(y1-y0)<=abs(x1-x0))
	{
		length = abs(x1-x0)+1;

		int m = 2 * abs(y0 - y1);
		int x = 0,y = y0;
		if((x1-x0)>=0)
		{
			int i = 0;
			int slope_error = m - (x1 - x0);
			for ( x = x0; x <= x1; x++)
			{
				X[i] = x;
				Y[i] = y;
				i++;
				// Add slope to increment angle formed
				slope_error += m;
				// Slope error reached limit, time to
				// increment y and update slope error.
				if (slope_error >= 0)
				{
					if(y0 - y1>0)
					{
						y--;
						if(y<3)
						{
							y = 3;
						}
					}
					else
					{
						y++;
						if(y>60)
						{
							y = 60;
						}
					}
					slope_error  -= 2 * (x1 - x0);
				}
			}
		}
		
		else
		{
			int i = 0;
			int slope_error = m - (x0 - x1);
			for ( x = x0; x >= x1; x--)
			{
				X[i] = x;
				Y[i] = y;
				i++;
				// Add slope to increment angle formed
				slope_error += m;
				// Slope error reached limit, time to
				// increment y and update slope error.
				if (slope_error >= 0)
				{
					if(y0 - y1>0)
					{
						y--;
						if(y<3)
						{
							y = 3;
						}
					}
					else
					{
						y++;
						if(y>60)
						{
							y = 60;
						}
					}
					slope_error  -= 2 * (x0 - x1);
				}
			}
		}
		
	}
	
	if(abs(y1-y0)>abs(x1-x0))
	{
		length = abs(y1-y0)+1;
		int m = 2 * abs(x1 - x0);
		int x = x0, y = y0;
		if((y0-y1)>=0)
		{
			int i = 0;
			int slope_error = m - (y0 - y1);
			for ( y = y0; y >= y1; y--)
			{
				
				X[i] = x;
				Y[i] = y;
				i++;
				//setpixel(buff,X[i],Y[i],0);
				// Add slope to increment angle formed
				slope_error += m;
				// Slope error reached limit, time to
				// increment y and update slope error.
				if (slope_error >= 0)
				{
					if(x1 - x0>0)
					{
						x++;
						if(x>122)
						{
							x = 122;
						}
					}
					else
					{
						x--;
						if(x<5)
						{
							x = 5;
						}
					}
					slope_error  -= 2 * (y0 - y1);
				}
			}
		}
		else
		{
			int i = 0;
			int slope_error = m - (y1 - y0);
			for ( y = y0; y <= y1; y++)
			{
				
				X[i] = x;
				Y[i] = y;
				i++;
				//setpixel(buff,X[i],Y[i],0);
				// Add slope to increment angle formed
				slope_error += m;
				// Slope error reached limit, time to
				// increment y and update slope error.
				if (slope_error >= 0)
				{
					if(x1 - x0>0)
					{
						x++;
						if(x>122)
						{
							x = 122;
						}
					}
					else
					{
						x--;
						if(x<5)
						{
							x = 5;
						}
					}
					slope_error  -= 2 * (y1 - y0);
				}
			}
		}
	}
}

uint8_t  path_algorithm(uint8_t *buff,uint8_t x0, uint8_t y0, uint8_t r,uint8_t dir,float k)
{
	memset(X,0,128*sizeof(uint8_t));
	memset(Y,0,128*sizeof(uint8_t));
	//Initial points and Final Points
	
	if (x0 == 122 || x0 == 5)
	{
		dir = !dir;
		k = -k;			
	}
	
	if (y0 == 60 || y0 == 3)
	{
		k = -k;	
	}
	
	uint8_t sphere_y = 0;
	uint8_t sphere_x = 0;
	
	
	
	if ((dir == 0)&(k<=0))
	{
		float temp=(float)(y0-60)/(x0-122);
		if(abs(k*100)<=abs(temp*100))
		{
			sphere_x = 122;		
			sphere_y = -k*(122-x0)+y0;
		}
		else
		{
			sphere_x = (y0-60)/k+x0;
			sphere_y = 60;
		}
	}
	if ((dir == 0)&(k>0))
	{
		float temp=(float)(y0-3)/(x0-122);
		if(abs(k*100)<=abs(temp*100))
		{
			sphere_x = 122;
			sphere_y = -k*(122-x0)+y0;
		}
		else
		{
			sphere_x = (y0-3)/k+x0;
			sphere_y = 3;
		}
	}

	if ((dir == 1)&(k<=0))
	{
		float temp=(float)(y0-3)/(x0-5);
		if(abs(k*100)<=abs(temp*100))
		{
			sphere_x = 5;
			sphere_y = k*(x0-5)+y0;
		}
		else
		{
			sphere_x = (y0-3)/k+x0;
			sphere_y = 3;
			
		}
	}
	if ((dir == 1)&(k>0))
	{
		float temp=(float)(y0-60)/(x0-5);
		if(abs(k*100)<=abs(temp*100))
		{
			sphere_x = 5;
			sphere_y = k*(x0-5)+y0;
		}
		else
		{
			sphere_x = -(60-y0)/k+x0;
			sphere_y = 60;
		}
	}
	//putstring("  ///// ");
	//putnum(sphere_x );
	//putstring("   ");
	//putnum(sphere_y );
	//putstring("   ");
	//Generate X & Y arrays with all points for the sphere move
	detect(buff,x0, y0, sphere_x, sphere_y,0);
	//putnum(length );
	//putstring("   ");
	//putnum(10*k );
	//putstring("   ");
	//putnum(dir);
	//putstring("   \n");

	ptr[0] = sphere_x;
	ptr[1] = sphere_y;
	ptr[2] = dir;
	ptr[3] = k;
	
	return 0;
}

uint8_t read_adc_x()
{
	DDRC &= ~(1<<PORTC5);
	DDRC &= ~(1<<PORTC3);
	

	DDRC |= (1<<PORTC4) | (1<<PORTC2);//Y+,Y- output
	PORTC |= (1<<PORTC4);
	
	adc_result = adc_read(5);//X-
	if(adc_result>943)
	{
		adc_result=943;
	}
	if(adc_result<120)
	{
		adc_result=120;
	}
	xa = -adc_result*20/125+138;
	
	if(xa>120)
	{
		xa=120;
	}
	if(xa<0)
	{
		xa=0;
	}
	/*putstring("   x_adc:  ");
	putnum(adc_result);
	putstring(" x:   ");
	putnum(x);
	putstring(" \n  ");*/
	return xa;
}

uint8_t read_adc_y()
{
	DDRC &= ~(1<<PORTC4);
	DDRC &= ~(1<<PORTC2);
	
	DDRC |= (1<<PORTC5) | (1<<PORTC3);//X+,X- output
	PORTC |= (1<<PORTC3);
	
	adc_result = adc_read(4);//Y-
	if(adc_result>764)
	{
		adc_result=764;
	}
	if(adc_result<176)
	{
		adc_result=176;
	}
	
	ya = adc_result*3/28-132/7;
	
	
	if(ya>63)
	{
		ya=63;
	}
	if(ya<0)
	{
		ya=0;
	}
	//putstring(" y_adc:    ");
	//putnum(adc_result);
	putstring(" y:   ");
	//putnum(ya);
	
	return ya;
}

int main(void)
{
	//setting up the gpio for backlight
	DDRD |= 0x80;
	PORTD &= ~0x80;
	PORTD |= 0x00;
	
	DDRB |= 0x01;
	PORTB &= ~0x01;
	PORTB |= 0x00;
	int x0 = 0, y0 = 0,mode = 0;
	int acc0 = 0,acc = 0;
	
	//lcd initialisation
	lcd_init();
	lcd_command(CMD_DISPLAY_ON);
	lcd_set_brightness(0x18);
	write_buffer(buff);
	_delay_ms(1000);
	clear_buffer(buff);
	

	adc_init();// initialize adc
	
	DDRB |= (1 << PORTB1);
	
	//initialize  interrupt
	TCCR1A = (1 << WGM10); //Set PWM Bit
	TCCR1B = (1 << WGM12);
	sei();
	TCCR1B |=  (1 << CS10) | (1 << CS11);
	DDRB |= (1 << PORTB3);
	

	
	while(1)
	{
		acc0 = 0;
		drawstring(buff, 43, 2, "1 Player");
		drawstring(buff, 43, 4, "2 Players");
		drawstring(buff, 43, 6, "Acc Mode");
		
		
		y = read_adc_y();
		x = read_adc_x();
		putnum(y);
		putstring("   ");
		putnum(x);
		putstring("   \n");
		
		if((x>=43)&(x<=43+40)&(((y>=12)&(y<=22))||((y>=24)&(y<32))||((y>=38)&(y<48))))
		{
			if((x>=43)&(x<=43+40)&(y>=12)&(y<22))
			{
				mode = 1;
			}
			if((x>=43)&(x<=43+40)&(y>=24)&(y<32))
			{
				mode = 0;
			}
			if((x>=43)&(x<=43+40)&(y>=38)&(y<48))
			{
				mode = 2;
				for(int i=0;i<50;i++)
				{
					
					acc0 += adc_read(1);
					//putnum(adc_read(1));
					putstring("   \n");
					
					
				}
				acc0 /= 50;
			}
			clear_buffer(buff);
			drawbackground();
			
			slope = rand()%100;
			slope = 9*slope/500 +0.5;
			dr = rand()%2;
			kk = rand()%3;
			if((kk==0) || (kk==2))
			{
				slope = -slope;
			}
			path_algorithm(buff,63, 32, 2  ,dr,slope);
			
			int i = 0,flag = 1;
			while (flag)
			{
				
				record1(count1);
				record2(count2);
				
				
				for(int i=0;i<63;i+=6)
				{
					drawline(buff,63,i,63,i+3,0);
				}
				
				
				switch(mode)
				{
					case 0:
					
					y = read_adc_y();
					x = read_adc_x();
					pad_c0 = movepad0(buff,pad_c0,x,y);
					pad_c1 = movepad1(buff,pad_c1,x,y);
					
					break;
					case 1:
					
					y = read_adc_y();
					x = read_adc_x();
					pad_c0 = movepad0(buff,pad_c0,x,y);
					if(pad_c1>y0)
					{
						pad_c1 = movepad1(buff,pad_c1,64,1);
					}
					if(pad_c1<y0)
					{
						pad_c1 = movepad1(buff,pad_c1,64,62);
					}
					
					
					break;
					case 2:
					acc = adc_read(1);
					putnum(acc0);
					putstring("  ");
					putnum(acc);
					putstring(" \n  ");
					if(acc<(acc0-10))
					{
						pad_c0 = movepad0(buff,pad_c0,62,1);
						putstring("flag1 \n  ");
					}
					if(acc>(acc0+10))
					{
						pad_c0 = movepad0(buff,pad_c0,62,62);
						putstring("flag2 \n  ");
					}
					if(pad_c1>y0)
					{
						pad_c1 = movepad1(buff,pad_c1,64,1);
					}
					if(pad_c1<y0)
					{
						pad_c1 = movepad1(buff,pad_c1,64,62);
					}
					break;
				}

				write_buffer(buff);

				
				//putnum(y);
				//putstring("   ");
				//putnum(x);
				//putstring("   \n");
				
				clearcircle(buff,X[i], Y[i], 2,0);
				write_buffer(buff);
				if (i < length-1 )
				{
					i++;
				}
				
				fillcircle(buff,X[i], Y[i], 2,0);
				x0 = X[i]; y0 = Y[i];
				//putnum(i);
				//putstring("   ");
				//putnum(X[i] );
				//putstring("   ");
				//putnum(Y[i] );
				//putstring("   \n");
				
				write_buffer(buff);
				if (i == length-1 )
				{
					if((x0==5)&((y0>=pad_c0+7)||(y0<=pad_c0-7)))
					{
						clearcircle(buff,X[i], Y[i], 2,0);
						OCR1A = 107;
						TIMSK1 |= (1 << OCIE1A);
						blink_white();
						_delay_ms(200);
						blink_white();
						TIMSK1 &= ~(1 << OCIE1A);
						PORTB &= ~(1 << PORTB3);
						
						slope = rand()%100;
						slope = 9*slope/500 +0.5;
						dr = rand()%2;
						kk = rand()%3;
						if((kk==0) || (kk==2))
						{
							slope = -slope;
						}
						path_algorithm(buff,63, 32, 2  ,dr,slope);
						
						count2++;
						if(count2>=3)
						{
							count1 = 0;
							count2 = 0;
							flag = 0;
							clear_buffer(buff);
							drawstring(buff, 33, 2, "Player 2 Win!");
							_delay_ms(500);
							clear_buffer(buff);
							break;
						}
						
					}
					else if((x0==122)&((y0>=pad_c1+7)||(y0<=pad_c1-7)))
					{
						clearcircle(buff,X[i], Y[i], 2,0);
						OCR1A = 60;
						TIMSK1 |= (1 << OCIE1A);
						blink_purple();
						_delay_ms(200);
						blink_purple();
						TIMSK1 &= ~(1 << OCIE1A);
						PORTB &= ~(1 << PORTB3);
						
						slope = rand()%100;
						slope = 9*slope/500 +0.5;
						dr = rand()%2;
						kk = rand()%3;
						if((kk==0) || (kk==2))
						{
							slope = -slope;
						}
						path_algorithm(buff,63, 32, 2  ,dr,slope);
						
						count1++;
						if(count1>=3)
						{
							count1 = 0;
							count2 = 0;
							flag = 0;
							clear_buffer(buff);
							drawstring(buff, 33, 2, "Player 1 Win!");
							_delay_ms(500);
							clear_buffer(buff);
							break;
							
						}
					}
					else if(((x0==122)||(x0==5))&((y0<pad_c1+7)||(y0>pad_c1-7)))
					{
						path_algorithm(buff,x0, y0, 2 ,ptr[2],ptr[3]);
						OCR1A = 120;
						TIMSK1 |= (1 << OCIE1A);
						_delay_ms(20);
						TIMSK1 &= ~(1 << OCIE1A);
						PORTB &= ~(1 << PORTB3);
					}
					else
					{
						path_algorithm(buff,x0, y0, 2 ,ptr[2],ptr[3]);
						OCR1A = 80;
						TIMSK1 |= (1 << OCIE1A);
						_delay_ms(20);
						TIMSK1 &= ~(1 << OCIE1A);
						PORTB &= ~(1 << PORTB3);
					}
					
					i = 0;
					
				}
				
			}
		}
		
		
	}
	
	
}

ISR(TIMER1_COMPA_vect)
{
	//toggle port to generate pulses
	PORTB ^= (1 << PORTB3);
}