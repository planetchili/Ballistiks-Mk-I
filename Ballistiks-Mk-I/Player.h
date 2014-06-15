#pragma once
#include "CollidableCircle.h"

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
	Player( Vec2 pos,float radius,float mass )
		:
		PhysicalCircle( radius,mass,{ 0.0f,0.0f },pos )
	{}
	Player( Vec2 pos,float radius )
		:
		Player( pos,radius,sq( radius ) / 250.0f )
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
	const float thrustForce = 1.0f;
	Vec2 thrustVector = { 0.0f,0.0f };
};