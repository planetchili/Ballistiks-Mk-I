/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	Game.h																				  *
 *	Copyright 2014 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#pragma once

// DIIIIIIIIIIIIICKS!!!!

#include <memory>
#include "D3DGraphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Sound.h"
#include "Randomizer.h"

#include "World.h"
#include "Viewport.h"
#include "Camera.h"

class GoalObserver : public AlertZone::Observer
{
public:
	GoalObserver( DSound& audio )
		:
		whistle( audio.CreateSound( "whistle.wav" ) )
	{}
	virtual void Notify() override
	{
		if( !goalScored )
		{
			goalScored = true;
			whistle.Play();
		}
	}
	bool GoalScored() const
	{
		return goalScored && timeSinceScored >= 2.0f;
	}
	void Reset()
	{
		goalScored = false;
		timeSinceScored = 0.0f;
	}
	void Step( const float dt )
	{
		if( goalScored )
		{
			timeSinceScored += dt;
		}
	}
private:
	float timeSinceScored = 0.0f;
	bool goalScored = false;
	Sound whistle;
};

class Game
{
public:
	Game( HWND hWnd,KeyboardServer& kServer,MouseServer& mServer );
	~Game();
	void Go();
private:
	void ComposeFrame();
	/********************************/
	/*  User Functions              */

	/********************************/
private:
	D3DGraphics gfx;
	KeyboardClient kbd;
	MouseClient mouse;
	DSound audio;
	Randomizer randomizer;
	/********************************/
	/*  User Variables              */
	Sound batman;
	Viewport vp;
	Camera cam;
	std::unique_ptr< World > pWorld;
	GoalObserver obs;
	std::vector< const TriangleStrip > dick;
	bool transitioning = false;
	const float transitionDuration = 1.5f;
	float transitionTime = 0.0f;

	/********************************/
	void UpdateModel();
};