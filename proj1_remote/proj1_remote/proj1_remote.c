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

/* --------- Transmit mode --------- */
#define TRANSMIT_PIN		1 << 4
#define TRANSMIT_PORT		PORTC
#define TRANSMIT_PORT_DDR	DDRC

uint16_t batteryCounter = 0;

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

//DEBUG ONLY:
void usart_putchar(char data)
{
	// Stay here until data buffer is empty
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = data;
	
}

//DEBUG ONLY:
int usart_putchar_printf(char var, FILE *stream) {
	if (var == '\n') usart_putchar('\r');
	usart_putchar(var);
	return 0;
}

float readBatteryVoltage() {
	
	uint8_t adcVal = 0;
	float voltage;
		
	ADMUX |= (1 << REFS0) | (1 << REFS1);	// Use 1.1V internal reference
	_delay_ms(10);
		
	ADMUX &= 0xF0;						// Clear the old channel
	ADMUX |= 5;
	ADCSRA |= (1<<ADSC);                // Start new conversion
	while(ADCSRA & (1<<ADSC));          // Wait until the conversion is done
	ADCSRA |= (1<<ADSC);                // Start new conversion
	while(ADCSRA & (1<<ADSC));          // Wait until the conversion is done
		
	adcVal = (ADCW>>2);		
		
	voltage =  (((float)adcVal/255.0)*1.025) * 5.4;
		
	ADMUX &= ~(1 << REFS1); 			// Use AVCC as reference.
	_delay_ms(10);
		
	return voltage;	
}

//FLYTTA TILL EGEN FIL:
ISR(TIMER0_OVF_vect) {
	batteryCounter++;
	if(batteryCounter > 200) {		//2000 = ungefär en minut
		
		PORTD |= (1 << 7);	//Enable power to voltage divider
		_delay_ms(2);
		
		float voltage = readBatteryVoltage();
		if(voltage < 4) {
			PORTD |= (1 << 2);
		}
		else {
			PORTD &= ~(1 << 2);
		}
		batteryCounter = 0;
		
		PORTD &= ~(1 << 7);	//Disable power to voltage divider
	}
}

void initBatteryCheckTimer() {
	TCCR0B = 1 << CS02 | 0 << CS01 | 1 << CS00;		//Prescaler 1024
	TIMSK0 = (1<<TOIE0);							//Enable Timer 2 overflow flag
}


static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);	//DEBUG ONLY

int main(void)
{	
	stdout = &mystdout;		//DEBUG ONLY
	
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
	
	/*
	while(1) {
		printf("jsLeft : %d, jsRight: %d\n", joystick_get_throttle(&js, JOYSTICK_LEFT_CHANNEL), joystick_get_throttle(&js, JOYSTICK_RIGHT_CHANNEL));
		
		_delay_ms(5000);
	}
	*/

	while(1)
    {		
		// Check if manual sleep button is pressed, if so go to sleep
		// Move this to interrupt pin?
		if(!(PIND & (1<<6))) {
			package.type = 0x03;
			send_package(package);
			_delay_ms(20);
			send_package(package);
			_delay_ms(20);
			send_package(package);
			_delay_ms(20);
			send_package(package);
			_delay_ms(20);
			send_package(package);
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