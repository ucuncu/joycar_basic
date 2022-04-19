#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "HID.h"

#if defined(_USING_HID)

typedef struct
{
	uint32_t buttons;

} gamepad_report_struct;

class Gamepad
{
private:
	gamepad_report_struct reportData;
public:
	Gamepad(bool useZRx = false);
	
	void sendUpdate();
	
	void setButtonState(uint8_t button, bool state);
	
};

#else
	#warning "HID is not supported for this chip"
#endif
#endif
