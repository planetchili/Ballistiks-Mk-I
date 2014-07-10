#pragma once
#include "Observable.h"
#include "Controller.h"
#include "GoalZone.h"
#include "Colors.h"
#include <array>

class Team
{
private:
	class GoalObserverRelay : public Observer,
							  public Observable
	{
	public:
		virtual void OnNotify() override
		{
			team.score++;
			Notify();
		}
		static std::shared_ptr< GoalObserverRelay > Make( Team& team )
		{
			return std::shared_ptr<GoalObserverRelay>( new GoalObserverRelay( team ) );
		}
	private:
		GoalObserverRelay( Team& team )
			:
			team( team )
		{}
	private:
		Team& team;
	};
public:
	Team( Controller::Factory& factory,const std::array<Color,2> colors,
		unsigned int nMembers = 1 )
		:
		nMembers( nMembers ),
		factory( factory ),
		goalRelay( GoalObserverRelay::Make( *this ) ),
		colors( colors )
	{}
	void ObserveGoal( GoalZone& goal )
	{
		goal.AddObserver( goalRelay );
	}
	void AddGoalRelayObserver( std::shared_ptr< Observer > obs )
	{
		goalRelay->AddObserver( obs );
	}
	inline unsigned int GetScore() const
	{
		return score;
	}
	inline const std::wstring& GetName() const
	{
		return factory.GetName();
	}
	Controller::Factory& GetFactory()
	{
		return factory;
	}
	Color GetPrimaryColor() const
	{
		return colors[0];
	}
	Color GetSecondaryColor() const
	{
		return colors[1];
	}
private:
	const unsigned int nMembers;
	unsigned int score = 0;
	Controller::Factory& factory;
	std::shared_ptr< GoalObserverRelay > goalRelay;
	const std::array<Color,2> colors;
};