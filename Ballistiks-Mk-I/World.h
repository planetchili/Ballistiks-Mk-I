#pragma once
#include <vector>
#include <memory>
#include "D3DGraphics.h"
#include "PolyClosed.h"
#include "Player.h"
#include "Ball.h"
#include "KeyboardController.h"
#include "DragProcessor.h"
#include "GoalZone.h"
#include "Walls.h"
#include "Viewport.h"

class World
{
public:
	World( KeyboardClient& kbd,const Viewport& vp,AlertZoneObserver& obs )
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
		players.push_back( Player( 
			{ vp.GetWidth() / 2.0f - vp.GetWidth() / 8.0f,vp.GetHeight() / 2.0f } ) );
		players.push_back( Player(
			{ vp.GetWidth() / 2.0f + vp.GetWidth() / 8.0f,vp.GetHeight() / 2.0f } ) );
		balls.push_back( Ball( { vp.GetWidth() / 2.0f,vp.GetHeight() / 2.0f } ) );
		goalZones.push_back( 
			GoalZone( PolyClosed { {	{ 80.0f,425.0f },
										{ 30.0f,415.0f },
										{ 30.0f,305.0f },
										{ 80.0f,295.0f } } } ) );		
		goalZones.push_back(
			GoalZone( PolyClosed { {	{ 1200.0f,295.0f },
										{ 1250.0f,305.0f },
										{ 1250.0f,415.0f },
										{ 1200.0f,425.0f } } } ) );
		goalZones[0].AddObserver( obs );
		goalZones[1].AddObserver( obs );

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
				walls.HandleCollision( **i );
			}
			for( GoalZone& z : goalZones )
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
		for( const GoalZone& z : goalZones )
		{
			tgt.Draw( z.GetDrawable() );
		}
		for( const auto& p : players )
		{
			tgt.Draw( p.GetDrawable() );
		}
		for( const auto& b : balls )
		{
			tgt.Draw( b.GetDrawable() );
		}
		tgt.Draw( walls.GetDrawable() );
	}
private:
	std::vector< PhysicalCircle* > circles;
	std::vector< Player > players;
	std::vector< Ball >	balls;
	std::vector< GoalZone > goalZones;
	Walls walls;
	std::unique_ptr< KeyboardController > controller;
	DragProcessor dp;
	const unsigned int stepsPerFrame = 8;
	const unsigned int stepsPerInput = 8;
	unsigned int stepCount = 0;
};