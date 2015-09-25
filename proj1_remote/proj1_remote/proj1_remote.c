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
#include <avr/sleep.h>

#include "joystick.h"
#include "Usartlib.h"
#include "super_paketet.h"
#include "SevenSeg.h"
#include "sleepMode.h"
#include "battery.h"

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

int main(void)
{	
	/*---------- Init Joysticks ----------*/
	Joystick js;
	joystick_init(&js, 3, 3, 122, 127);
	
	/*---------- Init UART and superpaketet ----------*/
	InitUART(9600);	
	
	super_paketet package;
	package.adress = ADRESS;	
	set_link_mode_functions(enable_transmit, disable_transmit);
	enable_transmit();
	
	/*---------- Init wakeup ISR and 7-seg ----------*/
	initWakeupISR();
	sei();
	initSevenSeg();
	
	// Init low battery led
	DDRD |= (1 << 2);
	initBatteryCheckTimer();
	

	obstacleDistance = 0;			// Distance to obstacle as measured by IR sensor	
	uint16_t th_scale = 5000;		// Throttle scaling, 0 - 10000
	
	// Counter for timing distance requests
	volatile uint8_t distReqCnt = 0;
	DDRC &= ~(1 << 5);
	
	// Manual sleep button
	DDRD &= ~(0 << 6);
	PORTD |= (1 << 6);
	
	//Radio enable pin
	DDRB |= (1 << 7);
	PORTB |= (1 << 7);
	
	//Power measure circuit vcc
	DDRD |= (1 << 7);
	PORTD &= ~(1 << 7);

	while(1)
    {		
		// Check if manual sleep button is pressed, if so go to sleep
		if(!(PIND & (1<<6))) {
			package.type = 0x03;
			send_package(package);
			_delay_ms(20);
			putToSleep();
		}
    	
		// Setup speed/direction package
		package.type = 0x01;
        package.payload[0] = joystick_get_throttle(&js, JOYSTICK_LEFT_CHANNEL);
		package.payload[1] = joystick_get_throttle(&js, JOYSTICK_RIGHT_CHANNEL);
		
		// Check if it should enter sleepmode
		if(package.payload[0] == 127 && package.payload[1] == 127) {
			if(js.idle != 1) {
				js.idle = 1;
				startSleepTimer();
			}
		}
		else {
			stopSleepTimer();
			js.idle = 0;
		}
		
		send_package(package);	// Send speed/direction command
		
		distReqCnt++;
		
		switch(distReqCnt)
		{
			// Send distance data request
			case 10:
				_delay_ms(3);
				package.type = 0x07;			//Demand distance data
				
				// Send package and wait for response package from the vehicle (20ms timeout)
				if(send_request_package(&package, 20) != -1) {
					obstacleDistance = package.payload[0];
				}
				
				break;
			
			// Send throttle scaling
			case 20:
				PORTD |= 1 << 5;
				if ((PIND & (1 << 5)) == 0)		// Check if the turbo switch is set
				{
					th_scale = 10000;
				}else{
					th_scale = 5000;
				}
				
				package.type = 0x02;
				*(uint16_t*)package.payload = th_scale;
				send_package(package);
				distReqCnt = 0;					//Last one, reset
				break;
		}
				
		_delay_ms(3);
		
    }
}