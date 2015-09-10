#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#define JOYSTICK_X_CHANNEL		0
#define JOYSTICK_Y_CHANNEL		1

#include <avr/io.h>

	typedef struct Joystick {
		uint8_t x;
		uint8_t y;	
		uint8_t deadzone_x;
		uint8_t deadzone_y;
		uint8_t middle_x;				//128 = middle of default range
		uint8_t middle_y;
	} Joystick;
	
	void joystick_init(Joystick*, uint8_t, uint8_t, uint8_t, uint8_t);
	
	void joystick_update(Joystick*);
	
	uint8_t joystick_read_axis(uint8_t);
	uint8_t joystick_get_throttle_dir_combined(Joystick*, unsigned char);
	
#endif
