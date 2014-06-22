#pragma once
#include <memory>
#include "ViewableWorld.h"
#include "Controller.h"

class AI : public Controller
{
public:
	class Factory
	{
	public:
		virtual std::unique_ptr< AI > Make() const = 0;
	};
public:
	AI( ControllablePlayer& player,const ViewableWorld& view )
		:
		Controller( player ),
		view( view )
	{}
protected:
	const ViewableWorld& view;
};