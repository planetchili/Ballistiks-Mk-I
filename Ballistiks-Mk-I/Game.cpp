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
	bounds( { { 50.0f,50.0f },{ 750.0f,50.0f },{ 750.0f,550.0f },{50.0f,550.0f} } ),
	player( {400.0f,300.0f} ),
	controller( player,kbd ),
	ball( {450.0f,300.0} )
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
	controller.Process();
	dp.ProcessDrag( player );
	dp.ProcessDrag( ball );
	player.Update( 1.0f / 60.0f );
	ball.Update( 1.0f / 60.0f );
	player.HandleCollision( ball );
	bounds.HandleCollision( player,PolyClosed::ReboundInternal );
	bounds.HandleCollision( ball,PolyClosed::ReboundInternal );
}

void Game::ComposeFrame()
{
	gfx.DrawCircle( player.GetCenter(),(int)player.GetRadius(),WHITE );
	gfx.DrawCircle( ball.GetCenter(),(int)ball.GetRadius(),GREEN );
	bounds.GetDrawable( WHITE ).Rasterize( gfx );
}
