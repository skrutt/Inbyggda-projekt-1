Motor leftMotor;
Motor rightMotor;
motor_init(&leftMotor, 0, &OCR0A, &DDRD, &PORTD, 2);
motor_init(&rightMotor, 0, &OCR0B, &DDRD, &PORTD, 5);

motor_pwm_init();

while(1)
{
	motor_set_direction(&leftMotor, 1);
	motor_set_direction(&rightMotor, 1);
	motor_set_throttle(&leftMotor, 5);
	motor_set_throttle(&rightMotor, 5);
	_delay_ms(2000);
	motor_set_direction(&leftMotor, 0);
	motor_set_direction(&rightMotor, 0);
	motor_set_throttle(&leftMotor, 20);
	motor_set_throttle(&rightMotor, 45);
	_delay_ms(2000);
	motor_set_throttle(&leftMotor, 230);
	motor_set_throttle(&rightMotor, 230);
	_delay_ms(2000);
}