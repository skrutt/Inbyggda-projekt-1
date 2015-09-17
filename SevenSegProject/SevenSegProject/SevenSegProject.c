/*
 * SevenSegProject.c
 *
 * Created: 2015-09-14 13:29:02
 *  Author: joje0019
 */ 


#define F_CPU 8000000
#define ADC_PIN	5
#include <util/delay.h> // Optimization -O1
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "SevenSeg.h"
uint8_t number;

int irSensor(int adc)
{
	double volt = 0.0035*adc;
	
	int cm = 26.922*pow(volt, -1.245);
	
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

int main(void)
{
	sei();
	initTimer2();
	DDRB = 0xFF;
	DDRC = 0x0F;
	DDRD = 0xFF;
	number = 0;
	ADCSRA |= _BV(ADEN); //Enable adc
	
	//uint8_t number = 0;
	while(1)
	{
		number = irSensor(adc_read(ADC_PIN));
		
		_delay_ms(200);
		//setNumber(number);
		//number++;
		
	}
}