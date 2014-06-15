#pragma once
#include "CollidableCircle.h"

class DragProcessor
{
public:
	DragProcessor( float coefficient )
		:
		dragCoefficient( coefficient )
	{}
	void ProcessDrag( PhysicalCircle& obj ) const
	{
		obj.ApplyForce( -obj.GetVel().Normalize() * 
			dragCoefficient * sq( obj.GetVel() ) * obj.GetRadius() );
	}
private:
	const float dragCoefficient;
};