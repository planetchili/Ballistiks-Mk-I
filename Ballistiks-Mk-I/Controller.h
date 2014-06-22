#pragma once
#include "Player.h"

class Controller
{
public:
	Controller( ControllablePlayer& player )
		:
		player( player )
	{}
	virtual ~Controller();
	inline void Process()
	{
		if( enabled )
		{
			_Process();
		}
	}
	inline void Enable()
	{
		enabled = true;
	}
	inline void Disable()
	{
		player.SetThrustVector( { 0.0f,0.0f } );
		enabled = false;
	}
	inline bool IsEnabled() const
	{
		return enabled;
	}
protected:
	virtual void _Process() = 0;
protected:
	ControllablePlayer& player;
private:
	bool enabled = true;
};