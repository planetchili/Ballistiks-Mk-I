#pragma once
#include <memory>
#include <string>
#include "ViewableWorld.h"
#include "Controller.h"

class AI : public Controller
{
public:
	class Factory
	{
	public:
		Factory( const std::string author )
			:
			author( author )
		{}
		virtual std::unique_ptr< AI > Make( ControllablePlayer& player,const ViewableWorld& view ) = 0;
		virtual ~Factory() {}
		const std::string& GetAuthor() const
		{
			return author;
		}
	private:
		const std::string author;
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