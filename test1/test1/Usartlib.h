//
/*
 * Lib for serial communication
 * Petter Olofsson 2015
 *
 * Created: 2015-08-31 13:24:59
 *  Author: peol0071
 */ 

#ifndef __USARTLIB_H
#define __USARTLIB_H

#include <avr/io.h>
#include "avr/interrupt.h"

//Stuff for usart system
#ifndef F_CPU
/* prevent compiler error by supplying a default */
# warning "F_CPU not defined for <Usartlib.h>"
#define F_CPU 8000000UL
#endif

/* initialize UART */
//This is placed here to make use of external define
void InitUART( unsigned int baud )
{
	int baudfactor = (F_CPU/16/baud-1);
	UBRRH = (unsigned char)(baudfactor>>8);	//set the baud rate
	UBRRL = (unsigned char)baudfactor;
	UCSRB = _BV(RXEN) | _BV(TXEN);			//enable UART receiver and transmitter
	/* Set frame format: 8data */
	UCSRC = (3<<UCSZ0);
	//Activate interrupts
	UCSRB |= (1 << UDRIE) + (1 << RXCIE);
}

/* Read and write functions */
char ReceiveByte(char * result);
char ReceiveByteBlocking();


void send_c(char c);


int8_t send_string(char data[], uint8_t length);

#endif	//ifndef __USARTLIB_H