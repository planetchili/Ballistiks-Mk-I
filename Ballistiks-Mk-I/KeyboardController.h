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
	virtual void Process() override
	{
		if( kbd.KeyIsPressed( VK_LEFT ) )
		{
			int x = 20;
		}
		player.SetThrustVector(
			( kbd.KeyIsPressed( VK_UP )		? Vec2 {  0.0f,-1.0f } : Vec2 { 0.0f,0.0f } ) +
			( kbd.KeyIsPressed( VK_DOWN )	? Vec2 {  0.0f, 1.0f } : Vec2 { 0.0f,0.0f } ) +
			( kbd.KeyIsPressed( VK_LEFT )	? Vec2 { -1.0f, 0.0f } : Vec2 { 0.0f,0.0f } ) +
			( kbd.KeyIsPressed( VK_RIGHT )	? Vec2 {  1.0f, 0.0f } : Vec2 { 0.0f,0.0f } ) );
	}
private:
	KeyboardClient& kbd;
};