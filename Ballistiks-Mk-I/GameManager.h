#pragma once
#include <memory>
#include <array>
#include "World.h"
#include "Controller.h"
#include "Team.h"

class GameManager
{
private:
	class PeriodEvent : public Observable
	{
	public:
		void OnPeriodEnd()
		{
			Notify();
		}
	};
public:
	GameManager( Controller::Factory& factoryA,Controller::Factory& factoryB )
		:
		teamA( factoryA,{ { TEAM_A_COLOR0,TEAM_A_COLOR1 } } ),
		teamB( factoryB,{ { TEAM_B_COLOR0,TEAM_B_COLOR1 } } )
	{
		ResetWorld();
	}
	void StartNewPoint()
	{
		pointStartTime = pWorld->GetTimeElapsed();
		ResetWorld();
	}
	void StartNewPeriod()
	{
		period++;
		pointStartTime = 0.0f;
		ResetWorld();
	}
	inline void Step( float dt )
	{
		pWorld->Step( dt );
		if( pWorld->GetTimeElapsed() >= periodLength )
		{
			periodEvent.OnPeriodEnd();
		}
	}
	inline void RenderWorld( DrawTarget& tgt )
	{
		pWorld->Render( tgt );
	}
	inline float GetTimeRemaining() const
	{
		return periodLength - pWorld->GetTimeElapsed();
	}
	inline const Team& GetTeamA() const
	{
		return teamA;
	}
	inline const Team& GetTeamB() const
	{
		return teamB;
	}
	inline const unsigned int GetPeriod() const
	{
		return period;
	}
	void AddTeamObservers( std::shared_ptr< Observer > obsA,std::shared_ptr< Observer > obsB )
	{
		teamA.AddGoalRelayObserver( obsA );
		teamB.AddGoalRelayObserver( obsB );
	}
	void AddPeriodObserver( std::shared_ptr< Observer > obs )
	{
		periodEvent.AddObserver( obs );
	}
private:
	void ResetWorld()
	{
		if( period % 2 == 0 )
		{
			pWorld = std::make_unique< World >( pointStartTime,teamA,teamB,periodEvent );
		}
		else
		{
			pWorld = std::make_unique< World >( pointStartTime,teamB,teamA,periodEvent );
		}
	}
private:
	unsigned int period = 0;
	const float periodLength = 1.0f * 60.0f;
	float pointStartTime = 0.0f;
	Team teamA;
	Team teamB;
	std::unique_ptr< World > pWorld;
	PeriodEvent periodEvent;
};