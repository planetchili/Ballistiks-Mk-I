//////////////////////////////////////////////////////////////////
// File: DInput.cpp
// Purpose: Handeling the Basic Direct Input Interface
//////////////////////////////////////////////////////////////////

// Preprocessor directives.
// Includes
#include "DInput.h"


// functions:
// public:
DINPUT::DINPUT(HINSTANCE AppInstance, HWND hWnd, bool& IsWindowActive)
	:
	mDirectInput(nullptr),
	mKeyboard(nullptr),
	mMouse(nullptr),
	mGamepad(nullptr),
	mIsWindowActive(IsWindowActive)
{
	HRESULT result;

	// 1st: create the DirectInput8 interface:
	result = DirectInput8Create(		AppInstance,						// Instance of the programm.
										0x0800,								// DirectInput Version (DirectX9 didn't make changes to DInput, 
																			// so there is no DInput9! (Use the "newest" Version 8.0 instead))
										IID_IDirectInput8,					// Let the programm decide if we use ANSI- or UNICODE version.
										(void**)(&mDirectInput),			// Fillout the Interface.
										NULL);								// Not needed (Something with COM aggregation).

	assert(!FAILED(result));

	
	// 2nd: Enumerate the Input Devices:
	result = mDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL,				// Want to Look for GameController (Xbox360 Controller, Joysticks or Steeringwheel).
											 DeviceCallback,				// The Function that is called for every found Device.
											 NULL,							// No extra information for the Callbackfunction.
											 DIEDFL_ATTACHEDONLY);			// Want to look only for attached devices.
	assert(!FAILED(result));
	
											 
	// 3rd: Create a valid Keyboard Device
	// 3.1.: Create the Keyboard Device with the primary, systemkeyboard.
	result = mDirectInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, NULL);
	assert(!FAILED(result));

	// 3.2.: Set the Dataformat for the Device (how shall DInput Interpret this device?)
	result = mKeyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(!FAILED(result));

	// 3.3.: Set the Cooperationlevel (special instructions for device-access).
	result = mKeyboard->SetCooperativeLevel(hWnd,
											DISCL_NONEXCLUSIVE |		// Other applications may use this device.
											DISCL_BACKGROUND);			// Only get data from the device if the window is focused.
	assert(!FAILED(result));

	// 3.4.: Acquire Access to the Interface
	result = mKeyboard->Acquire();
	assert(!FAILED(result));


	// 4th: Create a valid Mouse Device
	// 4.1.: Create the Mouse Device with the primary, systemmouse.
	result = mDirectInput->CreateDevice(GUID_SysMouse, &mMouse, NULL);
	assert(!FAILED(result));

	// 4.2.: Set the Dataformat for the Device (how shall DInput Interpret this device?)
	result = mMouse->SetDataFormat(&c_dfDIMouse);
	assert(!FAILED(result));

	// 4.3.: Set the Cooperationlevel (special instructions for device-access).
	result = mMouse->SetCooperativeLevel(hWnd,
											DISCL_NONEXCLUSIVE |		// Other applications may use this device.
											DISCL_BACKGROUND);			// Only get data from the device if the window is focused.
	assert(!FAILED(result));


	// 4.4.: Acquire Access to the Interface
	result = mMouse->Acquire();
	assert(!FAILED(result));

}
DINPUT::~DINPUT()
{
	// Release the Interfaces.
	if (mKeyboard)
	{
		mKeyboard->Unacquire();
		mKeyboard->Release();
		mKeyboard = nullptr;
	}
			
	if (mMouse)
	{
		mMouse->Unacquire();
		mMouse->Release();
		mMouse = nullptr;
	}

	if (mGamepad)
	{
		mGamepad->Unacquire();
		mGamepad->Release();
		mGamepad = nullptr;
	}
	
	if (mDirectInput)
	{
		mDirectInput->Release();
		mDirectInput = nullptr;
	}
}

void DINPUT::Update()
{
	HRESULT result;
	// 1st: Get the Information from the keyboard.
	// 1.1: Set every entry of the Arry to NULL.
	for (auto& Entry : mKeys)
	{
		Entry = 0;
	}
	
	
	// 1.2. Get the Data from the Device
	if (mIsWindowActive) // Only get information when the Window is active
	{
		if (mKeyboard)	// Only do that if we have a valid interface
		{
			result = mKeyboard->GetDeviceState(256, mKeys);
			if (result == DIERR_INPUTLOST)	// Couldn't get the Input-Data.
			{
				result = mKeyboard->Acquire();
				if (FAILED(result))
				{
					// Could not get access to the device;
				}
				else
				{
					// We were able to reacquire access to the device, so try to et the Devicestate again.
					result = mKeyboard->GetDeviceState(256, mKeys);
					assert(!FAILED(result));
				}

			}
		}
	}
	

	// 2nd: Get the Information from the mouse.
	// 2.1. Reset the Mouse Data
	mMouseData.lX = 0;
	mMouseData.lY = 0;
	mMouseData.lZ = 0;
	for (auto& Entry : mMouseData.rgbButtons)
	{
		Entry = 0;
	}


	// 2.2 Get the Data from the Device.
	if (mIsWindowActive) // Only get information when the Window is active
	{
		if (mMouse) // Only do that if we have a valid interface
		{
			result = mMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mMouseData);
			if (result == DIERR_INPUTLOST)	// Couldn't get the Input-Data.
			{
				result = mMouse->Acquire();
				if (FAILED(result))
				{
					// Could not get access to the device;
				}
				else
				{
					// We were able to reacquire access to the device, so try to et the Devicestate again.
					result = mKeyboard->GetDeviceState(256, mKeys);
					assert(!FAILED(result));
				}

			}
		}
	}
	


}

// Keyboard Functions:
bool DINPUT::KeyIsPressed(UCHAR keycode)
{
	if (mKeys[keycode] & 0x80)	// Is the 8th Bit set ?(the key is then pressed!)
	{
		int a = 10;
		return true;
	}
	// The Key wasn't pressed!
	return false;
	
}

// Mouse Functions:
bool DINPUT::MouseLeftPressed()
{
	if (mMouseData.rgbButtons[0] & 0x80)
	{
		return true;
	}
	return false;
}
bool DINPUT::MouseRightPressed()
{
	if (mMouseData.rgbButtons[1] & 0x80)
	{
		return true;
	}
	return false;
}
bool DINPUT::MouseMiddlePressed()
{
	if (mMouseData.rgbButtons[2] & 0x80)
	{
		return true;
	}
	return false;
}
Vei2 DINPUT::GetMouseMovement()
{
	return Vei2((int)mMouseData.lX, (int)mMouseData.lY);
}