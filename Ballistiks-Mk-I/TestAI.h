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

class TestAIFactory : public Controller::Factory
{
public:
	TestAIFactory()
		:
		Factory( "DerpyMcDerpersten" )
	{}
	virtual std::unique_ptr< Controller > Make( ControllablePlayer& player,const ViewableWorld& view ) override
	{
		return std::make_unique< TestAI >( player,view );
	}
};