//////////////////////////////////////////////////////////////////
// File: DInput.cpp
// Purpose: Handeling the Basic Direct Input Interface
//////////////////////////////////////////////////////////////////

// Preprocessor directives.
// Includes
#include "DInput.h"

DInput::DInput( HINSTANCE AppInstance, HWND hWnd )
	:
	pDirectInput(nullptr),
	pKeyboard(nullptr),
	pMouse(nullptr),
	pGamepad(nullptr)
{
	// 1st: create the DirectInput8 interface:
	assert( !FAILED(DirectInput8Create(	AppInstance,						// Instance of the programm.
										0x0800,								// DirectInput Version (DirectX9 didn't make changes to DInput, 
																			// so there is no DInput9! (Use the "newest" Version 8.0 instead))
										IID_IDirectInput8,					// Let the programm decide if we use ANSI- or UNICODE version.
										(void**)(pDirectInput),				// Fillout the Interface.
										NULL)));							// Not needed (Something with COM aggregation).

	// 2nd: Enumerate the Input Devices:
	assert(!FAILED(pDirectInput->EnumDevices(DI8DEVCLASS_KEYBOARD |			// Want to Look for Keyboard Devices.
											 DI8DEVCLASS_POINTER |			// Want to Look for Pointer Devices (Mouse or Graphicstablett).
											 DI8DEVCLASS_GAMECTRL,			// Want to Look for GameController (Xbox360 Controller, Joysticks or Steeringwheel).
											 DeviceCallback,				// The Function that is called for every found Device.
											 NULL,							// No extra information for the Callbackfunction.
											 DIEDFL_ATTACHEDONLY)));		// Want to look only for attached devices.

	// 3rd: Create a valid Keyboard Device
	// 3.1.: Create the Keyboard Device with the primary, systemkeyboard.
	assert(!FAILED(pDirectInput->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL)));

	// 3.2.: Set the Dataformat for the Device (how shall DInput Interpret this device?)
	assert(!FAILED(pKeyboard->SetDataFormat(&c_dfDIKeyboard)));

	// 3.3.: Set the Cooperationlevel (special instructions for device-access).
	assert(!FAILED(pKeyboard->SetCooperativeLevel(hWnd, 
												  DISCL_NONEXCLUSIVE |		// Other applications may use this device.
												  DISCL_FOREGROUND)));		// Only get data from the device if the window is focused.

	// 4th: Create a valid Mouse Device
	// 4.1.: Create the Mouse Device with the primary, systemmouse.
	assert(!FAILED(pDirectInput->CreateDevice(GUID_SysMouse, &pMouse, NULL)));

	// 4.2.: Set the Dataformat for the Device (how shall DInput Interpret this device?)
	assert(!FAILED(pMouse->SetDataFormat(&c_dfDIMouse)));

	// 4.3.: Set the Cooperationlevel (special instructions for device-access).
	assert(!FAILED(pKeyboard->SetCooperativeLevel(hWnd,
												  DISCL_NONEXCLUSIVE |		// Other applications may use this device.
												  DISCL_FOREGROUND)));		// Only get data from the device if the window is focused.

}
DInput::~DInput()
{
	// Release the Interfaces.
	pKeyboard->Unacquire();
	pKeyboard->Release();
	pKeyboard = nullptr;

	pMouse->Unacquire();
	pMouse->Release();
	pMouse = nullptr;

	pGamepad->Unacquire();
	pGamepad->Release();
	pGamepad = nullptr;

	pDirectInput->Release();
	pDirectInput = nullptr;
}