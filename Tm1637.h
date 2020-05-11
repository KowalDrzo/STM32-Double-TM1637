#pragma once

#include "gpio.h"

#define RED 1
#define BLUE 0

void tm1637DisplayDecimal(uint16_t v, _Bool displaySeparator, _Bool ifRed);

void tm1637SetBrightness(char brightness);

void tm1637SetBriOne(char brightness, _Bool ifRed);

void dispBlink(_Bool ifRed);

uint16_t int2Time(uint16_t integer);
