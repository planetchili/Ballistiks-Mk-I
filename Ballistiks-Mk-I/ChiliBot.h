#pragma once
#include "AI.h"
#include "Vec2.h"
#include <algorithm>


inline bool AreSameSide( Vec2 a,Vec2 b,Vec2 q,Vec2 p )
{
	const Vec2 ab = b - a;
	return ab.Cross( p - a ) * ab.Cross( q - a ) >= 0.0f;
}

inline bool IsInSector( Vec2 c,Vec2 n,float field,Vec2 p )
{
	const float phi = field / 2.0f;
	const Vec2 qn = c + n;
	const Vec2 q0 = c + n.Rotation(  phi ).Normalize();
	const Vec2 q1 = c + n.Rotation( -phi ).Normalize(); 
	return AreSameSide( c,q0,qn,p ) &&
		AreSameSide( c,q1,qn,p );
}

class ChiliBot2 : public AI
{
public:
	ChiliBot2( ControllablePlayer& player,const ViewableWorld& view )
		:
		AI( player,view ),
		goal( player.PlayerConst().GetCenter().x > 640.0f
			? Vec2 { 80.0f,360.0f } : Vec2 { 1200.0f,360.0f } ),
		ball( view.GetBalls().front() ),
		me( player.PlayerConst() ),
		other( *std::find_if(
			view.GetPlayers().begin(),view.GetPlayers().end(),
			[this]( const Player& p ) -> bool { return p != me; } ) )
	{
	}
protected:
	virtual void _Process() override
	{
		const Vec2 ballToP = ( ball.GetCenter() - goal ).Normalize() * ( ball.GetRadius() + me.GetRadius() * pFactor );
		const Vec2 p = ball.GetCenter() + ballToP;
		const Vec2 q0 = ball.GetCenter() + ballToP.Rotation( phi ).Normalize() * me.GetRadius() * qFactor;
		const Vec2 q1 = ball.GetCenter() + ballToP.Rotation( -phi ).Normalize() * me.GetRadius() * qFactor;

		if( AreSameSide( ball.GetCenter(),q0,me.GetCenter(),p ) &&
			AreSameSide( ball.GetCenter(),q1,me.GetCenter(),p ) )
		{
			player.SetThrustVector( ( p - me.GetCenter() ).Normalize() );
			if( me.GetRefactoryTime() <= 0.0f &&
				IsInSector( ball.GetCenter(),ballToP,burstField,me.GetCenter() ) &&
				(other.GetCenter().ClosestPtOnLine( ball.GetCenter(),goal) - other.GetCenter() ).LenSq()
					> sq( ball.GetRadius() + other.GetRadius() ) )
			{
				player.Burst();
			}
		}
		else
		{
			if( ( q0 - me.GetCenter() ).LenSq() <= ( q1 - me.GetCenter() ).LenSq() )
			{
				player.SetThrustVector( ( q0 - me.GetCenter() ).Normalize() );
			}
			else
			{
				player.SetThrustVector( ( q1 - me.GetCenter() ).Normalize() );
			}
		}
	}
private:
	const Player& me;
	const Player& other;
	const Ball& ball;
	const Vec2 goal;
	const float phi = PI / 5.5f;
	const float qFactor = 1.5f;
	const float pFactor = 0.8f;
	const float burstField = PI / 10.0f;
};

class ChiliBot : public AI
{
public:
	ChiliBot( ControllablePlayer& player,const ViewableWorld& view )
		:
		AI( player,view ),
		goal( player.PlayerConst().GetCenter().x > 640.0f
			? Vec2 { 80.0f,360.0f } : Vec2 { 1200.0f,360.0f } )
	{
	}
protected:
	virtual void _Process() override
	{
		const Ball& ball = view.GetBalls().front();
		const Player& me = player.PlayerConst();

		const Vec2 ballToP = ( ball.GetCenter() - goal ).Normalize() * (ball.GetRadius() + me.GetRadius() * pFactor);
		const Vec2 p  = ball.GetCenter() + ballToP;
		const Vec2 q0 = ball.GetCenter() + ballToP.Rotation(  phi ).Normalize() * me.GetRadius() * qFactor;
		const Vec2 q1 = ball.GetCenter() + ballToP.Rotation( -phi ).Normalize() * me.GetRadius() * qFactor;

		auto IsSameSide = [p]( Vec2 a,Vec2 b,Vec2 c ) -> bool
		{
			const Vec2 ab = b - a;
			return ab.Cross( p - a ) * ab.Cross( c - a ) >= 0.0f;
		};

		if( IsSameSide( ball.GetCenter(),q0,me.GetCenter() ) &&
			IsSameSide( ball.GetCenter(),q1,me.GetCenter() ) )
		{
			player.SetThrustVector( ( p - me.GetCenter() ).Normalize() );
		}
		else
		{
			if( ( q0 - me.GetCenter() ).LenSq() <= ( q1 - me.GetCenter() ).LenSq() )
			{
				player.SetThrustVector( ( q0 - me.GetCenter() ).Normalize() );
			}
			else
			{
				player.SetThrustVector( ( q1 - me.GetCenter() ).Normalize() );
			}
		}
	}
private:
	const Vec2 goal;
	const float phi = PI / 5.5f;
	const float qFactor = 1.5f;
	const float pFactor = 0.8f;
};

class ChiliBotFactory : public Controller::Factory
{
public:
	ChiliBotFactory()
		:
		Factory( L"chili" )
	{}
	virtual std::unique_ptr< Controller > Make( ControllablePlayer& player,const ViewableWorld& view ) override
	{
		return std::make_unique< ChiliBot2 >( player,view );
	}
	virtual std::unique_ptr< Controller > Make( ControllablePlayer& player,const ViewableWorld& view,unsigned int version ) override
	{
		std::unique_ptr< Controller > ai;
		switch( version )
		{
		case 0:
		case 1:
			ai = std::make_unique< ChiliBot >( player,view );
			break;
		default:
			ai = Make( player,view );
			break;
		}
		return ai;
	}
};