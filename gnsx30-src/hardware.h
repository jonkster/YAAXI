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
#define LED0    17
#define LED1    16

// Switches - define hardware pins that correspond to switches
#define SW00 37 // CDI
#define SW01 36 // OBS
#define SW02 35 // MSG
#define SW03 34 // FPL
#define SW04 33 // PROC
#define SW05 32 // CRSR
#define SW06 31 // ENTER
#define SW07 30 // CLR
#define SW08 29 // MENU
#define SW09 28 // DTO
#define SW10 27 // RNG IN
#define SW11 26 // RNG OUT

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
