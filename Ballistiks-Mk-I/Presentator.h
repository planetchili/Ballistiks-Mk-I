#pragma once
#include <memory>
#include "GameManager.h"
#include "DrawTarget.h"
#include "Midi.h"
#include "Sound.h"
#include "Viewport.h"
#include "Camera.h"

class Presentator
{
private:
	class Text : public Drawable
	{
	public:
		Text( const std::wstring& string,const RectF& rect,const Font& font,Color color )
			:
			string( string ),
			rect( rect ),
			font( font ),
			color( color )
		{}
		virtual void Rasterize( D3DGraphics& gfx ) const override
		{
			gfx.DrawString( string,rect,font,color );
		}
	private:
		const std::wstring string;
		const RectF rect;
		const Font& font;
		const Color color;
	};
private:
	class FlagObserver : public Observer
	{
	public:
		inline bool IsSet() const
		{
			return isSet;
		}
		inline void Reset()
		{
			isSet = false;
		}
		virtual void OnNotify() override
		{
			isSet = true;
		}
		static std::shared_ptr< FlagObserver > Make()
		{
			return std::shared_ptr< FlagObserver >( new FlagObserver() );
		}
	private:
		FlagObserver() {}
	private:
		bool isSet = false;
	};
#pragma region States
private:
	class State
	{
	protected:
		State( Presentator& parent )
			:
			parent( parent )
		{}
	public:
		virtual ~State() {}
		virtual void Step( float dt ) {}
		virtual void Observe() {}
		virtual void Draw( DrawTarget& tgt ) {}
	protected:
		Presentator& parent;
	};
	class NullState : public State
	{
	public:
		NullState( Presentator& parent )
			:
			State( parent )
		{}
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
			_Step( dt );
			countdownTime -= dt;
			if( countdownTime <= 0.0f )
			{
				OnCountdownFinished();
			}
		}
	protected:
		virtual void OnCountdownFinished() = 0;
		virtual void _Step( float dt ) {}
	protected:
		float countdownTime;
	};
	class BeginTitleState : public WaitState
	{
	public:
		BeginTitleState( Presentator& parent )
			:
			WaitState( parent,3.0f ),
			nameFont( L"Times New Roman",60 ),
			txtA(parent.GetManager().GetTeamA().GetName(),
				{ parent.vp.GetHeight() / 2.0f - 200.0f,
				  parent.vp.GetHeight() / 2.0f,0.0f,parent.vp.GetWidth() },
				  nameFont,parent.GetManager().GetTeamA().GetPrimaryColor() ),
			txtB( parent.GetManager().GetTeamB().GetName(),
				{ parent.vp.GetHeight() / 2.0f + 100.0f,
				  parent.vp.GetHeight() / 2.0f + 300.0f,0.0f,parent.vp.GetWidth() },
				  nameFont,parent.GetManager().GetTeamB().GetPrimaryColor() ),
			vs(L"VS",
				{ parent.vp.GetHeight() / 2.0f - 50.0f,
				  parent.vp.GetHeight() / 2.0f + 50.0f,0.0f,parent.vp.GetWidth() },
				  nameFont,WHITE)
		{
			parent.vsSound.Play(-500);
		}
	protected:
		virtual void OnCountdownFinished() override;
		virtual void Draw( DrawTarget& dt );
	private:
		const Font nameFont;
		Text txtA;
		Text txtB;
		Text vs;
	};
	class GameOnState : public State
	{
	public:
		GameOnState( Presentator& parent )
			:
			State( parent )
		{
			parent.batmanTheme.Play();
			parent.whistleShort.Play();
		}
		virtual void Observe() override;
		virtual void Draw( DrawTarget& tgt ) override;
		virtual void Step( float dt ) override;
	};
	class GoalScoredState : public WaitState
	{
	public:
		GoalScoredState( Presentator& parent )
			:
			WaitState( parent,5.0f ),
			nameFont( L"Times New Roman",80 ),
			scoreText( L"GOAL SCORED!",
			{ parent.vp.GetHeight() / 2.0f - 50.0f,
			parent.vp.GetHeight() / 2.0f + 50.0f,0.0f,parent.vp.GetWidth() },
			nameFont,WHITE )
		{
			parent.whistleBurst.Play();
			parent.crowd.Play();
			parent.batmanTheme.Stop();
		}
		virtual void Draw( DrawTarget& dt ) override;
	protected:
		virtual void OnCountdownFinished() override;
		virtual void _Step( float dt ) override;
	private:
		const Font nameFont;
		Text scoreText;
	};
	class PeriodEndState : public WaitState
	{
	public:
		PeriodEndState( Presentator& parent )
			:
			WaitState( parent,3.0f ),
			nameFont( L"Times New Roman",60 ),
			endText( L"Period " + std::to_wstring( parent.manager->GetPeriod() + 1 ) + L" Over",
			{ parent.vp.GetHeight() / 2.0f - 50.0f,
			parent.vp.GetHeight() / 2.0f + 50.0f,0.0f,parent.vp.GetWidth() },
			nameFont,WHITE )
		{
			parent.whistleLong.Play();
			parent.batmanTheme.Stop();
		}
		virtual void Draw( DrawTarget& dt ) override;
	protected:
		virtual void OnCountdownFinished() override;
		virtual void _Step( float dt ) override;
	private:
		const Font nameFont;
		Text endText;
	};
	class GameEndState : public State
	{
	public:
		GameEndState( Presentator& parent )
			:
			State( parent ),
			nameFont( L"Times New Roman",60 ),
			endText( L"Game Over",
			{ parent.vp.GetHeight() / 2.0f - 50.0f,
			parent.vp.GetHeight() / 2.0f + 50.0f,0.0f,parent.vp.GetWidth() },
			nameFont,WHITE )
		{
			parent.batmanTheme.Stop();
			parent.horn.Play(-150);
			parent.crowd.Play();
		}
		virtual void Step( float dt ) override;
		virtual void Draw( DrawTarget& tgt ) override;
	private:
		const Font nameFont;
		Text endText;
	};
#pragma endregion
public:
	Presentator( DSound& sound,Camera& cam,Viewport& vp )
		:
		whistleShort( sound.CreateSound( "whistleshort.wav" ) ),
		vsSound( sound.CreateSound( "vs.wav" ) ),
		whistleBurst(sound.CreateSound("whistleburst.wav")),
		crowd(sound.CreateSound("crowd.wav")),
		whistleLong(sound.CreateSound("whistlelong.wav")),
		horn(sound.CreateSound("horn.wav")),
		batmanTheme( L"batman_edit.mid",1.3f ),
		state(std::make_unique< NullState >( *this )),
		ptObs(FlagObserver::Make()),
		perObs(FlagObserver::Make()),
		cam( cam),
		vp(vp)
	{}
	inline void Step( float dt )
	{
		state->Step( dt );
		state->Observe();
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
		perObs->Reset();
		ptObs->Reset();
		manager->AddPeriodObserver( perObs );
		manager->AddTeamObservers( ptObs,ptObs );
		Transition( std::make_unique<BeginTitleState>( *this ) );
	}
private:
	inline void Transition( std::unique_ptr< State > newState )
	{
		state = std::move( newState );
	}
private:
	Camera& cam;
	Viewport& vp;
	MidiSong batmanTheme;
	Sound whistleShort;
	Sound whistleBurst;
	Sound vsSound;
	Sound crowd;
	Sound whistleLong;
	Sound horn;
	std::unique_ptr< State > state;
	std::unique_ptr< GameManager > manager;
	std::shared_ptr< FlagObserver > ptObs;
	std::shared_ptr< FlagObserver > perObs;
};