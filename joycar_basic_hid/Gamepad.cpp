#include "Gamepad.h"

#if defined(_USING_HID)

Gamepad::Gamepad(bool useZRx)
{
	static const uint8_t ReportDescriptor[] PROGMEM = {
		0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
		0x09, 0x05,                    // USAGE (Game Pad)
		0xa1, 0x01,                    // COLLECTION (Application)
		0xa1, 0x00,                    //   COLLECTION (Physical)
		0x85, 0x03,                	   //     REPORT_ID (3)
		0x05, 0x09,                    //     USAGE_PAGE (Button)
		0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
		0x29, 0x13,                    //     USAGE_MAXIMUM (Button 19)
		0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
		0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
		0x75, 0x01,                    //     REPORT_SIZE (1)
		0x95, 0x20,                    //     REPORT_COUNT (32)
		0x81, 0x02,                    //     INPUT (Data,Var,Abs)
		0xc0,                          //     END_COLLECTION
		0xc0                           // END_COLLECTION
	};
	static HIDSubDescriptor gamePadDescriptor(ReportDescriptor, sizeof(ReportDescriptor));
	HID().AppendDescriptor(&gamePadDescriptor);
	
	reportData.buttons 		= 0;
}

void Gamepad::setButtonState(uint8_t button, bool state)
{
	if(state == true) bitSet(reportData.buttons, button);
	else bitClear(reportData.buttons, button);
	sendUpdate();
}

void Gamepad::sendUpdate()
{
	HID().SendReport(0x03, &reportData, sizeof(gamepad_report_struct));
}
#endif
