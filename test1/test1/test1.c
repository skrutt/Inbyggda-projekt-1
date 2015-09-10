/*
 * test1.c
 *
 * Created: 2015-08-31 13:24:59
 *  Author: peol0071
 */ 


#include <avr/io.h>
#include <stdio.h>
#include "avr/interrupt.h"


//Super global defines
//super_paketet incomming;

//Function read adc
//Author 
#define	SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define	CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define	CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))
#define	F_CPU	8E6

//Own libs
#include "Usartlib.h"
#include "super_paketet.h"

#include "util/delay.h"


FILE mystdout;

int uart_putchar(char c, FILE *stream) 
{ 
	send_c(c);
	return 0;
}



int main()
{
	DDRD = 2;				// output
	DDRB = 0xff;					// input
	SETBIT(PORTB,PB0);			// enable pull-up
	SETBIT(PORTB,PB1);			// enable pull-up
	sei();
	InitUART(9600);	
	
	fdev_setup_stream(&mystdout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &mystdout;			
	char	ch = 0;
	send_string("Wait", 4);

	while(1)
	{
		
		send_string(".", 1);
		_delay_ms(20);
		super_paketet inc = check_for_package();
		if (inc.adress != 0 && inc.type == 1)
		{
			printf("Paket! payload: %d", inc.payload[0]);
		}
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
