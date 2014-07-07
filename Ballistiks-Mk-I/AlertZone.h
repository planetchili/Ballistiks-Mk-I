#pragma once
#include <vector>
#include "PolyClosed.h"
#include "PhysicalCircle.h"
#include "Observable.h"

class AlertZone : public Observable
{
public:
	enum Type
	{
		Continuous,
		OneShot
	};
public:
	AlertZone( PolyClosed&& poly,Type type = OneShot  )
		:
		poly( poly ),
		type( type )
	{}
	AlertZone( const PolyClosed& poly,Type type = OneShot )
		:
		poly( poly ),
		type( type )
	{}
	void HandleCollision( PhysicalCircle& obj )
	{
		if( IsEnabled() && poly.HandleCollision( obj,PolyClosed::ReboundNone ) )
		{
			Notify();
			if( type == OneShot )
			{
				Disable();
			}
		}
	}
private:
	const Type type;
	PolyClosed poly;
};