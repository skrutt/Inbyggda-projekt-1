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
	uint8_t adress;
	uint8_t type;
	uint8_t payload[2];
	uint8_t crc;
} super_paketet;

#define ADRESS 0x66//6

#define REQUEST_TYPE	128
#define is_request_type(x) x & REQUEST_TYPE

//Fetch new packages if there is one
super_paketet check_for_package();

//pad package and send over usart
void send_package(super_paketet outgoing_package);
//Send package and wait for answer
int8_t send_request_package(super_paketet *outgoing_package, int timeout);

void set_link_mode_functions(void (*enable_transmit), void (*disable_transmit));


void scramblePackage(super_paketet*);
void descramblePackage(super_paketet*);
uint8_t countTransitions (uint8_t);


#endif /* SUPER_PAKETET_H_ */