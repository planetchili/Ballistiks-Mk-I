#pragma once
#include "Vec2.h"
#include "Rect.h"
#include <assert.h>

class PhysicalCircle
{
public:
	inline Vec2 GetCenter( ) const
	{
		return pos;
	}
	inline float GetRadius( ) const
	{
		return radius;
	}
	inline RectF GetAABB( ) const
	{
		return { pos.y - radius,pos.y + radius,pos.x - radius,pos.x + radius };
	}
	inline Vec2 GetVel( ) const
	{
		return vel;
	}
	inline float GetMass( ) const
	{
		return mass;
	}
	inline void Rebound( Vec2 normal )
	{
		vel -= normal * ( vel * normal ) * 2.0f;
	}
	inline void ApplyImpulse( Vec2 j )
	{
		vel += j / mass;
	}
	inline void ApplyForce( Vec2 f )
	{
		force += f;
	}
	bool HandleCollision( PhysicalCircle& other )
	{
		bool collided = false;
		// dot : displacement from other to this
		Vec2 dot = pos - other.GetCenter( );
		if( dot.LenSq( ) < sq( other.GetRadius( ) + radius ) )
		{
			// velocity of other relative to that of this
			const Vec2 vot = other.GetVel( ) - vel;
			// only collide on approach
			if( vot * dot > 0.0f )
			{
				collided = true;
				const float mo = other.GetMass( );
				const float mt = mass;
				const float j = -2.0f * ( vot * dot.Normalize( ) ) * mo * mt / ( mo + mt );
				other.ApplyImpulse( dot * j );
				ApplyImpulse( dot * -j );
			}
		}
		return collided;
	}
	virtual void Update( const float dt )
	{
		vel += ( force / mass ) * dt;
		pos += vel * dt;
		force = { 0.0f,0.0f };
	}
protected:
	PhysicalCircle( float radius,float mass,Vec2 vel = { 0.0f,0.0f },Vec2 pos = { 0.0f,0.0f } )
		:
		radius( radius ),
		pos( pos ),
		mass( mass ),
		vel( vel )
	{}
protected:
	Vec2 pos;
	Vec2 vel;
	Vec2 force = { 0.0f,0.0f };
	float radius;
	float mass;
};