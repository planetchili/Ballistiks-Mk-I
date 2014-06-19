#pragma once
#include "PolyClosed.h"
#include "TriangleStrip.h"

class Walls
{
public:
	Walls( PolyClosed&& poly )
		:
		poly( poly ),
		strip( this->poly )
	{}
	Walls( const PolyClosed& poly )
		:
		poly( poly ),
		strip( poly )
	{}
	void HandleCollision( PhysicalCircle& obj )
	{
		poly.HandleCollision( obj,PolyClosed::ReboundInternal );
	}
	TriangleStrip::Drawable GetDrawable() const
	{
		return strip.GetDrawable( color );
	}
private:
	PolyClosed poly;
	TriangleStrip strip;
	const Color color = GRAY;
};