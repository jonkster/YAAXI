#ifndef __BOX_H__
#define __BOX_H__

#include <LiquidCrystal.h>

#define BOXID "BOXID:Motion Capture-CN001A-00000"
#define BOX_DEFINITIONS "C1USE-CI,C1STB-CI,C2USE-CI,C2STB-CI,N1USE-CI,N1STB-CI,N2USE-CI,N2STB-CI"
#define MSG_BOX_FISH "Avduino Box Fish"

/* Some random MAC Numbers
40:7b:d8:0b:85:16
9c:a1:cb:e2:6b:47
c5:df:d5:4b:82:58
ff:ce:f5:a2:a2:28
*/
#define MAC_ADDRESS 0xC0,0x90,0x33,0x53,0xB8,0xDF
#define IP_ADDRESS 192, 168, 0, 190
#define NETWORK_BROADCAST_ADDRESS 192, 168, 0, 255

// PIN ASSIGNMENTS
#define FF_SWITCH_PUSH 24
#define FF_SWITCH_LO   25

#define POS_BL 2
#define CONTRAST_0 3
#define CONTRAST_1 4

#define POS_LCD_0 39
#define RS_0  41
#define RW_0  42
#define EN_0  43
#define D4_0  44
#define D5_0  45
#define D6_0  46
#define D7_0  47

#define POS_LCD_1 29
#define RS_1  31
#define RW_1  32
#define EN_1  33
#define D4_1  34
#define D5_1  35
#define D6_1  36
#define D7_1  37

// display contrast/brightness
#define CONTRASTV_0 30
#define CONTRASTV_1 80
#define BL 180

#define ACTIVE_CHAR 0x7f
//#define ACTIVE_CHAR 0xff

extern LiquidCrystal lcd0;
extern LiquidCrystal lcd1;

struct tBoxState {
	unsigned long c1Use;
	unsigned long c1Stb;
	unsigned long c2Use;
	unsigned long c2Stb;
	unsigned long n1Use;
	unsigned long n1Stb;
	unsigned long n2Use;
	unsigned long n2Stb;
	int activeDisplay;
};

extern struct tBoxState currentState;
extern struct tBoxState prevState;

void boxSetup(void);
void initialiseBox(void);
void noConnectionActions(void);
void showNetworkInfo(void);
bool sendAnyChanges(tBoxState state, tBoxState prevState);
void setControl(char* device, char* value);
void updateState(void);

#endif
