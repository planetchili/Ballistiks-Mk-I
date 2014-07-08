#pragma once
#include "Player.h"
#include "ViewableWorld.h"

class Controller
{
public:
	class Factory
	{
	public:
		Factory( const std::wstring author )
			:
			author( author )
		{}
		virtual std::unique_ptr< Controller > Make( ControllablePlayer& player,const ViewableWorld& view ) = 0;
		virtual std::unique_ptr< Controller > Make( ControllablePlayer& player,const ViewableWorld& view,unsigned int parameter )
		{
			assert( "This factory does not support a make parameter." && false );
			return { nullptr };
		}
		virtual ~Factory() {}
		const std::wstring& GetName() const
		{
			return name;
		}
	private:
		const std::wstring name;
	};
public:
	Controller( ControllablePlayer& player )
		:
		player( player )
	{}
	virtual ~Controller() {}
	inline void Process()
	{
		if( enabled )
		{
			_Process();
		}
	}
	inline void Enable()
	{
		enabled = true;
	}
	inline void Disable()
	{
		player.SetThrustVector( { 0.0f,0.0f } );
		enabled = false;
	}
	inline bool IsEnabled() const
	{
		return enabled;
	}
protected:
	virtual void _Process() = 0;
protected:
	ControllablePlayer& player;
private:
	bool enabled = true;
};