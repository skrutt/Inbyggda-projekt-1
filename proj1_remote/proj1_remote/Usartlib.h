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

/* Read and write functions */
char ReceiveByte(char * result);
char ReceiveByteBlocking();


void send_c(char c);

inline void flush_usart();
int8_t send_string_blocking(char data[], uint8_t length);
int8_t send_string(char data[], uint8_t length);

#endif	//ifndef __USARTLIB_H