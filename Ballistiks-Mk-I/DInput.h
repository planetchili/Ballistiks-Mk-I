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
static BOOL CALLBACK DeviceCallback(LPCDIDEVICEINSTANCE DeviceInstance, void* pRef)
{
	// Not used at the moment (will use it later for gamepad devices!).

	// return to the next device of the enumeration.
	return DIENUM_CONTINUE;
}

class DINPUT
{
	// functions:
	// public:
public:
	DINPUT(HINSTANCE AppInstance, HWND hWnd, bool& IsWindowActive);
	~DINPUT();

	/* Get the Information from the devices. */
	void Update();

	bool KeyIsPressed(UCHAR keycode);

	// private:
private:
	

	// member:
	// private:
private:
	/* the main DirectInput8 interface. */
	LPDIRECTINPUT8 mDirectInput;
	// The Supported InputDevices
	LPDIRECTINPUTDEVICE8 mKeyboard;
	LPDIRECTINPUTDEVICE8 mMouse;
	LPDIRECTINPUTDEVICE8 mGamepad;

	// Datastorage for the Input Devices.
	/* Storage for the Keyboard. */
	BYTE mKeys[256];
	/* Storage for the mouse. */
	DIMOUSESTATE mMouseData;

	// Helper Variables.
	/* Tracks the activeness of the window. */
	bool& mIsWindowActive;

};


///// To-Do-List
// 1st: IF the Window looses focus DInput need to reaquire the Interfaces.
// 2nd: Need to react to "WM_DEVICECHANGE" message, to react to controllers that are plugged in while running.
// 3rd: What if I disconnect a device, DInput got an working interface on?