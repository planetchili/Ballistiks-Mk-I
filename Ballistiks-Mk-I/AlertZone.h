#pragma once
#include <vector>
#include "PolyClosed.h"
#include "PhysicalCircle.h"


class AlertZone
{
public:
	class Observer
	{
	public:
		virtual void Notify() = 0;
	};
public:
	AlertZone( PolyClosed&& poly )
		:
		poly( poly )
	{}
	AlertZone( const PolyClosed& poly )
		:
		poly( poly )
	{}
	void AddObserver( Observer& observer )
	{
		observers.push_back( &observer );
	}
	void HandleCollision( PhysicalCircle& obj )
	{
		if( poly.HandleCollision( obj,PolyClosed::ReboundNone ) )
		{
			for( Observer* const o : observers )
			{
				o->Notify();
			}
		}
	}
private:
	PolyClosed poly;
	std::vector< Observer* const > observers;
};