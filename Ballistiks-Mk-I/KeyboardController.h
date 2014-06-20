#pragma once
#include "Controller.h"
#include "Keyboard.h"

class KeyboardController : public Controller
{
public:
	KeyboardController( ControllablePlayer& player,KeyboardClient& kbd )
		:
		Controller( player ),
		kbd( kbd )
	{}
protected:
	virtual void _Process() override
	{
		player.SetThrustVector(
			( kbd.KeyIsPressed( VK_UP )		? Vec2 {  0.0f,-1.0f } : Vec2 { 0.0f,0.0f } ) +
			( kbd.KeyIsPressed( VK_DOWN )	? Vec2 {  0.0f, 1.0f } : Vec2 { 0.0f,0.0f } ) +
			( kbd.KeyIsPressed( VK_LEFT )	? Vec2 { -1.0f, 0.0f } : Vec2 { 0.0f,0.0f } ) +
			( kbd.KeyIsPressed( VK_RIGHT )	? Vec2 {  1.0f, 0.0f } : Vec2 { 0.0f,0.0f } ) );

		while( !kbd.KeyEmpty() )
		{
			const KeyEvent k = kbd.ReadKey();
			if( k.GetCode() == VK_SPACE && k.IsPress() )
			{
				player.Burst();
			}
		}
	}
private:
	KeyboardClient& kbd;
};