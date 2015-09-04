/*
 * test1.c
 *
 * Created: 2015-08-31 13:24:59
 *  Author: peol0071
 */ 


#include <avr/io.h>
#include "avr/interrupt.h"


typedef struct __attribute__((packed)) {
		uint8_t adress;
		uint8_t type;
		uint8_t payload[1];
		int crc;
} super_paketet;


//Super global defines
super_paketet incomming;

//Function read adc
//Author 
#define	SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define	CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define	CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))
#define	F_CPU	8E6

#include "Usartlib.h"
#include "util/delay.h"

int main()
{
	DDRD = 2;				// output
	DDRB = 0xff;					// input
	SETBIT(PORTB,PB0);			// enable pull-up
	SETBIT(PORTB,PB1);			// enable pull-up
	sei();
	InitUART(9600);				
	char	ch = 0;

	while(1)
	{
		ch = ReceiveByteBlocking();
		send_string("Hello!!\n\r", 9);
		send_c(ch);
		if (ch=='a')
		{
			PORTB = 1<<7;
		}else if (ch=='s')
		{
			PORTB = 1<<6;
		}else if (ch=='d')
		{
			PORTB = 1<<5;
	}else if (ch=='f')
	{
		PORTB = 1<<4;
}else if (ch==' ')
{
	PORTB = 0;
}
		_delay_ms(2);
		}
}