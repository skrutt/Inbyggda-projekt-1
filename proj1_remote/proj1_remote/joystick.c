#include "joystick.h"

void joystick_init(Joystick *js, uint8_t deadzone_x, uint8_t deadzone_y, uint8_t middle_x, uint8_t middle_y) {
	js->deadzone_x = deadzone_x;
	js->deadzone_y = deadzone_y;
	js->middle_x = middle_x;
	js->middle_y = middle_y;

	//------ Config Timer Interrupt	------
	TCCR1B |= (1 << WGM12); 					// Enable CTC mode.
	TIMSK1 |= (1 << OCIE1A);					// Enable timer CTC interrupt.
	
	OCR1A = 60000; 								// Set compare value.
	TCCR1B |= ((1 << CS10) | (1 << CS11)); 		// Start timer with 64 prescaler.
	
	//----------- Config ADC -------------
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  	// ADC prescaler to 128 (gives 125kHz with 8MHz cpu).
	ADMUX |= (1 << REFS0); 						// Use AVCC as reference.
	
	ADCSRA |= (1 << ADEN); 						// Enable ADC
	ADCSRA |= (1 << ADSC);  					// Start conversion
	
}

void joystick_update(Joystick *js) {
	js->x = joystick_read_axis(JOYSTICK_X_CHANNEL);
	js->y = joystick_read_axis(JOYSTICK_Y_CHANNEL);
}

uint8_t joystick_read_axis(uint8_t ch) {
	ADMUX &= 0xF0;						//Clear the old channel
	ADMUX |= ch;
	ADCSRA |= (1<<ADSC);                //Start new conversion
	while(ADCSRA & (1<<ADSC));          //Wait until the conversion is done
	return (ADCW>>2);					//Returns the ADC value from 0 - 256
}


uint8_t joystick_get_throttle_dir_combined(Joystick *js, unsigned char motor) {
	
	uint8_t throttle = 0;
	uint8_t center_x_high = js->middle_x+((float)js->deadzone_x/(float)2);
	uint8_t center_x_low = js->middle_x-((float)js->deadzone_x/(float)2);
	uint8_t center_y_high = js->middle_y+((float)js->deadzone_y/(float)2);
	uint8_t center_y_low = js->middle_y-((float)js->deadzone_y/(float)2);
	
	uint8_t x = joystick_read_axis(JOYSTICK_X_CHANNEL);
	uint8_t y = joystick_read_axis(JOYSTICK_Y_CHANNEL);
	
	//uint8_t x = js->x;		//Only for debug
	//uint8_t y = js->y;		//Only for debug		

	//If only turning (no throttle)
	if(y >= center_y_low && y <= center_y_high){
		if(x >= center_x_low && x <= center_x_high) {
			throttle = 127;
		}
		else {
			if(motor == 'l') {
				throttle = x;
				} else if(motor == 'r') {
				throttle = 128-(x-128);
			}
		}
	}
		
	// If turning right.
	else if(x > center_x_high) {	
		throttle = y;
					
		if(motor == 'r') {
			if(y < center_y_low) {
				throttle = (uint8_t) (y+(((float)(x-128)/(255-y))*128) + 0.5);
			}
			else if(y > center_y_high) {
				throttle = (uint8_t) (y-(((float)(x-128)/y)*128) + 0.5);
			}
		}
	}
		
	// If turning left.
	else if(x < center_x_low) {
		throttle = y;
		
		if(motor == 'l') {			
			if(y < center_y_low) {
				throttle = (uint8_t) (y+(((float)((255-x)-128)/(255-y))*128) + 0.5);  
			}
			else if(y > center_y_high) {
				throttle = (uint8_t) (y-(((float)(128-x)/y)*128) + 0.5);
			}
		}
	}	
	
	else {
		throttle = y;
	}	

		
	return throttle;
}
