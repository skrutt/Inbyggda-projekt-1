#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#define JOYSTICK_LEFT_CHANNEL		1
#define JOYSTICK_RIGHT_CHANNEL		0

#include <avr/io.h>

typedef struct Joystick {
	uint8_t left;				// Left joystick throttle
	uint8_t right;				// Right joystick throttle
	uint8_t deadzone_left;		// Deadzone for left joystick
	uint8_t deadzone_right;		// Deadzone for right joystick
	uint8_t middle_left;		// Center value for left joystick
	uint8_t middle_right;		// Center value for right joystick
	uint8_t idle;				// Set to one if the joystick is at center position
} Joystick;			
	
void joystick_init(Joystick*, uint8_t, uint8_t, uint8_t, uint8_t);
	
uint8_t joystick_get_throttle(Joystick*, uint8_t);
uint8_t joystick_get_throttle_dir_combined(Joystick*, unsigned char);
	
#endif
