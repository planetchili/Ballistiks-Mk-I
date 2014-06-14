#pragma once
#include <assert.h>

class CollidableCircle
{
public:
	inline RectF GetAABB() const
	{
		const Vec2 c = GetCenter();
		const float r = GetRadius();
		return { c.y - r,c.y + r,c.x - r,c.x + r };
	}
	inline Vec2 GetCenter() const
	{
		return pos;
	}
	inline float GetRadius() const
	{
		return radius;
	}
	virtual Vec2 GetVel() const = 0;
	virtual float GetMass() const = 0;
	virtual void Rebound( Vec2 normal ) = 0;
	virtual void ApplyImpulse( Vec2 j ) = 0;
protected:
	CollidableCircle( float radius,Vec2 pos = { 0.0f,0.0f } )
		:
		pos( pos ),
		radius( radius )
	{}
protected:
	Vec2 pos;
	float radius;
};

class PhysicalCircle : public CollidableCircle
{
public:
	virtual Vec2 GetVel() const override
	{
		return vel;
	}
	virtual float GetMass() const override
	{
		return mass;
	}
	virtual void Rebound( Vec2 normal ) override
	{
		vel -= normal * ( vel * normal ) * 2.0f;
	}
	virtual void ApplyImpulse( Vec2 j ) override
	{
		vel += j / mass;
	}
protected:
	PhysicalCircle( float radius,float mass,Vec2 vel = { 0.0f,0.0f },Vec2 pos = { 0.0f,0.0f } )
		:
		CollidableCircle( radius,pos ),
		mass( mass ),
		vel( vel )
	{}
protected:
	Vec2 vel;
	float mass;
};

class LogicalCircle : public CollidableCircle
{
public:
	virtual Vec2 GetVel() const override
	{
		_wassert( L"Cannot get velocity from LogicalCircle",_CRT_WIDE( __FILE__ ),__LINE__ );
	}
	virtual float GetMass() const override
	{
		_wassert( L"Cannot get mass from LogicalCircle",_CRT_WIDE( __FILE__ ),__LINE__ );
	}
	virtual void Rebound( Vec2 normal ) override
	{
		_wassert( L"Cannot rebound LogicalCircle",_CRT_WIDE( __FILE__ ),__LINE__ );
	}
	virtual void ApplyImpulse( Vec2 j ) override
	{
		_wassert( L"Cannot apply impulse to LogicalCircle",_CRT_WIDE( __FILE__ ),__LINE__ );
	}
protected:
	LogicalCircle( float radius,Vec2 pos = { 0.0f,0.0f } )
		:
		CollidableCircle( radius,pos )
	{}
};