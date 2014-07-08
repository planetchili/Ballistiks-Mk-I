#pragma once
#include "Observable.h"
#include "Controller.h"
#include "GoalZone.h"

class Team
{
private:
	class GoalObserverRelay : public Observer,
							  public Observable
	{
	public:
		GoalObserverRelay( Team& team )
			:
			team( team )
		{}
		virtual void OnNotify() override
		{
			team.score++;
			Notify();
		}
	private:
		Team& team;
	};
public:
	Team( Controller::Factory& factory,unsigned int nMembers = 1 )
		:
		nMembers( nMembers ),
		factory( factory ),
		goalRelay( *this )
	{}
	void ObserveGoal( GoalZone& goal )
	{
		goal.AddObserver( goalRelay );
	}
	void AddGoalRelayObserver( Observer& obs )
	{
		goalRelay.AddObserver( obs );
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
private:
	const unsigned int nMembers;
	unsigned int score = 0;
	Controller::Factory& factory;
	GoalObserverRelay goalRelay;
};