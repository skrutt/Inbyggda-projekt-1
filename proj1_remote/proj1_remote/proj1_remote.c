/*
 * proj1_remote.c
 *
 * Created: 2015-09-10 12:06:25
 *  Author: jiut0001
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "joystick.h"
#include "Usartlib.h"
#include "super_paketet.h"
#include "SevenSeg.h"

/* --------- Transmit mode --------- */
#define TRANSMIT_PIN		1 << 4
#define TRANSMIT_PORT		PORTC
#define TRANSMIT_PORT_DDR	DDRC

//Set transmit high
void enable_transmit()
{
	TRANSMIT_PORT_DDR |= TRANSMIT_PIN;
	TRANSMIT_PORT	|= TRANSMIT_PIN;
}
//Set transmit low
void disable_transmit()
{
	TRANSMIT_PORT_DDR |= TRANSMIT_PIN;
	TRANSMIT_PORT	&= ~TRANSMIT_PIN;
}

/* ------------------------------- */


void usart_putchar(char data)
{
	// Stay here until data buffer is empty
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = data;
	
}

int usart_putchar_printf(char var, FILE *stream) {
	if (var == '\n') usart_putchar('\r');
	usart_putchar(var);
	return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);


int main(void)
{
	stdout = &mystdout;
	Joystick js;
	joystick_init(&js, 6, 6, 145, 150);
	
	InitUART(9600);
	
	sei();
	initSevenSeg();

	obstacleDistance = 0;		//Distance to obstacle as measured by IR sensor
		
	super_paketet package;
	package.adress = ADRESS;
	
	//Set up superpaketet
	set_link_mode_functions(enable_transmit, disable_transmit);	
	enable_transmit();
	
	
	uint16_t thScaling = 10000;
	
	_delay_ms(1000);
	package.type = 0x02;
	package.payload[0] = thScaling & 0xFF;
	package.payload[1] = (thScaling >> 8);
	send_package(package);
	_delay_ms(25);
	
	
	// Counter for timing distance requests
	volatile uint8_t distReqCnt = 0;
	uint16_t th_scale = 5000;
	DDRC &= ~(1 << 5);

	while(1)
    {		
		//printf("%d, ", joystick_get_throttle(&js, JOYSTICK_LEFT_CHANNEL));
		//printf("%d \n", joystick_get_throttle(&js, JOYSTICK_RIGHT_CHANNEL));
	
    
    	
		package.type = 0x01;
        package.payload[0] = joystick_get_throttle(&js, JOYSTICK_LEFT_CHANNEL);
		package.payload[1] = joystick_get_throttle(&js, JOYSTICK_RIGHT_CHANNEL);
        //uint8_t thRight = joystick_get_throttle_dir_combined(&js, 'r');
		send_package(package);	
		
		distReqCnt++;
		
		switch(distReqCnt)
		{
			 case 10:
				_delay_ms(3);
				package.type = 0x07;	//Demand distance data
				if(send_request_package(&package, 20) != -1) {
					obstacleDistance = package.payload[0];
				}
				break;
			case 20:
				PORTC |= 1 << 5;
				if ((PINC & (1 << 5)) == 0)
				{
					th_scale = 10000;
				}else{
					th_scale = 5000;
				}
				
				package.type = 0x02;
				*(uint16_t*)package.payload = th_scale;
				//uint8_t thRight = joystick_get_throttle_dir_combined(&js, 'r');
				send_package(package);
				//Last one, reset
				distReqCnt = 0;
				break;
		}
		
		
		_delay_ms(3);
		

    }
}