#ifndef __SWITCHES_H__
#define __SWITCHES_H__

#include "SPI.h"
#include "TFT_22_ILI9225.h"
#include "./fonts/FreeMono12pt7b.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED_OFF 0
#define LED_ON 1

#define RED_LED_BANK 0
#define GREEN_LED_BANK 1

#define TFT_RST A4 
#define TFT_RS  A3 
#define TFT_CS  A5   // SS
#define TFT_SDI A2   // MOSI
#define TFT_CLK A1   // SCK
#define TFT_LED A0   //

#define DIGIT_SELECT_0   22
#define DIGIT_SELECT_1   23
#define DIGIT_SELECT_2   24
#define DIGIT_SELECT_3   25
#define DIGIT_SELECT_4   26

#define BCD8	28
#define BCD4	30
#define BCD2	32
#define BCD1	34

// Switches - define hardware pins that correspond to switches
#define AP_DN	27
#define AP_UP	29
#define AP_ON	31
#define AP_HDG	33
#define AP_NAV	35
#define AP_APR	37
#define AP_VS	39
#define AP_ALT	41
#define AP_FD	45


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
