#ifndef mega_h
#define mega_h

#define DEBUG

#include <Arduino.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>

#define W_POSITION A2
#define BUT_DOWN 30
#define BUT_UP 28
#define BUT_SET 24
#define POWER_CONTROL 26
#define ERR_SIGNAL 6


void mega_dev_init();
void mega_dev_all_off();


#endif