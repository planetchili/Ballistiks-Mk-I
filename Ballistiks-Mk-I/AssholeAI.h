#pragma once
#include "AI.h"

class AssholeAI : public AI
{
public:
	AssholeAI( ControllablePlayer& player,const ViewableWorld& view )
		:
		AI( player,view )
	{}
protected:
	virtual void _Process() override
	{
		const Player& me = player.PlayerConst();
		const Player& other = [me,this]() -> const Player&
		{
			for( const Player& p : view.GetPlayers() )
			{
				if( p != me )
				{
					return p;
				}
			}
			// should not ever see this... unless <= 1 player in game :?
			assert( false );
			return me;
		}();
		
		const Vec2 d = other.GetCenter() - me.GetCenter();
		const float len = d.Len();
		player.SetThrustVector( d / len );
		if( len < 2.0f * me.GetRadius() + 2.0f )
		{
			player.Burst();
		}
	}
};

class AssholeAIFactory : public Controller::Factory
{
public:
	AssholeAIFactory()
		:
		Factory( L"SomeAsshole" )
	{}
	virtual std::unique_ptr< Controller > Make( ControllablePlayer& player,const ViewableWorld& view ) override
	{
		return std::make_unique< AssholeAI >( player,view );
	}
};