//////////////////////////////////////////////////////////////////
// File: DInput.h
// Purpose: Handeling the Basic Direct Input Interface
//////////////////////////////////////////////////////////////////

// preprocessor directives:
#pragma once

// including the DirectInput libraries:
#pragma comment(lib, "DInput8.lib")
#pragma comment(lib, "DXGUID.lib")

// includes:
#include <dinput.h>
#include <cassert>

/* Callbackfunction is called ONCE for every InputDevice, when we enumerate through the devices. */
BOOL CALLBACK DeviceCallback(LPCDIDEVICEINSTANCE DeviceInstance, void* pRef)
{
	// Not used at the moment (will use it later for gamepad devices!).

	// return to the next device of the enumeration.
	return DIENUM_CONTINUE;
}

class DInput
{
	// functions:
	// public:
public:
	DInput( HINSTANCE AppInstance, HWND hWnd );
	~DInput();

	// member:
	// private:
private:
	/* the main DirectInput8 interface. */
	LPDIRECTINPUT8 pDirectInput;
	// The Supported InputDevices
	LPDIRECTINPUTDEVICE8 pKeyboard;
	LPDIRECTINPUTDEVICE8 pMouse;
	LPDIRECTINPUTDEVICE8 pGamepad;

};


///// To-Do-List
// 1st: Get the Application Interface from the Windows function.
// 2nd: IF the Window looses focus DInput need to reaquire the Interfaces.
// 3rd: Get the HWND from the Windows function.