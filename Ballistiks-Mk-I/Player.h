#pragma once
#include "PhysicalCircle.h"
#include "Parameters.h"
#include "Drawable.h"

class ControllablePlayer
{
public:
	virtual void SetThrustVector( Vec2 vector ) = 0;
	virtual void Burst() = 0;
	virtual const class Player& PlayerConst() const = 0;
};

class Player : public PhysicalCircle,
			   public ControllablePlayer
{
public:
	class Drawable : public ::Drawable
	{
	public:
		Drawable( const Player& parent )
			:
			::Drawable( Mat3::Translation( parent.pos ) ),
			parent( parent )
		{}
		virtual void Rasterize( D3DGraphics& gfx ) const override
		{
			const float scaledRadius = parent.radius * trans.ExtractScaleIsometric();
			const Vec2 screenPos = trans * Vec2{ 0.0f,0.0f };
			gfx.DrawFilledCircleClip( screenPos,(int)scaledRadius,clip,parent.color1 );
			gfx.DrawFilledCircleClip( screenPos,(int)( scaledRadius * 0.8f ),clip,parent.color0 );
		}
	private:
		const Player& parent;
	};
public:
	Player( Vec2 pos,unsigned int uid,float radius,float density,
		float drag,Color primary,Color secondary )
		:
		PhysicalCircle( radius,density,pos,drag ),
		uid( uid ),
		color0( primary ),
		color1( secondary)
	{}
	Player( Vec2 pos,unsigned int uid,Color primary,Color secondary )
		:
		Player( pos,uid,PLAYER_RADIUS,PLAYER_DENSITY,PLAYER_DRAG,primary,secondary )
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
	virtual void Burst() override
	{
		if( refactoryTimeLeft <= 0.0f && thrustVector != Vec2{0.0f,0.0f} )
		{
			ApplyImpulse( thrustVector.Normalize() * burstImpulse );
			refactoryTimeLeft = refactoryPeriod;
		}
	}
	virtual void Update( const float dt ) override
	{
		ApplyForce( thrustVector * thrustForce );
		refactoryTimeLeft -= dt;
		PhysicalCircle::Update( dt );
	}
	virtual const Player& PlayerConst() const override
	{
		return *this;
	}
	Drawable GetDrawable() const
	{
		return Drawable( *this );
	}
public:	// viewer interface
	inline float GetRefactoryTime() const
	{
		return refactoryTimeLeft;
	}
	inline float GetRefactoryPeriod() const
	{
		return refactoryPeriod;
	}
	inline unsigned int GetUID() const
	{
		return uid;
	}
	inline bool operator==( const Player& rhs ) const
	{
		return uid == rhs.uid;
	}
	inline bool operator!=( const Player& rhs ) const
	{
		return !( *this == rhs );
	}
private:
	const float thrustForce = PLAYER_THRUST;
	const float refactoryPeriod = PLAYER_REFACTORY;
	const float burstImpulse = PLAYER_BURST;
	float refactoryTimeLeft = 0.0f;
	Vec2 thrustVector = { 0.0f,0.0f };
	const Color color0;
	const Color color1;
	const unsigned int uid;
};