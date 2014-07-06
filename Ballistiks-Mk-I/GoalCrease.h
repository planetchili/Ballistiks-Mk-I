#pragma once
#include "PolyClosed.h"

class GoalCrease
{
public:
	GoalCrease( bool isRightFacing,Vec2 pos,float radius )
		:
		poly( PolyClosed::GenerateSemicircle( 
			radius,
			31,
			Mat3::Translation( pos ) * 
			Mat3::Rotation( isRightFacing ? -PI / 2.0f : PI / 2.0f ) ) )
	{}
	void HandleCollision( PhysicalCircle& obj )
	{
		poly.HandleCollision( obj,PolyClosed::ReboundExternal );
	}
	PolyClosed::Drawable GetDrawable() const
	{
		return poly.GetDrawable( color );
	}
private:
	PolyClosed poly;
	const Color color = WHITE;
};