#ifndef _ENCLIB_
#define _ENCLIB_

#include <Arduino.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#define SW A12
#define ENC_GND 22
#define ENC0_A A8
#define ENC0_B A9
#define ENC1_A A10
#define ENC1_B A11


int getEncoderValue(int encNum);

bool checkSwitchPushed(void);

bool debugDidSomethingHappen(void);

int getEncoderDir(int encNum);

void resetSwitch(void);

void setupEncoders(void);

#ifdef __cplusplus
}

#endif
#endif

