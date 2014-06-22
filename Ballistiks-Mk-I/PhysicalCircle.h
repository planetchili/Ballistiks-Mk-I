#pragma once
#include "Vec2.h"
#include "Rect.h"
#include <assert.h>

class PhysicalCircle
{
public: // view interface
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
	inline float GetDragCoefficient() const
	{
		return dragCoefficient;
	}
public:
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
	PhysicalCircle( float radius,float density,Vec2 pos,float dragCoefficient = 1.0f,Vec2 vel = { 0.0f,0.0f })
		:
		radius( radius ),
		pos( pos ),
		density( density ),
		mass( PI * sq( radius ) * density ),
		vel( vel ),
		dragCoefficient( dragCoefficient )
	{}
	~PhysicalCircle();
protected:
	Vec2 pos;
	Vec2 vel;
	Vec2 force = { 0.0f,0.0f };
	const float radius;
	const float density;
	const float mass;
	const float dragCoefficient;
};