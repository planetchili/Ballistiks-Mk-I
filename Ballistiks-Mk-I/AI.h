#pragma once
#include "ViewableWorld.h"
#include "Controller.h"

class AI : public Controller
{
public:
	AI( ControllablePlayer& player,const ViewableWorld& view )
		:
		Controller( player ),
		view( view )
	{}
protected:
	const ViewableWorld& view;
};