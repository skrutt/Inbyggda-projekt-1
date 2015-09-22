#include "sleepMode.h"

// Timer overflow interrupt
// Puts the MCU to power_off mode after ~10 sec
ISR(TIMER1_OVF_vect) {
	putToSleep();
}

// Wakeup interrupt called when INT0 goes low
ISR (INT1_vect)
{
}

void startSleepTimer() {
	TCCR1B = (1 << CS12) | (1 << CS10);	// Prescaler 1024
	TIMSK1 = (1 << TOIE1);				// Enable OF-interrupt
}

void stopSleepTimer() {
	TCNT1 = 0;
	TCCR1B &= ~(1 << CS12);
	TCCR1B &= ~(1 << CS10);
}

void initWakeupISR() {
	DDRD &= ~(1 << 3);
	PORTD |= (1 << 3);
	EIMSK |= (1 << INT1);		// Turns on INT0
}

void putToSleep() {
	PORTC &= ~(1 << 2) &  ~(1 << 3);	//Turn off 7seg
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sei();
	sleep_cpu();
	sleep_disable();
}