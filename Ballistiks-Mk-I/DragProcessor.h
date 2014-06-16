#pragma once
#include "CollidableCircle.h"

class DragProcessor
{
public:
	DragProcessor( float coefficient = GLOBAL_DRAG )
		:
		dragCoefficient( coefficient )
	{}
	void ProcessDrag( PhysicalCircle& obj ) const
	{
		if( obj.GetVel() != Vec2{0.0f,0.0f} )
		{
			obj.ApplyForce( -obj.GetVel().Normalize() *
				dragCoefficient * obj.GetDragCoefficient() * 
				sq( obj.GetVel() ) * obj.GetRadius() );
		}
	}
private:
	const float dragCoefficient;
};