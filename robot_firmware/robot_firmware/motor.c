#include "motor.h"

void motor_init(Motor *motor, uint8_t th_offset, volatile uint8_t *OCR, volatile uint8_t *DDR, volatile uint8_t *dirPort, uint8_t dirPin) {
	motor->throttle_offset = th_offset;
	motor->OCR = OCR;
	motor->dirPort = dirPort;
	motor->dirPin = dirPin;
	*DDR |= (1 << dirPin);
	
	motor_set_direction(motor, 1);
}

void motor_pwm_init() {
	DDRD |= (1 << PORTD6) | (1 << PORTD5);

	TCCR0A |= (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1);		// PWM, Phase Correct
	TCCR0B |= (1 << CS02);										// 256 prescaler (ska väl kanske ändras)
	TCNT0 = 0;													// Reset TCNT0
	
	OCR0A = 0;
	OCR0B = 0;
}

void motor_set_throttle(Motor *motor, uint8_t th, float scale) 
{
	const float max_in = 127;
	float max_ut = 255 - motor->throttle_offset;
	float	perc = th / max_in;
	uint8_t finalThrottle = (perc * max_ut * scale + 0.5) + motor->throttle_offset;
	
	
	
	
	if(th + motor->throttle_offset > 255) {
		finalThrottle = 255;
	}
	else if(th + motor->throttle_offset < 0) {
		finalThrottle = 0;
	}
	//Set to stop!
	if (th == 0)
	{
		finalThrottle = 0;
	}
	
	// Reverse PWM output if going in reverse.
	if(motor->direction == 0) {
		finalThrottle = 255 - finalThrottle;
	}
	
	*(motor->OCR) = finalThrottle;
}

void motor_set_direction(Motor *motor, uint8_t dir) {
	if(dir == 1) {
		motor->direction = dir;
		*(motor->dirPort) &= ~(1 << motor->dirPin);
	}
	else if(dir == 0) {
		motor->direction = dir;
		*(motor->dirPort) |= (1 << motor->dirPin);
	}
}