#pragma once

#include <dinput.h>

#include "Controller.h"
#include "Keyboard.h"
#include "DInput.h"

class KeyboardController : public Controller
{
public:
	KeyboardController( ControllablePlayer& player,KeyboardClient& kbd, DINPUT& Input )
		:
		Controller( player ),
		kbd( kbd ),
		Input( Input )
	{}
protected:
	virtual void _Process() override
	{
		/*
		player.SetThrustVector(
			( kbd.KeyIsPressed( VK_UP )		? Vec2 {  0.0f,-1.0f } : Vec2 { 0.0f,0.0f } ) +
			( kbd.KeyIsPressed( VK_DOWN )	? Vec2 {  0.0f, 1.0f } : Vec2 { 0.0f,0.0f } ) +
			( kbd.KeyIsPressed( VK_LEFT )	? Vec2 { -1.0f, 0.0f } : Vec2 { 0.0f,0.0f } ) +
			( kbd.KeyIsPressed( VK_RIGHT )	? Vec2 {  1.0f, 0.0f } : Vec2 { 0.0f,0.0f } ) );
		*/
		Input.Update();
		/*
		player.SetThrustVector(
			(Input.KeyIsPressed(DIK_UPARROW) ? Vec2{ 0.0f, -1.0f } : Vec2{ 0.0f, 0.0f }) +
			(Input.KeyIsPressed(DIK_DOWNARROW) ? Vec2{ 0.0f, 1.0f } : Vec2{ 0.0f, 0.0f }) +
			(Input.KeyIsPressed(DIK_LEFTARROW) ? Vec2{ -1.0f, 0.0f } : Vec2{ 0.0f, 0.0f }) +
			(Input.KeyIsPressed(DIK_RIGHTARROW) ? Vec2{ 1.0f, 0.0f } : Vec2{ 0.0f, 0.0f }));
		*/

		player.SetThrustVector(Vec2{ ((float)Input.GetGamePadXAxis()) / 500.0f, ((float)Input.GetGamePadYAxis()) / 500.0f });

		/*
		while( !kbd.KeyEmpty() )
		{
			const KeyEvent k = kbd.ReadKey();
			if( k.GetCode() == VK_SPACE && k.IsPress() )
			{
				player.Burst();
			}
		}*/
		/*
		if (Input.KeyIsPressed(DIK_SPACE))
		{
			player.Burst();
		}*/
		if (Input.GamePadButtonIsPressed(0))
		{
			player.Burst();
		}



	}
private:
	KeyboardClient& kbd;
	DINPUT& Input;
};