#pragma once
#include "Player.h"

class Controller
{
public:
	Controller( ControllablePlayer& player )
		:
		player( player )
	{}
	void Process()
	{
		if( enabled )
		{
			_Process();
		}
	}
	void Enable()
	{
		enabled = true;
	}
	void Disable()
	{
		player.SetThrustVector( { 0.0f,0.0f } );
		enabled = false;
	}
	bool IsEnabled() const
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