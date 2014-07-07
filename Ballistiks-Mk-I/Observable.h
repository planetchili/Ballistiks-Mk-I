#pragma once
#include <vector>
#include <algorithm>

class Observer
{
public:
	virtual void OnNotify() = 0;
};

class Observable
{
public:
	inline void AddObserver( Observer& obs )
	{
		observers.push_back( &obs );
	}
	inline void RemoveObserver( Observer& obs )
	{
		const auto end = observers.end();
		const auto i = std::find( observers.begin(),end,&obs );
		if( i != end )
		{
			observers.erase( i );
		}
	}
	inline void Disable()
	{
		enabled = false;
	}
	inline void Enable()
	{
		enabled = true;
	}
	inline bool IsEnabled() const
	{
		return enabled;
	}
protected:
	inline void Notify()
	{
		for( Observer* const o : observers )
		{
			o->OnNotify();
		}
	}
private:
	std::vector< Observer* const > observers;
	bool enabled = true;
};