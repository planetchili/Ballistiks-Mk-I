#pragma once
#include "CollidableCircle.h"

class Ball : public PhysicalCircle
{
public:
	Ball( Vec2 pos )
		:
		PhysicalCircle( BALL_RADIUS,BALL_DENSITY,pos,BALL_DRAG )
	{}
};