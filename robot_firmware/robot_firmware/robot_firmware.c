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
#define ADC_PIN	5

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

uint8_t irSensor(uint16_t adc)
{
	double volt = 0.0035*adc;
	
	uint8_t cm = 26.922*pow(volt, -1.245);
	
	return cm;
}

uint16_t adc_read(uint8_t adcx) {
	/* adcx is the analog pin we want to use.  ADMUX's first few bits are
	 * the binary representations of the numbers of the pins so we can
	 * just 'OR' the pin's number with ADMUX to select that pin.
	 * We first zero the four bits by setting ADMUX equal to its higher
	 * four bits. */
	ADMUX	&=	0xf0;
	ADMUX	|=	adcx;

	/* This starts the conversion. */
	ADCSRA |= _BV(ADSC);

	/* This is an idle loop that just wait around until the conversion
	 * is finished.  It constantly checks ADCSRA's ADSC bit, which we just
	 * set above, to see if it is still set.  This bit is automatically
	 * reset (zeroed) when the conversion is ready so if we do this in
	 * a loop the loop will just go until the conversion is ready. */
	while ( (ADCSRA & _BV(ADSC)) );

	/* Finally, we return the converted value to the calling function. */
	return ADC;
}
int main()
{
	DDRD = 2;					// output
	DDRB = 0xff;				// input
	SETBIT(PORTB,PB0);			// enable pull-up
	SETBIT(PORTB,PB1);			// enable pull-up
	ADCSRA |= _BV(ADEN);		//Enable adc	
	//Set up motor pwm
	Motor leftMotor;
	Motor rightMotor;
	//left OCA & PD7
	motor_init(&leftMotor, 10, &OCR0A, &DDRD, &PORTD, 7);
	//right OCB & PB7
	motor_init(&rightMotor, 10, &OCR0B, &DDRB, &PORTB, 7);

	motor_pwm_init(); 
	
	//Set up superpaketet
	set_link_mode_functions(enable_transmit, disable_transmit);
	
	//We are listening here
	disable_transmit();
	

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
			//Check if package want a response
			if (is_request_type(inc.type))
			{
				//Send response
				enable_transmit();
				//Fill data
				//put distance from irsensor into inc.payload here
				inc.payload[0] = irSensor(adc_read(ADC_PIN));
				send_package(inc);
				//wait for send
				flush_usart();
				disable_transmit();
			}
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
