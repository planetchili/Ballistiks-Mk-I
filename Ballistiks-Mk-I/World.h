#pragma once
#include <vector>
#include <memory>
#include "PolyClosed.h"
#include "Player.h"
#include "Ball.h"
#include "KeyboardController.h"
#include "D3DGraphics.h"
#include "DragProcessor.h"

class World
{
public:
	World( KeyboardClient& kbd )
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
			{ 80.0f,295.0f } } ),
		players( 
			Vec2{ 400.0f,300.0f },
			Vec2{ 400.0f + 2.0f * PLAYER_RADIUS + 2.0f * BALL_RADIUS,300.0f } ),
		balls( { 400.0f + PLAYER_RADIUS + BALL_RADIUS,300.0f } ),
		controller( players[ 0 ],kbd )
	{
		for( PhysicalCircle& c : players )
		{
			circles.push_back( std::shared_ptr< PhysicalCircle >( &c ) );
		}
		for( PhysicalCircle& c : balls )
		{
			circles.push_back( std::shared_ptr< PhysicalCircle >( &c ) );
		}
	}
	void Step( const float dt )
	{
		controller.Process();
		for( auto& c : circles )
		{
			dp.ProcessDrag( *c );
			c->Update( dt );
		}
		for( auto i = circles.begin(),end = circles.end(); i != end; i++ )
		{
			for( auto j = std::next( i ); j != end; j++ )
			{
				( *i )->HandleCollision( **j );
			}
			walls.HandleCollision( **i,PolyClosed::ReboundInternal );
		}
	}
	void Render( D3DGraphics& gfx ) const
	{
		for( const auto& p : players )
		{
			p.GetDrawable().Rasterize( gfx );
		}
		for( const auto& b : balls )
		{
			b.GetDrawable().Rasterize( gfx );
		}
		walls.GetDrawable( WHITE ).Rasterize( gfx );
	}
private:
	std::vector< Player > players;
	std::vector< Ball >	balls;
	std::vector< std::shared_ptr< PhysicalCircle > > circles;
	PolyClosed walls;
	KeyboardController controller;
	DragProcessor dp;
};