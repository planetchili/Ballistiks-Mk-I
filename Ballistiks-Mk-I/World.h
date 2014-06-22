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
#include "GoalCrease.h"
#include "Walls.h"
#include "Viewport.h"
#include "ViewableWorld.h"
#include "AIFactoryCodex.h"

class GoalObs : public AlertZone::Observer
{
public:
	GoalObs( std::vector< std::unique_ptr< Controller > >& controllers )
		:
		controllers( controllers )
	{}
	virtual void Notify() override
	{
		for( std::unique_ptr< Controller >& c : controllers )
		{
			c->Disable();
		}
	}
private:
	std::vector< std::unique_ptr< Controller > >& controllers;
};

class World : public ViewableWorld
{
public:
	World( KeyboardClient& kbd,const Viewport& vp,AlertZone::Observer& obs )
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
		wobs( controllers )
	{
		players.push_back( Player( 
			{ vp.GetWidth() / 2.0f - vp.GetWidth() / 8.0f,vp.GetHeight() / 2.0f },0 ) );
		players.push_back( Player(
			{ vp.GetWidth() / 2.0f + vp.GetWidth() / 8.0f,vp.GetHeight() / 2.0f },1 ) );
		balls.push_back( Ball( { 
			vp.GetWidth()  / 2.0f + float( rand() % 11 - 5 ),
			vp.GetHeight() / 2.0f + float( rand() % 11 - 5 )
		} ) );
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
		goalZones[0].AddObserver( wobs );
		goalZones[1].AddObserver( wobs );

		creases.push_back( GoalCrease( true,{ 79.0f,360.0f },100.0f ) );
		creases.push_back( GoalCrease( false,{ 1200.0f,360.0f },100.0f ) );

		controllers.push_back( codex.GetRandomFactory().Make( players[0],*this ) );
		controllers.push_back( codex.GetRandomFactory().Make( players[1],*this ) );

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
				for( std::unique_ptr< Controller >& c : controllers )
				{
					c->Process();
				}
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
			for( GoalCrease& c : creases )
			{
				for( Player& p : players )
				{
					c.HandleCollision( p );
				}
			}
			stepCount++;
		}
	}
	void Render( DrawTarget& tgt ) const
	{
		for( const GoalCrease& c : creases )
		{
			tgt.Draw( c.GetDrawable() );
		}
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
public: // viewer interface
	inline virtual const std::vector< Player >& GetPlayers() const override
	{
		return players;
	}
	inline virtual const std::vector< Ball >& GetBalls() const override
	{
		return balls;
	}
private:
	std::vector< PhysicalCircle* > circles;
	std::vector< Player > players;
	std::vector< Ball >	balls;
	std::vector< GoalZone > goalZones;
	std::vector< GoalCrease > creases;
	Walls walls;
	std::vector< std::unique_ptr< Controller > > controllers;
	AIFactoryCodex codex;
	DragProcessor dp;
	const unsigned int stepsPerFrame = 8;
	const unsigned int stepsPerInput = 8;
	unsigned int stepCount = 0;
	GoalObs wobs;
};