#pragma once
#include "PolyClosed.h"
#include "TriangleStrip.h"

class Walls
{
public:
	Walls( std::vector< const Vec2 >&& vertices )
		:
		poly( std::move( vertices ) ),
		strip( TriangleStrip::ExtractClosedWall( this->poly,WALL_WIDTH ) )
	{}
	Walls( const PolyClosed& poly )
		:
		poly( poly ),
		strip( TriangleStrip::ExtractClosedWall( this->poly,WALL_WIDTH ) )
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
	const Color color = WHITE;
};