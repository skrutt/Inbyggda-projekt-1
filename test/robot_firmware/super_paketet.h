/*
 * super_paketet.h
 *
 * Created: 2015-09-10 13:28:58
 *  Author: peol0071
 */ 
#include <avr/io.h>

#ifndef SUPER_PAKETET_H_
#define SUPER_PAKETET_H_

/* initialize UART */
void InitUART( unsigned int);

typedef struct __attribute__((packed)) {
	//uint16_t preamble;
	uint8_t adress;
	uint8_t type;
	uint8_t payload[2];
	uint8_t crc;
} super_paketet;

#define ADRESS 0x66//6

//Fetch new packages if there is one
super_paketet check_for_package();

//pad package and send over usart
void send_package(super_paketet outgoing_package);



#endif /* SUPER_PAKETET_H_ */