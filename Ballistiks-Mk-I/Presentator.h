#pragma once
#include <memory>
#include "DrawTarget.h"

class Presentator
{
private:
	class State
	{
	public:
		State( Presentator& parent )
			:
			parent( parent )
		{}
		virtual void Step( float dt ) {}
		virtual void OnGoal() {}
		virtual void OnPeriod() {}
		virtual void Draw( DrawTarget& dt ) {}
	protected:
		Presentator& parent;
	};
	class WaitState : public State
	{
	public:
		WaitState( Presentator& parent,float time )
			:
			State( parent ),
			countdownTime( time )
		{}
		virtual void Step( float dt ) final
		{
			countdownTime -= dt;
			if( countdownTime <= 0.0f )
			{
				OnCountdownFinished();
			}			
		}
	protected:
		virtual void OnCountdownFinished() = 0;
	protected:
		float countdownTime;
	};
	class BeginTitleState : public WaitState
	{
	protected:
		virtual void OnCountdownFinished() override;
		virtual void Draw( DrawTarget& dt );
	};
	class GameOn : public State
	{
	public:
		GameOn( Presentator& parent )
			:
			State( parent )
		{}
		virtual void OnGoal() override;
		virtual void OnPeriod() override;
		virtual void Draw( DrawTarget& dt ) {}
	};
public:
	inline State& GetState()
	{
		return *state;
	}
private:
	inline void Transition( std::unique_ptr< State > newState )
	{
		state = std::move( newState );
	}
private:
	std::unique_ptr< State > state;
};