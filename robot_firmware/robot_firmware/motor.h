#ifndef MOTOR_H_
#define MOTOR_H_

#include <avr/io.h>

typedef struct Motor {
	int8_t throttle_offset;		// Used for compensating for individual motor characteristics.
	volatile uint8_t *OCR;		// Which PWM output compare register/pin to use.
	uint8_t direction;			// 0 = reverse, 1 = forward
	volatile uint8_t *dirPort;
	uint8_t dirPin;
} Motor;

void motor_init(Motor*, uint8_t, volatile uint8_t*, volatile uint8_t*, volatile uint8_t*, uint8_t);
void motor_pwm_init();

void motor_set_throttle(Motor*, uint8_t);
void motor_set_direction(Motor*, uint8_t);

#endif