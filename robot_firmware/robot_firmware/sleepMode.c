#include "sleepMode.h"
#include <avr/interrupt.h>
#include <avr/power.h>

void putToSleep() {
	set_sleep_mode(SLEEP_MODE_IDLE);

	sleep_enable();
	sei();
	sleep_cpu();
	sleep_disable();
}

void disableIR() {
	power_adc_disable(); // Turn of ADC
	//TODO: Stäng av IR-sensor
	PORTD &= ~(1 << 2);		//Turn off status LED
}

void enableIR() {
	power_adc_enable(); // Turn of ADC
}