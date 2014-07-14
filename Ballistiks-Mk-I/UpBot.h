#pragma once
#include "AI.h"

class UpBot : public AI
{
public:
	UpBot( ControllablePlayer& player,const ViewableWorld& view )
		:
		AI( player,view )
	{}
protected:
	virtual void _Process() override
	{
		player.SetThrustVector(
		{ 0.0f,-1.0f } );
	}
};

class UpBotFactory : public Controller::Factory
{
public:
	UpBotFactory()
		:
		Factory( L"UpBot" )
	{}
	virtual std::unique_ptr< Controller > Make( ControllablePlayer& player,const ViewableWorld& view ) override
	{
		return std::make_unique< UpBot >( player,view );
	}
};