/*
 * SevenSeg.h
 *
 * Created: 2015-09-14 13:12:32
 *  Author: joje0019
 */ 


#ifndef SEVENSEG_H_
#define SEVENSEG_H_

#ifndef F_CPU
/* prevent compiler error by supplying a default */
# warning "F_CPU not defined for <SevenSeg.h>"
#define F_CPU 8000000UL
#endif


#include <avr/io.h>

void initTimer2();
void initSevenSeg();
//void setNumber(uint8_t number);

uint8_t obstacleDistance;

#endif /* SEVENSEG_H_ */