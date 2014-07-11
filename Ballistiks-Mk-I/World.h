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
#include "Team.h"

class World : public ViewableWorld
{
private:
	class HaltPlayObs : public Observer
	{
	public:
		virtual void OnNotify() override
		{
			for( std::unique_ptr< Controller >& c : parent.controllers )
			{
				c->Disable();
			}
			for( GoalZone& g : parent.goalZones )
			{
				g.Disable();
			}
			parent.StopClock();
		}
		static std::shared_ptr< HaltPlayObs > Make( World& parent )
		{
			return std::shared_ptr< HaltPlayObs >( new HaltPlayObs( parent ) );
		}
	private:
		HaltPlayObs( World& parent )
			:
			parent( parent )
		{}
	private:
		World& parent;
	};
public:
	World( float startTime,Team& leftTeam,Team& rightTeam,Observable& periodEvent )
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
		haltObserver( HaltPlayObs::Make( *this ) ),
		timeElapsed( startTime )
	{
		players.push_back( Player(
			{ 1280.0f / 2.0f - 1280.0f / 8.0f,720.0f / 2.0f },1,
			leftTeam.GetPrimaryColor(),leftTeam.GetSecondaryColor() ) );
		players.push_back( Player(
			{ 1280.0f / 2.0f + 1280.0f / 8.0f,720.0f / 2.0f },0,
			rightTeam.GetPrimaryColor(),rightTeam.GetSecondaryColor() ) );
		balls.push_back( Ball( { 
			1280.0f / 2.0f + RandError( 5.0f ),
			720.0f / 2.0f + RandError( 5.0f )
		} ) );
		goalZones.push_back( 
			GoalZone( PolyClosed { {	{ 77.0f,424.8f },
										{ 30.0f,415.0f },
										{ 30.0f,305.0f },
										{ 77.0f,295.2f } } },leftTeam.GetPrimaryColor() ) );
		goalZones.push_back(
			GoalZone( PolyClosed { {	{ 1203.0f,295.2f },
										{ 1250.0f,305.0f },
										{ 1250.0f,415.0f },
										{ 1203.0f,424.8f } } },rightTeam.GetPrimaryColor() ) );
		creases.push_back( GoalCrease( true,{ 76.0f,360.0f },105.0f ) );
		creases.push_back( GoalCrease( false,{ 1203.0f,360.0f },105.0f ) );

		periodEvent.AddObserver( haltObserver );
		goalZones[0].AddObserver( haltObserver );
		goalZones[1].AddObserver( haltObserver );
		leftTeam.ObserveGoal( goalZones[1] );
		rightTeam.ObserveGoal( goalZones[0] );

		controllers.push_back( leftTeam.GetFactory().Make( players[0],*this ) );
		controllers.push_back( rightTeam.GetFactory().Make( players[1],*this ) );

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
		const float dtStep = dt / (float)stepsPerFrame;
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
				c->Update( dtStep );
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
			UpdateClock( dtStep );
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
private:
	inline void UpdateClock( float dt )
	{
		if( !clockStopped )
		{
			timeElapsed += dt;
		}
	}
	inline void StopClock()
	{
		clockStopped = true;
	}
	inline void StartClock()
	{
		clockStopped = false;
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
	inline virtual float GetTimeElapsed() const override
	{
		return timeElapsed;
	}
private:
	std::vector< PhysicalCircle* > circles;
	std::vector< Player > players;
	std::vector< Ball >	balls;
	std::vector< GoalZone > goalZones;
	std::vector< GoalCrease > creases;
	Walls walls;
	std::vector< std::unique_ptr< Controller > > controllers;
	DragProcessor dp;
	bool clockStopped = false;
	float timeElapsed;
	const unsigned int stepsPerFrame = 8;
	const unsigned int stepsPerInput = 8;
	unsigned int stepCount = 0;
	std::shared_ptr< HaltPlayObs > haltObserver;
};