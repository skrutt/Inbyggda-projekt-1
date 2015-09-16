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
	//Set up motor pwm
	Motor leftMotor;
	Motor rightMotor;
	motor_init(&leftMotor, 0, &OCR0A, &DDRD, &PORTD, 3);
	motor_init(&rightMotor, 0, &OCR0B, &DDRD, &PORTD, 4);

	motor_pwm_init();
	
	DDRD = 2;				// output
	DDRB = 0xff;					// input
	SETBIT(PORTB,PB0);			// enable pull-up
	SETBIT(PORTB,PB1);			// enable pull-up
	sei();
	InitUART(9600);	
	
	fdev_setup_stream(&mystdout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &mystdout;			
	char	ch = 0;
	send_string(" Wait", 4);
	
	int thLeft = 127,  thRight = 127;

	while(1)
	{
		
		send_string(".", 1);
		_delay_ms(20);
		super_paketet inc = check_for_package();
		if (inc.adress != 0 && inc.type == 1)
		{
			thLeft = inc.payload[0];
			thRight = inc.payload[1];
			printf("Paket! payload: %d", inc.payload[0]);
		}
		
		if(thLeft < 127) {
			motor_set_direction(&leftMotor, 0);
			motor_set_throttle(&leftMotor, 127 - thLeft);
		}
		else {
			motor_set_direction(&leftMotor, 1);
			motor_set_throttle(&leftMotor, thLeft - 127);
		}
		if(thRight < 127) {
			motor_set_direction(&rightMotor, 0);
			motor_set_throttle(&rightMotor, 127 - thRight);
		}
		else {
			motor_set_direction(&rightMotor, 1);
			motor_set_throttle(&rightMotor, thRight - 127);
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
