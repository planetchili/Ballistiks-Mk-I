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
#include <sstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include "CommandLine.h"

Game::Game( HWND hWnd,const std::wstring cmdStr,KeyboardServer& kServer,MouseServer& mServer )
	:
	gfx( hWnd ),
	audio( hWnd ),
	kbd( kServer ),
	mouse( mServer ),
	vp( gfx,gfx.GetScreenRect() ),
	cam( vp,vp.GetWidth(),vp.GetHeight(),{ vp.GetWidth() / 2.0f,vp.GetHeight() / 2.0f } ),
	scoreFont(L"Verdana",25.0f),
	nameFont(L"Arial",12.0f,false),
	timeFont(L"Tahoma",25.0f),
	clockRect( {-4.0f,35.0f,vp.GetWidth() / 2.0f - 80.0f,vp.GetWidth() / 2.0f + 80.0f } ),
	clockBack( TriangleStrip::ExtractSolidStripCollection( clockRect.ExtractVertices() ) ),
	pres(audio,cam,vp),
	endObs(std::make_shared< EndGameObserver >(*this)),
	progressFont(L"Times New Roman",60.0f )
{
	pres.AddEndGameObserver( endObs );

	CommandLine cmd( cmdStr );
	switch( cmd.type )
	{
	case CommandLine::Simulate:
		simulation = true;
		tManager = std::make_unique<TournamentManager>( codex,pres,cmd.seed );
		tManager->StartNextMatch();
		tManager->StartNextGame();
		break;
	case CommandLine::Watch:
		simulation = false;
		srand( cmd.seed );
		pres.StartGame(
			codex.GetFactoryByName( cmd.player0 ),
			codex.GetFactoryByName( cmd.player1 ) );
		frameskip = cmd.frameskip;
		break;
	case CommandLine::Play:
		simulation = false;
		kbdFactory = std::make_unique< KeyboardControllerFactory >( kbd );
		pres.StartGame(	*kbdFactory,codex.GetFactoryByName( cmd.player0 ) );
		frameskip = cmd.frameskip;
		break;
	}
	frameTimer.StartWatch();
}

void Game::Exit()
{
	PostQuitMessage( 0 );
}

Game::~Game()
{
}

void Game::Go()
{
	UpdateModel();
	if( !simulation )
	{
		gfx.BeginFrame();
		ComposeFrame();
		gfx.EndFrame();
	}
	else if( frameTimer.GetTimeSec() > ( 5.0f / 60.0f ) )
	{
		gfx.BeginFrame();
		ComposeFrame();
		gfx.EndFrame();
		frameTimer.StartWatch();
	}
}

void Game::UpdateModel( )
{
	for( unsigned int n = 0; n < frameskip + 1; n++ )
	{
		pres.Step( 1.0f / 60.0f );
	}
}

void Game::ComposeFrame()
{
	if( !simulation )
	{
		pres.Draw( vp );

		const GameManager& gameManager = pres.GetManager();
		const Team& leftTeam = gameManager.GetPeriod() % 2 == 0 ? gameManager.GetTeamA()
			: gameManager.GetTeamB();
		const Team& rightTeam = gameManager.GetPeriod() % 2 == 0 ? gameManager.GetTeamB()
			: gameManager.GetTeamA();

		gfx.DrawString( leftTeam.GetName(),
			RectF {
			0.0f,
			40.0f,
			0.0f,
			280.0f },nameFont,leftTeam.GetPrimaryColor(),Surface::Alignment::Left );
		gfx.DrawString( std::to_wstring( leftTeam.GetScore() ),
			RectF {
			15.0f,
			65.0f,
			0.0f,
			60.0f },scoreFont,GREEN );
		gfx.DrawString( rightTeam.GetName(),
			RectF {
			0.0f,
			40.0f,
			1000.0f,
			1276.0f },nameFont,rightTeam.GetPrimaryColor(),Surface::Alignment::Right );
		gfx.DrawString( std::to_wstring( rightTeam.GetScore() ),
			RectF {
			15.0f,
			65.0f,
			1220.0f,
			1280.0f },scoreFont,GREEN );

		vp.Draw( clockBack.front().GetDrawable( Color( 96,80,140 ) ) );
		std::wstringstream s;
		s << L'P' << gameManager.GetPeriod() + 1 << L' '
			<< int( gameManager.GetTimeRemaining() ) / 60 << L":"
			<< std::setfill( L'0' ) << std::setw( 2 ) << int( gameManager.GetTimeRemaining() ) % 60;
		gfx.DrawString( s.str(),clockRect,timeFont,GREEN );
	}
	else
	{
		std::wstringstream s;
		s << std::setprecision( 0 ) << std::fixed << tManager->GetCompletionPercent() << L"%";
		gfx.DrawString(
			s.str(),
			RectF {
			vp.GetHeight() / 2.0f - 60.0f,
			vp.GetHeight() / 2.0f + 60.0f,
			0.0f,vp.GetWidth() },progressFont );
	}
}
