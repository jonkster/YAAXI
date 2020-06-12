#ifndef __SWITCHES_H__
#define __SWITCHES_H__
#ifdef __cplusplus
extern "C" {
#endif

#define TOGGLE0_IDX    0
#define TOGGLE1_IDX    1
#define PUSH0_IDX      2
#define PUSH1_IDX      3
#define PUSH2_IDX      4
#define PUSH3_IDX      5
#define TWIST0_IDX     6
#define TWIST1_IDX     7


#define s1 26
#define s2 22
#define s3 24

bool actOnRotations();

bool checkAnySwitchesChanged();

void checkSwitches();

int getLastRot(const int encoderNum);

bool getSwitchChanged(const int switchIdx);

int getSwitchState(const int switchIdx);

void initSwitches();

void lightSet(const int ledNum, const bool onoff);

void lightsAll(const bool onoff);

void showWaiting();

void testLeds(const int delayms);

#ifdef __cplusplus
}
#endif
#endif
