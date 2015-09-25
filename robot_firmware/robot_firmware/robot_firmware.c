/*
 * robot_firmware.c
 *
 *  Created: 2015-08-31 13:24:59
 *  Author: peol0071
 */ 


#include <avr/io.h>
#include <stdio.h>
#include <avr/power.h>
#include "avr/interrupt.h"


//Super global defines

#define	SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define	CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define	CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))
#define	F_CPU	8E6
#define ADC_PIN	5

//Own libs
#include "Usartlib.h"
#include "super_paketet.h"
#include "motor.h"
#include "sleepMode.h"

#include "util/delay.h"

FILE mystdout;

int uart_putchar(char c, FILE *stream) 
{ 
	send_c(c);
	return 0;
}

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

// Convert adc reading to distance in centimeters
uint8_t irSensor(uint16_t adc)
{
	if(adc < 115) {
		return 99;
	}
	else if(adc > 857) {
		return 0;	
	}
	
	//translate into voltage
	double volt = 0.0035*adc;
	//converts to cm
	uint8_t cm = 26.922*pow(volt, -1.245);
	
	return cm;
}

// Read ADC to get IR-sensor value
uint16_t adc_read(uint8_t adcx) {
	//sets ADMUX to the pin that will be read
	ADMUX	&=	0xf0;
	ADMUX	|=	adcx;

	//start conversion
	ADCSRA |= _BV(ADSC);


	//Waiting loop for conversion
	while ( (ADCSRA & _BV(ADSC)) );
	return ADCW;
}

int main()
{
	DDRD = 2;					// output
	DDRB = 0xff;				// input
	SETBIT(PORTB,PB0);			// enable pull-up
	SETBIT(PORTB,PB1);			// enable pull-up
	
	//Init ADC:
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  	// ADC prescaler to 128 (gives 125kHz with 8MHz cpu).
	ADMUX |= (1 << REFS0); 						// Use AVCC as reference.
		
	ADCSRA |= (1 << ADEN); 						// Enable ADC
	ADCSRA |= (1 << ADSC);  					// Start conversion
	
	// Enable IR sensor
	uint8_t irEnabled = 1;
	DDRB |= (1 << 6);
	PORTB |= (1 << 6);
	uint8_t obstacleDistance = 255;

	Motor leftMotor;
	Motor rightMotor;
	//left OCA & PD7
	motor_init(&leftMotor, 10, &OCR0A, &DDRD, &PORTD, 7);
	//right OCB & PB7
	motor_init(&rightMotor, 10, &OCR0B, &DDRB, &PORTB, 7);

	//Set up motor pwm
	motor_pwm_init(); 
	
	//Set up superpaketet
	set_link_mode_functions(enable_transmit, disable_transmit);
	
	//We are listening here
	disable_transmit();

	sei();
	InitUART(9600);	
	
	//Enable status led (shows if in sleep mode or not)
	DDRD |= (1 << 2);
	PORTD |= (1 << 2);
	
	uint8_t thLeft = 127,  thRight = 127;
	
	static float th_scale = 1;

	
	while(1)
	{			
 		_delay_ms(3);
		obstacleDistance = irSensor(adc_read(ADC_PIN));
		
		super_paketet inc = check_for_package();
		if (inc.adress != 0)
		{
			//Check if package want a response
			if ((inc.type & 0x0f) == 7 && irEnabled)
			{
				//Send response
				enable_transmit();
				inc.type = 7;
				
				// Put distance from irSensor into inc.payload here
				inc.payload[0] = obstacleDistance;
				
				send_package(inc);
				//wait for send
				flush_usart();
				_delay_ms(1);

				disable_transmit();
			}
			switch(inc.type)
			{
				// Speed/direction 
				case 1:
					thLeft = inc.payload[0];
					thRight = inc.payload[1];
					break;
				case 2:		//Throttle scaling
					th_scale = *(uint16_t*)inc.payload / 10000.0;
					printf("Paket! %d & %d\n\r", inc.payload[0], inc.payload[1]);
					break;		
				default:
					break;
			}
		}
		
		// If standing still, disable IR sensor
		if(thRight == 127 && thLeft == 127 && obstacleDistance > 25) {
			irEnabled = 0;
			PORTB &= ~(1 << 6);
			power_adc_disable();
		}
		else {
			irEnabled = 1;
			PORTB |= (1 << 6);	// Enable IR sensor	
			power_adc_enable();
		}


		
		if(obstacleDistance < 20) {
			motor_set_direction(&rightMotor, 1);
			motor_set_throttle(&rightMotor, 127, th_scale);
			motor_set_direction(&leftMotor, 1);
			motor_set_throttle(&leftMotor, 127, th_scale);
		}
		else {
			if(thLeft < 127) {
				motor_set_direction(&leftMotor, 0);
				motor_set_throttle(&leftMotor, 127 - thLeft, th_scale);
			}
			else {
				motor_set_direction(&leftMotor, 1);
				motor_set_throttle(&leftMotor, thLeft - 127, th_scale);
			}
			
			if(thRight < 127) {
				motor_set_direction(&rightMotor, 0);
				motor_set_throttle(&rightMotor, 127 - thRight, th_scale);
			}
			else{
				motor_set_direction(&rightMotor, 1);
				motor_set_throttle(&rightMotor, thRight - 127, th_scale);
			}
		}
		
		
		// Go to idle mode to save some power
		putToSleep();
	}
}
