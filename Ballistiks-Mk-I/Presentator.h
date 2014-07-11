#pragma once
#include <memory>
#include "GameManager.h"
#include "DrawTarget.h"
#include "Midi.h"
#include "Sound.h"

class Presentator
{
#pragma region States
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
		virtual void Draw( DrawTarget& tgt ) {}
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
	class NullState : public State
	{
	public:
		NullState( Presentator& parent )
			:
			State( parent )
		{}
	};
	class GameOn : public State
	{
	public:
		GameOn( Presentator& parent )
			:
			State( parent )
		{
			parent.whistleShort.Play();
		}
		virtual void OnGoal() override;
		virtual void OnPeriod() override;
		virtual void Draw( DrawTarget& tgt ) override;
	};
#pragma endregion
#pragma region Observers
private:
	class PointObserver : public Observer
	{
	public:
		virtual void OnNotify() override
		{
			parent.state->OnGoal();
		}
		static std::shared_ptr< PointObserver > Make( Presentator& parent )
		{
			return std::shared_ptr< PointObserver >( new PointObserver( parent ) );
		}
	private:
		PointObserver( Presentator& parent )
			:
			parent( parent )
		{}
	private:
		Presentator& parent;
	};
	class PeriodObserver : public Observer
	{
	public:
		virtual void OnNotify() override
		{
			parent.state->OnPeriod();
		}
	private:
		PeriodObserver( Presentator& parent )
			:
			parent( parent )
		{}
	private:
		Presentator& parent;
	};
#pragma endregion
public:
	Presentator( DSound& sound )
		:
		whistleShort( sound.CreateSound( "whistle.wav" ) ),
		state(std::make_unique< NullState >( *this ))
	{}
	inline void Step( float dt )
	{
		state->Step( dt );
	}
	inline void Draw( DrawTarget& tgt )
	{
		state->Draw( tgt );
	}
	inline const GameManager& GetManager() const
	{
		return *manager;
	}
	inline void StartGame( Controller::Factory& teamA,Controller::Factory& teamB )
	{
		manager = std::make_unique< GameManager >( teamA,teamB );
		Transition( std::make_unique<GameOn>( *this ) );
	}
private:
	inline void Transition( std::unique_ptr< State > newState )
	{
		state = std::move( newState );
	}
private:
	//MidiSong batmanTheme;
	//MidiSong littleHobo;
	Sound whistleShort;
	//Sound whistleLong;
	//Sound batmanTransition;
	std::unique_ptr< State > state;
	std::unique_ptr< GameManager > manager;
	std::shared_ptr< PointObserver > ptObs;
	std::shared_ptr< PeriodObserver > perObs;
};