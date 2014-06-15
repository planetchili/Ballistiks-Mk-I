#pragma once
#include "Player.h"

class Controller
{
public:
	Controller( ControllablePlayer& player )
		:
		player( player )
	{}
	virtual void Process() = 0;
protected:
	ControllablePlayer& player;
};