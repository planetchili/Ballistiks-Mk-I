/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	Game.cpp																			  *
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
#include "Game.h"

Game::Game( HWND hWnd,KeyboardServer& kServer,MouseServer& mServer )
:	gfx( hWnd ),
	//audio( hWnd ),
	kbd( kServer ),
	mouse( mServer ),
	bounds( { { 40.0f,40.0f },{ 1240.0f,40.0f },{ 1240.0f,680.0f },{ 40.0f,680.0f } } ),
	player( {400.0f,300.0f} ),
	controller( player,kbd ),
	opponent( { 400.0f + 2.0f * PLAYER_RADIUS + 2.0f * BALL_RADIUS,300.0f } ),
	ball( {400.0f + PLAYER_RADIUS + BALL_RADIUS,300.0f} )
{
}

Game::~Game()
{
}

void Game::Go()
{
	UpdateModel();
	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel( )
{
	const float dt = 1.0f / 60.0f;
	controller.Process();
	dp.ProcessDrag( player );
	dp.ProcessDrag( ball );
	dp.ProcessDrag( opponent );
	player.Update( dt );
	opponent.Update( dt );
	ball.Update( dt );
	player.HandleCollision( opponent );
	player.HandleCollision( ball );
	opponent.HandleCollision( ball );
	bounds.HandleCollision( player,PolyClosed::ReboundInternal );
	bounds.HandleCollision( opponent,PolyClosed::ReboundInternal );
	bounds.HandleCollision( ball,PolyClosed::ReboundInternal );
}

void Game::ComposeFrame()
{
	gfx.DrawCircle( player.GetCenter(),(int)player.GetRadius(),WHITE );
	gfx.DrawCircle( opponent.GetCenter(),(int)opponent.GetRadius(),BLUE );
	gfx.DrawCircle( ball.GetCenter(),(int)ball.GetRadius(),GREEN );
	bounds.GetDrawable( WHITE ).Rasterize( gfx );
}
