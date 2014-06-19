#pragma once
#include <vector>
#include "PolyClosed.h"
#include "CollidableCircle.h"

class AlertZoneObserver
{
public:
	virtual void Notify() = 0;
};

class AlertZone : private PolyClosed
{
public:
	AlertZone( PolyClosed&& poly )
		:
		PolyClosed( poly )
	{}
	AlertZone( const PolyClosed& poly )
		:
		PolyClosed( poly )
	{}
	void AddObserver( AlertZoneObserver& observer )
	{
		observers.push_back( &observer );
	}
	void HandleCollision( PhysicalCircle& obj )
	{
		if( PolyClosed::HandleCollision( obj,PolyClosed::ReboundNone ) )
		{
			for( AlertZoneObserver* const o : observers )
			{
				o->Notify();
			}
		}
	}
private:
	std::vector< AlertZoneObserver* const > observers;
};