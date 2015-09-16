/*
 * robot_firmware.c
 *
 *  Created: 2015-08-31 13:24:59
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
#include "motor.h"

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
	
	//Set up motor pwm
	Motor leftMotor;
	Motor rightMotor;
	//left OCA & PD7
	motor_init(&leftMotor, 10, &OCR0A, &DDRD, &PORTD, 7);
	//right OCB & PB7
	motor_init(&rightMotor, 10, &OCR0B, &DDRB, &PORTB, 7);

	motor_pwm_init(); 
	

	sei();
	InitUART(9600);	
	
	fdev_setup_stream(&mystdout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &mystdout;			
	_delay_ms(5);
	
	send_string(" Wait", 5);
	
	int thLeft = 127,  thRight = 127;
	
	static float th_scale = 1;
	
	while(1)
	{
				
 		_delay_ms(3);
		super_paketet inc = check_for_package();
		if (inc.adress != 0)
		{
			switch(inc.type)
			{
				case 1:
					thLeft = inc.payload[0];
					thRight = inc.payload[1];
					//printf("Paket! %d & %d\n\r", inc.payload[0], inc.payload[1]);
					break;
				case 2:		//Throttle scaling
					th_scale = *(uint16_t*)inc.payload / 10000.0;
					printf("Paket! %d & %d\n\r", inc.payload[0], inc.payload[1]);
					break;
				default:
					send_string(".", 1);
					break;
			}
		}
		
		
		if(thLeft < 127) {
			motor_set_direction(&leftMotor, 0);
			motor_set_throttle(&leftMotor, 127 - thLeft, th_scale);
		}
		else 
		{
			motor_set_direction(&leftMotor, 1);
			motor_set_throttle(&leftMotor, thLeft - 127, th_scale);
		}
		
		if(thRight < 127) {
			motor_set_direction(&rightMotor, 0);
			motor_set_throttle(&rightMotor, 127 - thRight, th_scale);
		}
		else {
			motor_set_direction(&rightMotor, 1);
			motor_set_throttle(&rightMotor, thRight - 127, th_scale);
		}
		
	}
}
