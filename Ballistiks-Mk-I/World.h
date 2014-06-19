#pragma once
#include <vector>
#include <memory>
#include "PolyClosed.h"
#include "Player.h"
#include "Ball.h"
#include "KeyboardController.h"
#include "D3DGraphics.h"
#include "DragProcessor.h"
#include "AlertZone.h"

class World
{
public:
	World( KeyboardClient& kbd,AlertZoneObserver& obs )
		:
		walls( 
			{ { 80.0f,40.0f },
			{ 1200.0f,40.0f },
			{ 1200.0f,295.0f },
			{ 1250.0f,305.0f },
			{ 1250.0f,415.0f },
			{ 1200.0f,425.0f },
			{ 1200.0f,680.0f },
			{ 80.0f,680.0f },
			{ 80.0f,425.0f },
			{ 30.0f,415.0f },
			{ 30.0f,305.0f },
			{ 80.0f,295.0f } } )
	{
		players.push_back( Player( { 400.0f,300.0f } ) );
		players.push_back( Player( { 400.0f + 2.0f * PLAYER_RADIUS + 2.0f * BALL_RADIUS,300.0f } ) );
		balls.push_back( Ball( { 400.0f + PLAYER_RADIUS + BALL_RADIUS,300.0f } ) );
		alertZones.push_back( 
			AlertZone( PolyClosed { {	{ 80.0f,425.0f },
										{ 30.0f,415.0f },
										{ 30.0f,305.0f },
										{ 80.0f,295.0f } } } ) );		
		alertZones.push_back(
			AlertZone( PolyClosed { {	{ 1200.0f,295.0f },
										{ 1250.0f,305.0f },
										{ 1250.0f,415.0f },
										{ 1200.0f,425.0f } } } ) );
		alertZones[0].AddObserver( obs );
		alertZones[1].AddObserver( obs );

		controller = std::make_unique< KeyboardController >( players[ 0 ],kbd );

		for( PhysicalCircle& c : players )
		{
			circles.push_back( &c );
		}
		for( PhysicalCircle& c : balls )
		{
			circles.push_back( &c );
		}
	}
	void Step( const float dt )
	{
		for( unsigned int x = 0; x < stepsPerFrame; x++ )
		{
			if( stepCount % stepsPerInput == 0 )
			{
				controller->Process();
			}
			for( auto& c : circles )
			{
				dp.ProcessDrag( *c );
				c->Update( dt / (float)stepsPerFrame );
			}
			for( auto i = circles.begin(),end = circles.end(); i != end; i++ )
			{
				for( auto j = std::next( i ); j != end; j++ )
				{
					( *i )->HandleCollision( **j );
				}
				walls.HandleCollision( **i,PolyClosed::ReboundInternal );
			}
			for( AlertZone& z : alertZones )
			{
				for( Ball& b : balls )
				{
					z.HandleCollision( b );
				}
			}
			stepCount++;
		}
	}
	void Render( DrawTarget& tgt ) const
	{
		for( const auto& p : players )
		{
			tgt.Draw( p.GetDrawable() );
		}
		for( const auto& b : balls )
		{
			tgt.Draw( b.GetDrawable() );
		}
		tgt.Draw( walls.GetDrawable( WHITE ) );
	}
private:
	std::vector< PhysicalCircle* > circles;
	std::vector< Player > players;
	std::vector< Ball >	balls;
	std::vector< AlertZone > alertZones;
	PolyClosed walls;
	std::unique_ptr< KeyboardController > controller;
	DragProcessor dp;
	const unsigned int stepsPerFrame = 8;
	const unsigned int stepsPerInput = 8;
	unsigned int stepCount = 0;
};