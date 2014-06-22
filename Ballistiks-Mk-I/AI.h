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
		Factory( const ViewableWorld& view )
			:
			view( view )
		{}
		virtual std::unique_ptr< AI > Make( ControllablePlayer& player ) = 0;
		virtual ~Factory() {}
	protected:
		const ViewableWorld& view;
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