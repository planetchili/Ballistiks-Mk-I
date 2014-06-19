#pragma once
#include <vector>
#include "PolyClosed.h"
#include "PhysicalCircle.h"

class AlertZoneObserver
{
public:
	virtual void Notify() = 0;
};

class AlertZone
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
	void AddObserver( AlertZoneObserver& observer )
	{
		observers.push_back( &observer );
	}
	void HandleCollision( PhysicalCircle& obj )
	{
		if( poly.HandleCollision( obj,PolyClosed::ReboundNone ) )
		{
			for( AlertZoneObserver* const o : observers )
			{
				o->Notify();
			}
		}
	}
private:
	PolyClosed poly;
	std::vector< AlertZoneObserver* const > observers;
};