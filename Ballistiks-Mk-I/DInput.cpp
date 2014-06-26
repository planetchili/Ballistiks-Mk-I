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

	
	// 2nd: Create a valid GamepadDevice
	// 2.1.: Enumerate through all Gamecontroller devices
	result = mDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL,				// Want to Look for GameController (Xbox360 Controller, Joysticks or Steeringwheel).
											 DeviceCallback,				// The Function that is called for every found Device.
											 NULL,							// No extra information for the Callbackfunction.
											 DIEDFL_ATTACHEDONLY);			// Want to look only for attached devices.
	assert(!FAILED(result));
	
	// 2.2.: Create the GamecontrollerDevice.
	result = mDirectInput->CreateDevice(GamepadGUID, &mGamepad, NULL);
	assert(!FAILED(result));

	// 2.3.: Set the Dataformat for the Device (how shall DInput Interpret this device?)
	result = mGamepad->SetDataFormat(&c_dfDIJoystick);
	assert(!FAILED(result));
	
	// 2.4.: Set the Cooperationlevel (special instructions for device-access).
	result = mGamepad->SetCooperativeLevel(hWnd,
										   DISCL_NONEXCLUSIVE |			// Other applications may use this device.
										   DISCL_BACKGROUND);			// Only get data from the device if the window is focused.

	// 2.5.: Calibrate the Gamepad.
	CalibrateJoystick();

	// 2.6.: Acquire Access to the Interface
	result = mGamepad->Acquire();
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
			result = mMouse->Poll();
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
					result = mMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mMouseData);
					assert(!FAILED(result));
				}

			}
		}
	}

	// 3rd: Get the information from the Gamepad.
	// 3.1.: Reset the Gamepad Data
	mGamepadData.lX = 0;
	mGamepadData.lY = 0;
	mGamepadData.lZ = 0;
	mGamepadData.lRx = 0;
	mGamepadData.lRy = 0;
	mGamepadData.lRz = 0;
	mGamepadData.rglSlider[0] = 0;
	mGamepadData.rglSlider[1] = 0;
	mGamepadData.rgdwPOV[0] = -1;
	mGamepadData.rgdwPOV[1] = -1;
	mGamepadData.rgdwPOV[2] = -1;
	mGamepadData.rgdwPOV[3] = -1;
	for (auto& Entry : mGamepadData.rgbButtons)
	{
		Entry = 0;
	}

	// 3.2 Get the Data from the Device.
	//if (mIsWindowActive) // Only get information when the Window is active
	//{
		if (mGamepad) // Only do that if we have a valid interface
		{
			result = mGamepad->Poll();
			result = mGamepad->GetDeviceState(sizeof(DIJOYSTATE), &mGamepadData);
			if (result == DIERR_INPUTLOST)	// Couldn't get the Input-Data.
			{
				result = mGamepad->Acquire();
				if (FAILED(result))
				{
					// Could not get access to the device;
				}
				else
				{
					// We were able to reacquire access to the device, so try to et the Devicestate again.
					result = mGamepad->GetDeviceState(sizeof(DIJOYSTATE), &mGamepadData);
					assert(!FAILED(result));
				}

			}
		}
	//}


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

// Gamepad Functions:
LONG DINPUT::GetGamePadXAxis()
{
	return mGamepadData.lX;
}
LONG DINPUT::GetGamePadYAxis()
{
	return mGamepadData.lY;
}
bool DINPUT::GamePadButtonIsPressed(UCHAR Number)
{
	if (mGamepadData.rgbButtons[Number] & 0x80)
	{
		return true;
	}
	return false;
}

// private:
void DINPUT::CalibrateJoystick()
{
	if (mGamepad)	// Only do that if we have a valid interface
	{
		HRESULT result;

		DIPROPRANGE Range;				// Scaling of the Axes.
		DIPROPDWORD DeadZone;			// DeadZone of the Axes.
		DIPROPDWORD Saturation;			// Saturation of the Axes.

		// 1st: Axes Scaling:
		Range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		Range.diph.dwSize = sizeof(DIPROPRANGE);
		Range.diph.dwObj = 0;					
		Range.diph.dwHow = DIPH_DEVICE;			// Changes made shall be used for the whole device (therefore dwObj must be 0!)
		Range.lMin = -1000;
		Range.lMax = +1000;
		result = mGamepad->SetProperty(DIPROP_RANGE, &Range.diph);
		assert(!FAILED(result));

		// 2nd: DeadZone.
		DeadZone.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		DeadZone.diph.dwSize = sizeof(DIPROPDWORD);
		DeadZone.diph.dwObj = 0;
		DeadZone.diph.dwHow = DIPH_DEVICE;		// Changes made shall be used for the whole device (therefore dwObj must be 0!)
		DeadZone.dwData = 750;					// 5,000 = 50% DeadZone! (750 = 7.5% DeadZone).
		result = mGamepad->SetProperty(DIPROP_DEADZONE, &DeadZone.diph);
		assert(!FAILED(result));

		// 3rd: Saturation.
		Saturation.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		Saturation.diph.dwSize = sizeof(DIPROPDWORD);
		Saturation.diph.dwObj = 0;
		Saturation.diph.dwHow = DIPH_DEVICE;		// Changes made shall be used for the whole device (therefore dwObj must be 0!)
		Saturation.dwData = 9000;					// 5,000 = 50% Saturation! (9000 = 90% Saturation).
		result = mGamepad->SetProperty(DIPROP_SATURATION, &Saturation.diph);
		assert(!FAILED(result));

	}
}