#pragma once
#include "AI.h"

class LuXAI : public AI
{
public:
	LuXAI( ControllablePlayer& player, const ViewableWorld& view )
		:
		AI( player,view ),
		MyGoalRight( player.PlayerConst( ).GetCenter( ).x > 640.0f ? true : false ) // Apparently using static bool doesn't work; using chilis method instead
	{}

	inline Vec2 NewPoint( Vec2 &p, float dist, float ang )
	{
		return Vec2( p.x + dist * cos( ang ), p.y + dist * sin( ang ) );
	}
	inline Vec2 VectorSet( float dist, float ang )
	{
		return Vec2( dist * cos( ang ), dist * sin( ang ) );
	}
	inline Vec2 VectorSet( Vec2  &a, Vec2 &b )
	{
		return Vec2( b.x - a.x, b.y - a.y );
	}
	inline float GetAngle( Vec2 &v )
	{
		float x = atan2( v.y, v.x ); return x < 0 ? x + 6.283f : x;
	}
	inline float GetAngle( Vec2 a, Vec2 b )
	{
		float x = atan2( b.y - a.y, b.x - a.x ); return x < 0 ? x + 6.283f : x;
	}
	inline float GetDistance( Vec2 &a, Vec2 &b )
	{
		return sqrtf( pow( a.x - b.x, 2 ) + pow( a.y - b.y, 2 ) );
	}
	inline float GetDistance( Vec2 &a )
	{
		return sqrtf( pow( a.x, 2 ) + pow( a.y, 2 ) );
	}


	class Circle
	{
	public: Vec2 o; float radius;

			Circle( Vec2 &o, float radius ) : o( o ), radius( radius ) { }
			Circle( ) : o( 0, 0 ), radius( 0 ) { }
	};
	struct Line
	{
		Vec2 a, b;

		Line( Vec2 a, Vec2 b ) : a( a ), b( b ) { }
		Line( ) : a( 0, 0 ), b( 0, 0 ) { }
	};

	inline float CrossProduct( Vec2 &a, Vec2 &b )
	{
		return a.x * b.y - a.y * b.x;
	}
	inline bool LineIntersect( Line& A, Line& B )
	{
		A.b = VectorSet( A.a, A.b ); B.b = VectorSet( B.a, B.b );

		if ( CrossProduct( A.b, B.b ) == 0 ) return false;
		if ( CrossProduct( Vec2( B.a - A.a ), A.b ) == 0 ) return false;

		float t = CrossProduct( Vec2( B.a - A.a ), B.b ) / CrossProduct( A.b, B.b );
		float u = CrossProduct( Vec2( B.a - A.a ), A.b ) / CrossProduct( A.b, B.b );

		if ( t >= 0 && t <= 1 && u >= 0 && u <= 1 ) return true; else return false;
	}

	inline bool CircleLineCollision( Line& A, Circle& O )
	{
		if ( GetDistance( A.a, O.o ) <= O.radius ) { return true; }
		if ( GetDistance( A.b, O.o ) <= O.radius ) { return true; }

		float a = GetAngle( A.a, A.b );
		Vec2 Aq = NewPoint( A.a, 20, a + 1.571f );
		Vec2 Bq = NewPoint( A.b, 20, ( a + 3.142f ) + 1.571f );

		if ( SideOfLine( Aq, A.a, O.o ) == Dir::Left || SideOfLine( Bq, A.b, O.o ) == Dir::Left ) { return false; }

		return ( abs( ( A.b.x - A.a.x ) * ( A.a.y - O.o.y ) - ( A.a.x - O.o.x ) * ( A.b.y - A.a.y ) ) ) /
			( sqrtf( ( A.b.x - A.a.x ) * ( A.b.x - A.a.x ) + ( A.b.y - A.a.y ) * ( A.b.y - A.a.y ) ) ) <= O.radius;
	}




	class Dir { public: enum Direction { None, Up, Right, Down, Left }; };

	inline Dir::Direction SideOfLine( Vec2 &a, Vec2 &b, Vec2 &t )
	{
		return ( ( ( b.x - a.x ) * ( t.y - a.y ) - ( b.y - a.y ) * ( t.x - a.x ) ) >= 0 ? Dir::Right : Dir::Left );
	}
	inline Dir::Direction SideOfLineEx( Vec2 &a, Vec2 &b, Vec2 &t )
	{
		float f = ( b.x - a.x ) * ( t.y - a.y ) - ( b.y - a.y ) * ( t.x - a.x ); return ( f > 0 ? Dir::Right : ( f < 0 ? Dir::Left : Dir::None ) );
	}

protected:
	virtual void _Process() override
	{
		// Get player and enemy ball
		const Player& BALL_PLAYER = player.PlayerConst( );
		const Player& BALL_ENEMY = [ BALL_PLAYER, this ]( ) -> const Player&
		{
			for ( const Player& p : view.GetPlayers( ) )
			{
				if ( p != BALL_PLAYER )
				{
					return p;
				}
			}
		}( );

		// Set variables

		Vec2 BALL_POS = view.GetBalls( ).front( ).GetCenter( );

		float PLAYER_RAD = BALL_PLAYER.GetRadius( );
		Vec2 PLAYER_POS = BALL_PLAYER.GetCenter( );
		Vec2 PLAYER_VEL = BALL_PLAYER.GetVel( );
		Vec2 ENEMY_POS = BALL_ENEMY.GetCenter( );
		Vec2 ENEMY_VEL = BALL_ENEMY.GetVel( );

		Vec2 WALL_TL = { 80.0f, 40.0f };	// Top		left	corner
		Vec2 WALL_TR = { 1199.0f, 40.0f };	// Top		right	corner
		Vec2 WALL_BR = { 1199.0f, 679.0f };	// Bottom	right	corner
		Vec2 WALL_BL = { 80.0f, 679.0f };	// Bottom	left	corner

		float GoalRad = 105.0f;

		Vec2 Goal_LT = { 76, 296 };	// Left goal top
		Vec2 Goal_LB = { 76, 423 };	// Left goal bottom
		Vec2 Goal_ALT = { 94, 309 };	// Alt left goal top
		Vec2 Goal_ALB = { 94, 410 };	// Alt left goal bottom
		Vec2 Goal_LC = { 76, 360 };	// Left		goal center

		Vec2 Goal_RT = { 1203, 296 };	// Right goal top
		Vec2 Goal_RB = { 1203, 423 };	// Right goal bottom
		Vec2 Goal_ART = { 1185, 309 };	// Alt right goal top
		Vec2 Goal_ARB = { 1185, 410 };	// Alt right goal bottom
		Vec2 Goal_RC = { 1203, 360 };	// Right	 goal center

		

		// AI part
		float VelAng = GetAngle( PLAYER_VEL );							// Direction of current movement
		float PlaAng = GetAngle( VectorSet( PLAYER_POS, BALL_POS ) );	// Direction towards ball

		float direction = PlaAng;	// Initially move towards ball


		if ( MyGoalRight )
		{
			float distFromGoal = GetDistance( PLAYER_POS, Goal_LC );
			bool PlaMovBall = VelAng > PlaAng - ( 1.0f / distFromGoal * 20.0f ) && VelAng < PlaAng + ( 1.0f / distFromGoal * 20.0f );				// Player moving towards ball
			bool PlaMovGoal = LineIntersect( Line( PLAYER_POS, NewPoint( PLAYER_POS, 1000.0f, VelAng ) ), Line( Goal_ALT, Goal_ALB ) ) &&			// Player moving towards goal
				LineIntersect( Line( PLAYER_POS, NewPoint( PLAYER_POS, 1000.0f, VelAng ) ), Line( Goal_LT, Goal_LB ) );
			bool ShotAtGoal = LineIntersect( Line( PLAYER_POS, NewPoint( PLAYER_POS, 1000.0f, PlaAng ) ), Line( Goal_ALT, Goal_ALB ) ) &&			// Player-ball aligned with goal
				LineIntersect( Line( PLAYER_POS, NewPoint( PLAYER_POS, 1000.0f, PlaAng ) ), Line( Goal_LT, Goal_LB ) );
			bool EneInWay = CircleLineCollision( Line( PLAYER_POS, NewPoint( PLAYER_POS, 1000.0f, VelAng ) ), Circle( ENEMY_POS, PLAYER_RAD ) );	// Enemy in the way
			bool HasBall = GetDistance( PLAYER_POS, BALL_POS ) <= 40.0f;


			if ( !ShotAtGoal && HasBall )
			{


				Vec2 PointInFront = NewPoint( PLAYER_POS, 100.0f, PlaAng );
				Dir::Direction side = SideOfLine( PLAYER_POS, PointInFront, Goal_LC );

				if ( side == Dir::Left )
				{
					direction += 0.5f;
				}
				else if ( side == Dir::Right )
				{
					direction -= 0.5f;
				}
			}

			player.SetThrustVector( VectorSet( 10.0f, direction ) );

			// Burst - If player moves towards the ball, and towards the goal, and the enemy is not in the way
			if ( PlaMovBall && PlaMovGoal && !EneInWay )
			{
				player.Burst( );
			}
		}
		else
		{
			float distFromGoal = GetDistance( PLAYER_POS, Goal_RC );
			bool PlaMovBall = VelAng > PlaAng - ( 1.0f / distFromGoal * 20.0f ) && VelAng < PlaAng + ( 1.0f / distFromGoal * 20.0f );				// Player moving towards ball
			bool PlaMovGoal = LineIntersect( Line( PLAYER_POS, NewPoint( PLAYER_POS, 1000.0f, VelAng ) ), Line( Goal_ART, Goal_ARB ) ) &&			// Player moving towards goal
				LineIntersect( Line( PLAYER_POS, NewPoint( PLAYER_POS, 1000.0f, VelAng ) ), Line( Goal_RT, Goal_RB ) );
			bool ShotAtGoal = LineIntersect( Line( PLAYER_POS, NewPoint( PLAYER_POS, 1000.0f, PlaAng ) ), Line( Goal_ART, Goal_ARB ) ) &&			// Player-ball aligned with goal
				LineIntersect( Line( PLAYER_POS, NewPoint( PLAYER_POS, 1000.0f, PlaAng ) ), Line( Goal_RT, Goal_RB ) );
			bool EneInWay = CircleLineCollision( Line( PLAYER_POS, NewPoint( PLAYER_POS, 1000.0f, VelAng ) ), Circle( ENEMY_POS, PLAYER_RAD ) );	// Enemy in the way
			bool HasBall = GetDistance( PLAYER_POS, BALL_POS ) <= 40.0f;


			if ( !ShotAtGoal && HasBall )
			{


				Vec2 PointInFront = NewPoint( PLAYER_POS, 100.0f, PlaAng );
				Dir::Direction side = SideOfLine( PLAYER_POS, PointInFront, Goal_RC );

				if ( side == Dir::Left )
				{
					direction += 0.5f;
				}
				else if ( side == Dir::Right )
				{
					direction -= 0.5f;
				}
			}

			player.SetThrustVector( VectorSet( 10.0f, direction ) );

			// Burst - If player moves towards the ball, and towards the goal, and the enemy is not in the way
			if ( PlaMovBall && PlaMovGoal && !EneInWay )
			{
				player.Burst( );
			}
		}
	}
private:
	bool MyGoalRight;
};

class LuXAIFactory : public Controller::Factory
{
public:
	LuXAIFactory()
		:
		Factory( L"DerpMaster2000(LuX)" )
	{}
	virtual std::unique_ptr< Controller > Make( ControllablePlayer& player, const ViewableWorld& view ) override
	{
		return std::make_unique< LuXAI >( player, view );
	}
};