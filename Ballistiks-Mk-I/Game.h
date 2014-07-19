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
#include <iostream>
#include <fstream>
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
#include "Presentator.h"
#include "TournamentManager.h"
#include "Timer.h"


class Game
{
public:
	class EndGameObserver : public Observer
	{
	public:
		EndGameObserver( Game& parent )
			:
			parent( parent )
		{}
	protected:
		void OnNotify() override
		{
			if( parent.simulation )
			{
				parent.tManager->RecordGameResults();
				if( !parent.tManager->StartNextGame() )
				{
					parent.tManager->RecordMatchResults();
					if( !parent.tManager->StartNextMatch() )
					{
						parent.tManager->RecordTotalResults();
						parent.Exit();
					}
					else
					{
						parent.tManager->StartNextGame();
					}
				}
			}
		}
	private:
		Game& parent;
	};
public:
	Game( HWND hWnd,const std::wstring cmdStr,KeyboardServer& kServer,MouseServer& mServer );
	~Game();
	void Go();
private:
	void ComposeFrame();
	void Exit();
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
	Viewport vp;
	Camera cam;
	RectF clockRect;
	std::vector< const TriangleStrip > clockBack;
	Font scoreFont;
	Font nameFont;
	Font timeFont;
	Font progressFont;
	Timer frameTimer;
	AIFactoryCodex codex;
	Presentator pres;
	std::shared_ptr< EndGameObserver > endObs;
	bool simulation = true;
	std::unique_ptr< TournamentManager > tManager;
	std::unique_ptr< KeyboardControllerFactory > kbdFactory;

	/********************************/
	void UpdateModel();
};