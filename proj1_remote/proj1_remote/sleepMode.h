#ifndef SLEEPMODE_H_
#define SLEEPMODE_H_

#include <avr/io.h>
#include <avr/sleep.h>
#include "avr/interrupt.h"

void startSleepTimer();
void stopSleepTimer();

void initWakeupISR();
void putToSleep();


#endif
