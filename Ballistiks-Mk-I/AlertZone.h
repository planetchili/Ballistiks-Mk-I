#pragma once
#include <vector>
#include "PolyClosed.h"
#include "PhysicalCircle.h"
#include "Observable.h"

class AlertZone : public Observable
{
public:
	AlertZone( PolyClosed&& poly )
		:
		poly( poly )
	{}
	AlertZone( const PolyClosed& poly )
		:
		poly( poly )
	{}
	void HandleCollision( PhysicalCircle& obj )
	{
		if( poly.HandleCollision( obj,PolyClosed::ReboundNone ) )
		{
			Notify();
		}
	}
private:
	PolyClosed poly;
};