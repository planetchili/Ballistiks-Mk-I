#pragma once
#include "CollidableCircle.h"

class ControllablePlayer
{
public:
	virtual void SetTrust( Vec2 vector ) = 0;
	virtual const Player& Player() const = 0;
};

class Player : public PhysicalCircle,
			   public ControllablePlayer
{
public:
	virtual void 
};