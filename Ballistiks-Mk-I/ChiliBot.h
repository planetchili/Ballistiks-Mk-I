#pragma once
#include "AI.h"
#include "Vec2.h"
#include <algorithm>
#include <array>


inline bool AreSameSide( Vec2 a,Vec2 b,Vec2 q,Vec2 p )
{
	const Vec2 ab = b - a;
	return ab.Cross( p - a ) * ab.Cross( q - a ) >= 0.0f;
}

inline bool IsInSector( Vec2 c,Vec2 n,float field,Vec2 p )
{
	const float phi = field / 2.0f;
	const Vec2 qn = c + n;
	const Vec2 q0 = c + n.Rotation( phi ).Normalize();
	const Vec2 q1 = c + n.Rotation( -phi ).Normalize();
	return AreSameSide( c,q0,qn,p ) &&
		AreSameSide( c,q1,qn,p );
}

class ChiliBot3 : public AI
{
public:
	ChiliBot3( ControllablePlayer& player,const ViewableWorld& view )
		:
		AI( player,view ),
		goal( player.PlayerConst().GetCenter().x > 640.0f
		? Vec2 { 80.0f,360.0f } : Vec2 { 1200.0f,360.0f } ),
		ownGoal( player.PlayerConst().GetCenter().x > 640.0f
		? Vec2 { 1200.0f,360.0f } : Vec2 { 80.0f,360.0f } ),
		ball( view.GetBalls().front() ),
		me( player.PlayerConst() ),
		other( *std::find_if(
		view.GetPlayers().begin(),view.GetPlayers().end(),
		[this]( const Player& p ) -> bool { return p != me; } ) ),
		wallDiags( { {
			{ { 80.0f,110.0f },  { 150.0f,40.0f } },
			{ { 1130.0f,40.0f }, { 1200.0f,110.0f } },
			{ { 1200.0f,610.0f },{ 1130.0f,680.0f } },
			{ { 150.0f,680.0f }, { 80.0f,610.0f } } } } )
	{
		const float offset = BALL_RADIUS * 1.3f;
		wallRect = {
			40.0f   + offset,
			680.0f  - offset,
			80.0f   + offset,
			1200.0f - offset };
		for( auto& d : wallDiags )
		{
			const Vec2 in = ( d.second - d.first ).CW90().Normalize();
			d = { d.first + in * offset,d.second + in * offset };
		}
	}
protected:
	bool BallsToWalls() const
	{
		bool outsideDiag = false;
		for( auto& d : wallDiags )
		{
			outsideDiag = outsideDiag || AreSameSide( d.first,d.second,
				d.first + ( d.second - d.first ).CCW90(),ball.GetCenter() );
		}
		return outsideDiag || !wallRect.Contains( ball.GetCenter() );
	}
	virtual void _Process() override
	{
		if( BallsToWalls() && 
			( ball.GetCenter() - me.GetCenter() ).LenSq() <= sq( ( BALL_RADIUS + PLAYER_RADIUS ) * 1.15f ) &&
			( ball.GetCenter() - other.GetCenter() ).LenSq() <= sq( ( BALL_RADIUS + PLAYER_RADIUS ) * 1.15f ) )
		{
				player.SetThrustVector( ( ball.GetCenter() - me.GetCenter() ).Normalize() );
				player.Burst();
		}
		else
		{
			const Vec2 ballToP = ( ball.GetCenter() - goal ).Normalize() * ( ball.GetRadius() + me.GetRadius() * pFactor );
			const Vec2 p = ball.GetCenter() + ballToP;
			const Vec2 q0 = ball.GetCenter() + ballToP.Rotation( phi ).Normalize() * me.GetRadius() * qFactor;
			const Vec2 q1 = ball.GetCenter() + ballToP.Rotation( -phi ).Normalize() * me.GetRadius() * qFactor;

			if( goal.x > 640.0f ? ball.GetCenter().x > 560.0f : ball.GetCenter().x < 720.0f )
			{
				if( AreSameSide( ball.GetCenter(),q0,me.GetCenter(),p ) &&
					AreSameSide( ball.GetCenter(),q1,me.GetCenter(),p ) )
				{
					player.SetThrustVector( ( p - me.GetCenter() ).Normalize() );
					if( me.GetRefactoryTime() <= 0.0f &&
						IsInSector( ball.GetCenter(),ballToP,burstField,me.GetCenter() ) &&
						( other.GetCenter().ClosestPtOnLine( ball.GetCenter(),goal ) - other.GetCenter() ).LenSq()
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
			else
			{
				const Vec2 ballToP = ( ownGoal - ball.GetCenter() ).Normalize() * ( ball.GetRadius() + me.GetRadius() * pFactor );
				const Vec2 p = ballToP + ball.GetCenter();
				const Vec2 q0 = ball.GetCenter() + ballToP.Rotation( phi ).Normalize() * me.GetRadius() * qFactor;
				const Vec2 q1 = ball.GetCenter() + ballToP.Rotation( -phi ).Normalize() * me.GetRadius() * qFactor;
				if( AreSameSide( ball.GetCenter(),q0,me.GetCenter(),p ) &&
					AreSameSide( ball.GetCenter(),q1,me.GetCenter(),p ) )
				{
					player.SetThrustVector( ( p - me.GetCenter() ).Normalize() );
					if( me.GetRefactoryTime() <= 0.0f &&
						IsInSector( ball.GetCenter(),ballToP,burstField,me.GetCenter() ) &&
						( other.GetCenter().ClosestPtOnLine( ball.GetCenter(),ownGoal ) - other.GetCenter() ).LenSq()
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
				if( (me.GetCenter() - ball.GetCenter()).LenSq() > sq(me.GetRadius() * 4.0f) )
				{
					player.Burst();
				}
			}
		}
	}
private:
	const Player& me;
	const Player& other;
	const Ball& ball;
	const Vec2 goal;
	const Vec2 ownGoal;
	const float phi = PI / 5.0f;
	const float qFactor = 1.5f;
	const float pFactor = 0.8f;
	const float burstField = PI / 10.0f;
	RectF wallRect;
	std::array< std::pair<Vec2,Vec2>,4 > wallDiags;
};

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
				( other.GetCenter().ClosestPtOnLine( ball.GetCenter(),goal ) - other.GetCenter() ).LenSq()
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

		const Vec2 ballToP = ( ball.GetCenter() - goal ).Normalize() * ( ball.GetRadius() + me.GetRadius() * pFactor );
		const Vec2 p = ball.GetCenter() + ballToP;
		const Vec2 q0 = ball.GetCenter() + ballToP.Rotation( phi ).Normalize() * me.GetRadius() * qFactor;
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
		return std::make_unique< ChiliBot3 >( player,view );
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
		case 2:
			ai = std::make_unique< ChiliBot2 >( player,view );
			break;
		default:
			ai = Make( player,view );
			break;
		}
		return ai;
	}
};

class ChiliBotControlFactory : public Controller::Factory
{
public:
	ChiliBotControlFactory()
		:
		Factory( L"chili_control" )
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
		case 2:
			ai = std::make_unique< ChiliBot2 >( player,view );
			break;
		default:
			ai = Make( player,view );
			break;
		}
		return ai;
	}
};