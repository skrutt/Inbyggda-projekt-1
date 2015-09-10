/*
 * proj1_remote.c
 *
 * Created: 2015-09-10 12:06:25
 *  Author: jiut0001
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "joystick.h"
#include "Usartlib.h"
#include "super_paketet.h"



int main(void)
{
	
	Joystick js;
	joystick_init(&js, 3, 3, 125, 122);
	
	InitUART(9600);
		
	super_paketet package;
	package.adress = ADRESS;
	package.type = 0x01;
	package.payload[0] = 130;
	
	
	
	
    while(1)
    {
		
        package.payload[0] = joystick_get_throttle_dir_combined(&js, 'l');
        uint8_t thRight = joystick_get_throttle_dir_combined(&js, 'r');
		_delay_ms(100);
		send_package(package);
    }
}