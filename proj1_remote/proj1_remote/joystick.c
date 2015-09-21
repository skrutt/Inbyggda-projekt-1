#include "joystick.h"

void joystick_init(Joystick *js, uint8_t deadzone_left, uint8_t deadzone_right, uint8_t middle_left, uint8_t middle_right) {
	js->deadzone_left = deadzone_left;
	js->deadzone_right = deadzone_right;
	js->middle_left = middle_left;
	js->middle_right = middle_right;

	//----------- Config ADC -------------
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  	// ADC prescaler to 128 (gives 125kHz with 8MHz cpu).
	ADMUX |= (1 << REFS0); 						// Use AVCC as reference.
	
	ADCSRA |= (1 << ADEN); 						// Enable ADC
	ADCSRA |= (1 << ADSC);  					// Start conversion
	
}

void joystick_update(Joystick *js) {
	//js->x = joystick_read_axis(JOYSTICK_X_CHANNEL);
	//js->y = joystick_read_axis(JOYSTICK_Y_CHANNEL);
}

uint8_t joystick_get_throttle(Joystick *js, uint8_t ch) {
	ADMUX &= 0xF0;						//Clear the old channel
	ADMUX |= ch;
	ADCSRA |= (1<<ADSC);                //Start new conversion
	while(ADCSRA & (1<<ADSC));          //Wait until the conversion is done
	ADCSRA |= (1<<ADSC);                //Start new conversion
	while(ADCSRA & (1<<ADSC));          //Wait until the conversion is done
	
	uint8_t mid_min = 127;
	uint8_t mid_max = 127;
	
	if(ch == JOYSTICK_LEFT_CHANNEL) {
		mid_min = js->middle_left - js->deadzone_left;
		mid_max = js->middle_left + js->deadzone_left;
	}
	else if(ch == JOYSTICK_RIGHT_CHANNEL) {
		mid_min = js->middle_right - js->deadzone_right;
		mid_max = js->middle_right + js->deadzone_right;		
	}
	
	uint8_t th = (ADCW>>2);
	
	if(th >= mid_min && th <= mid_max) {
		th = 127;
	}
	
	else if (th < mid_min) {
		th = 127-(((float)(mid_min - th)/(float)(mid_min)) * 127);
	}
	else if (th > mid_max) {
		th = (((float)(th-mid_max)/(float)(255-mid_max)) * 127) + 127;
	}

	return th;
}