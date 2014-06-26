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
#include "Vec2.h"


// XBOX 360 Controller
// Left Stick: x,y
// Right Stick: Rx, Ry
// Digital: POV 1
// LT: z+
// RT: z-
// A: 0
// B: 1
// X: 2
// Y: 3
// LB: 4
// RB: 5
// Back: 6
// Start: 7
// L3: 8
// R3: 9

// static global GUID for the gamepad.
static GUID GamepadGUID;

 /* Callbackfunction is called ONCE for every InputDevice, when we enumerate through the devices. */
static BOOL CALLBACK DeviceCallback(LPCDIDEVICEINSTANCE DeviceInstance, void* pRef)
{
	// Use the first gamepad connected to the system.
	GamepadGUID = DeviceInstance->guidInstance;


	// Stop the enumeration
	return DIENUM_STOP;
}

class DINPUT
{
	// functions:
	// public:
public:
	DINPUT(HINSTANCE AppInstance, HWND hWnd, bool& IsWindowActive);
	~DINPUT();

	/* Update the Device States. */
	void Update();

	// Keyboard Functions:
	/* Is a key pressed? */
	bool KeyIsPressed(UCHAR keycode);

	// Mouse Functions:
	/* Is the left Mouse Button pressed? */
	bool MouseLeftPressed();
	/* Is the right Mouse Button pressed? */
	bool MouseRightPressed();
	/* Is the Mousewheel Pressed? */
	bool MouseMiddlePressed();
	/* Get Mousemovement*/
	Vei2 GetMouseMovement();

	// Gamepad Functions:
	LONG GetGamePadXAxis();
	LONG GetGamePadYAxis();
	bool GamePadButtonIsPressed(UCHAR Number);

	// private:
private:
	void CalibrateJoystick();

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
	/* Storage for the Gamepad. */
	DIJOYSTATE mGamepadData;

	// Helper Variables.
	/* Tracks the activeness of the window. */
	bool& mIsWindowActive;

};


///// To-Do-List
// 1st: IF the Window looses focus DInput need to reaquire the Interfaces.
// 2nd: Need to react to "WM_DEVICECHANGE" message, to react to controllers that are plugged in while running.
// 3rd: What if I disconnect a device, DInput got an working interface on?