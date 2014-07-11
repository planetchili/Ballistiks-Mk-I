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
#include "Midi.h"
#include "Randomizer.h"
#include "World.h"
#include "GameManager.h"
#include "Viewport.h"
#include "Camera.h"


class Game
{
private:
	class GoalObserver : public Observer
	{
	public:
		virtual void OnNotify() override
		{
			if( !goalScored )
			{
				goalScored = true;
				theme.Stop();
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
		static std::shared_ptr< GoalObserver > Make( DSound& audio,MidiSong& theme )
		{
			return std::shared_ptr< GoalObserver >( new GoalObserver( audio,theme ) );
		}
	private:
		GoalObserver( DSound& audio,MidiSong& theme )
			:
			whistle( audio.CreateSound( "whistle.wav" ) ),
			theme( theme )
		{}
	private:
		float timeSinceScored = 0.0f;
		bool goalScored = false;
		Sound whistle;
		MidiSong& theme;
	};
	class PeriodObserver : public Observer
	{
	public:
		virtual void OnNotify() override
		{
			if( !periodEnded )
			{
				periodEnded = true;
				theme.Stop();
				whistle.Play();
			}
		}
		bool PeriodEnded() const
		{
			return periodEnded && timeSincePeriodEnd >= 2.0f;
		}
		void Reset()
		{
			periodEnded = false;
			timeSincePeriodEnd = 0.0f;
		}
		void Step( const float dt )
		{
			if( periodEnded )
			{
				timeSincePeriodEnd += dt;
			}
		}
		static std::shared_ptr< PeriodObserver > Make( DSound& audio,MidiSong& theme )
		{
			return std::shared_ptr< PeriodObserver >( new PeriodObserver( audio,theme ) );
		}
	private:
		PeriodObserver( DSound& audio,MidiSong& theme )
			:
			whistle( audio.CreateSound( "whistle.wav" ) ),
			theme( theme )
		{}
	private:
		float timeSincePeriodEnd = 0.0f;
		bool periodEnded = false;
		Sound whistle;
		MidiSong& theme;
	};

public:
	Game( HWND hWnd,KeyboardServer& kServer,MouseServer& mServer );
	~Game();
	void Go();
private:
	void ComposeFrame();
	/********************************/
	/*  User Functions              */

	void MakeAIVsAI()
	{
		gameManager = std::make_unique< GameManager >( codex.GetRandomFactory(),codex.GetRandomFactory() );
		gameManager->AddTeamObservers( pointObs,pointObs );
		gameManager->AddPeriodObserver( periodObs );
	}
	void MakeAIVsHuman()
	{
		gameManager = std::make_unique< GameManager >( kbdFactory,codex.GetRandomFactory() );
		gameManager->AddTeamObservers( pointObs,pointObs );
		gameManager->AddPeriodObserver( periodObs );
	}

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
	MidiSong batmanTheme;
	Viewport vp;
	Camera cam;
	std::shared_ptr< GoalObserver > pointObs;
	std::shared_ptr< PeriodObserver > periodObs;
	std::vector< const TriangleStrip > dick;
	RectF clockRect;
	std::vector< const TriangleStrip > clockBack;
	bool transitioningPoint = false;
	bool transitioningPeriod = false;
	const float transitionDuration = 1.5f;
	float transitionTime = 0.0f;
	Font scoreFont;
	Font nameFont;
	Font timeFont;

	AIFactoryCodex codex;
	KeyboardControllerFactory kbdFactory;
	std::unique_ptr< GameManager > gameManager;

	/********************************/
	void UpdateModel();
};