#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#define JOYSTICK_LEFT_CHANNEL		1
#define JOYSTICK_RIGHT_CHANNEL		0

#include <avr/io.h>

	typedef struct Joystick {
		uint8_t left;
		uint8_t right;	
		uint8_t deadzone_left;
		uint8_t deadzone_right;
		uint8_t middle_left;				//128 = middle of default range
		uint8_t middle_right;
		uint8_t idle;
	} Joystick;
	
	void joystick_init(Joystick*, uint8_t, uint8_t, uint8_t, uint8_t);
	
	void joystick_update(Joystick*);
	
	uint8_t joystick_get_throttle(Joystick*, uint8_t);
	uint8_t joystick_get_throttle_dir_combined(Joystick*, unsigned char);
	
#endif
