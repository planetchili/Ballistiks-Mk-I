#pragma once
#include "Player.h"
#include "Ball.h"
#include <vector>

class ViewableWorld
{
public: // view interface
	inline virtual const std::vector< Player >& GetPlayers() const = 0;
	inline virtual const std::vector< Ball >& GetBalls() const = 0;
	inline virtual float GetTimeElapsed() const = 0;
};