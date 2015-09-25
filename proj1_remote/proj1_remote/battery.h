#ifndef BATTERY_H_
#define BATTERY_H_

uint16_t batteryCounter;

float readBatteryVoltage();
void initBatteryCheckTimer();

#endif