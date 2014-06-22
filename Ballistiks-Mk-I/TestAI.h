#pragma once
#include "AI.h"

class TestAI : public AI
{
public:
	TestAI( ControllablePlayer& player,const ViewableWorld& view )
		:
		AI( player,view )
	{}
protected:
	virtual void _Process() override
	{
		player.SetThrustVector(
			( view.GetBalls().front().GetCenter() - player.PlayerConst().GetCenter() ).Normalize() );
	}
};