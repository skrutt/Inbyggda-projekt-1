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
	joystick_init(&js, 2, 4, 126, 131);
	
	InitUART(9600);
	
	sei();
	initSevenSeg();

	obstacleDistance = 49;		//Distance to obstacle as measured by IR sensor
		
	super_paketet package;
	package.adress = ADRESS;
	
	
	uint16_t thScaling = 10000;
	
	_delay_ms(1000);
	package.type = 0x02;
	package.payload[0] = thScaling & 0xFF;
	package.payload[1] = (thScaling >> 8);
	send_package(package);
	_delay_ms(25);

	
    while(1)
    {		
		//printf("%d, ", joystick_get_throttle(&js, JOYSTICK_LEFT_CHANNEL));
		//printf("%d \n", joystick_get_throttle(&js, JOYSTICK_RIGHT_CHANNEL));
		
		package.type = 0x01;
        package.payload[0] = joystick_get_throttle(&js, JOYSTICK_LEFT_CHANNEL);
		package.payload[1] = joystick_get_throttle(&js, JOYSTICK_RIGHT_CHANNEL);
        //uint8_t thRight = joystick_get_throttle_dir_combined(&js, 'r');
		send_package(package);
		_delay_ms(1);
		

    }
}