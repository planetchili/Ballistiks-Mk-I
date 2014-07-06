#pragma once
#include "DrawTarget.h"
#include "Vec2.h"

class Camera : public DrawTarget
{
public:
	Camera( DrawTarget& next,float width,float height,Vec2 pos = { 0.0f,0.0f } )
		:
		next( next ),
		pos( pos ),
		toCenter( { width / 2.0f,height / 2.0f } )
	{}
	virtual void Draw( Drawable& obj ) override
	{
		obj.Transform( Mat3::Translation( toCenter ) * Mat3::Scaling( zoom ) * Mat3::Translation( -pos )  );
		next.Draw( obj );
	}
	void MoveTo( Vec2 newPos )
	{
		pos = newPos;
	}
	float GetZoom() const
	{
		return zoom;
	}
	void SetZoom( float zoom )
	{
		this->zoom = zoom;
	}
	void Zoom( float factor )
	{
		zoom *= factor;
	}

private:
	DrawTarget& next;
	float zoom = 1.0f;
	Vec2 pos;
	Vec2 toCenter;
};