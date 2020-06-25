#ifndef __SWITCHES_H__
#define __SWITCHES_H__
#ifdef __cplusplus
extern "C" {
#endif

#define LED_OFF 0
#define LED_ON 1

//Leds - define hardware pins that correspond to leds
#define LED0    26
#define LED1    24
#define LED2    5
#define LED3    4
#define LED4    11
#define LED5    12

#define RED_LED_BANK 0
#define GREEN_LED_BANK 1

// Switches - define hardware pins that correspond to switches
#define TOGGLE0    8
#define TOGGLE1    9
#define PUSH0    A4
#define PUSH1    A5
#define PUSH2    28
#define PUSH3    30
#define TWIST0    7
#define TWIST1    6


	struct tBoxState {
		bool	sw0; 
		bool	sw1; 
		bool	sw2; 
		bool	sw3; 
		bool	sw4; 
		bool	sw5; 
		bool	sw6; 
		bool	sw7; 
		int	knob0;
		int	knob1;
		bool	led0; 
		bool	led1; 
		bool	led2; 
		bool	led3; 
		bool	led4; 
		bool	led5; 
	};

	bool boxStateChanged();

	bool checkModeChange();

	void diagnosticDisplay();

	void displayBoxState(struct tBoxState state);

	struct tBoxState getBoxState();

	bool getDiagnosticMode();

	int getEncoderValue(int encNumber);

	bool getSwitchState(unsigned int switchPin);

	void initHardware();

	void setControl(char* device, char* value);

	void setDiagnosticMode(bool mode);

	void setLedGroup(unsigned int redOrGreen, bool state);

	void setLedState(int ledPin, bool state);

	void setLedIDXState(int ledIdx, bool state);

	void showErrorLeds(const int delayms);

	void showOKLeds(const int delayms);

	void testLeds(const int delayms);

#ifdef __cplusplus
}
#endif
#endif
