/*
 * SevenSeg.c
 *
 * Created: 2015-09-14 13:13:29
 *  Author: joje0019
 */ 

#define F_CPU 8000000
#define	SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define	CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#include <util/delay.h> // Optimization -O1
#include <avr/io.h>
#include <avr/interrupt.h>
#include "SevenSeg.h"
//uint8_t whichSeg = 0;
uint8_t number;
//Timer 2 Overflow
ISR(TIMER2_OVF_vect)
{
	static uint8_t SevenSeg[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
	uint8_t sevenTen;
	uint8_t sevenOne;
	static uint8_t whichSeg = 0;
	//static uint8_t number = 0;

	//sevenTen; //Tiondelar
	//sevenOne; //Endelar

	if(number < 10)
	{
		sevenTen = 0;
		sevenOne = number;
	}
	else
	{
		sevenTen = number / 10;
		sevenOne = number % 10;
	}

	if(whichSeg == 0)
	{
		SETBIT(PORTC, 2);
		CLEARBIT(PORTC, 3);	
		//PORTC = PORTC ~& 0xF;
		PORTB = SevenSeg[sevenTen];
		whichSeg = 1;
	}
	
	else
	{
		SETBIT(PORTC, 3);
		CLEARBIT(PORTC, 2);
		//PORTC = 0x08;
		PORTB = SevenSeg[sevenOne];
		whichSeg = 0;
	}
	
}

void initTimer2()
{	
	TCCR2B = 1 << CS22 | 1 << CS21 | CS20 << 0; //Prescale 256
	
	TIMSK2 = (1<<TOIE2); //Enable Timer 2 overflow flag	
}

/*
uint8_t setNumber(uint8_t number)
{
	return number;
}
*/