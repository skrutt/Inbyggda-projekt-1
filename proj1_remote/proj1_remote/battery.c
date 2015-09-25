#include <avr/interrupt.h>
#include <util/delay.h>
#include "battery.h"

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

ISR(TIMER0_OVF_vect) {
	batteryCounter++;
	if(batteryCounter > 200) {		//2000 = ungefär en minut
		
		PORTD |= (1 << 7);	//Enable power to voltage divider
		_delay_ms(2);
		
		float voltage = readBatteryVoltage();
		
		//Enable low battery LED if battery voltage is below 4V
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