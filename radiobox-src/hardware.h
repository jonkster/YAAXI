#ifndef __SWITCHES_H__
#define __SWITCHES_H__
#ifdef __cplusplus
extern "C" {
#endif


#define LED_OFF 0
#define LED_ON 1

#define RED_LED_BANK 0
#define GREEN_LED_BANK 1

//Leds - define hardware pins that correspond to leds
#define LED0    31
#define LED1    32

// Switches - define hardware pins that correspond to switches
#define SW00 22 // rad1 <--> rad2
#define SW01 16 // stdby <--> activ
#define SW02 17 // c <--> v <--> a

#define COM1_M 0
#define COM2_M 1
#define NAV1_M 2
#define NAV2_M 3
#define ADF1_M 4


	extern int activeDisplay;
	extern int standbyDisplay;

	struct tBoxState {
		bool	sw00; 
		bool	sw01; 
		bool	sw02; 
		bool	sw03; 
		bool	sw04; 
		bool	sw05; 
		bool	sw06; 
		bool	sw07; 
		bool	sw08; 
		bool	sw09; 
		bool	sw10; 
		bool	sw11; 
		int	knob00;
		int	knob01;
	};

	bool boxStateChanged();

	bool checkModeChange();

	void diagnosticDisplay();

	void displayBoxState(struct tBoxState state);

	struct tBoxState getBoxState();

	bool getDiagnosticMode();

	int getEncoderValue(int encNumber);

	int getRadioMode();

	bool getSwitchState(unsigned int switchPin);

	void incMode();

	void initHardware();

	void setControl(char* device, char* value);

	void setDiagnosticMode(bool mode);

	void setLedGroup(unsigned int redOrGreen, bool state);

	void setLedState(int ledPin, bool state);

	void setLedIDXState(int ledIdx, bool state);

	void showErrorLeds(const int delayms);

	void showNoConnection(const int delayms);

	void showOKLeds(const int delayms);

	void testLeds(const int delayms);

#ifdef __cplusplus
}
#endif
#endif
