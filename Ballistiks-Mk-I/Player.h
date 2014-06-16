#pragma once
#include "CollidableCircle.h"
#include "Parameters.h"

class ControllablePlayer
{
public:
	virtual void SetThrustVector( Vec2 vector ) = 0;
	virtual const class Player& PlayerConst() const = 0;
};

class Player : public PhysicalCircle,
			   public ControllablePlayer
{
public:
	Player( Vec2 pos,float radius,float density )
		:
		PhysicalCircle( radius,density,pos )
	{}
	Player( Vec2 pos )
		:
		PhysicalCircle( PLAYER_RADIUS,PLAYER_DENSITY,pos,PLAYER_DRAG )
	{}
	virtual void SetThrustVector( Vec2 vector ) override
	{
		const float length = vector.Len();
		if( length > 1.0f )
		{
			vector /= length;
		}
		thrustVector = vector;
	}
	virtual void Update( const float dt ) override
	{
		ApplyForce( thrustVector * thrustForce );
		PhysicalCircle::Update( dt );
	}
	virtual const Player& PlayerConst() const override
	{
		return *this;
	}
private:
	const float thrustForce = PLAYER_THRUST;
	Vec2 thrustVector = { 0.0f,0.0f };
};