#pragma once
#include <vector>
#include <algorithm>

class Observable
{
public:
	class Observer
	{
	public:
		virtual void OnNotify() = 0;
	};
public:
	void AddObserver( Observer& obs )
	{
		observers.push_back( &obs );
	}
	void RemoveObserver( Observer& obs )
	{
		const auto end = observers.end();
		const auto i = std::find( observers.begin(),end,&obs );
		if( i != end )
		{
			observers.erase( i );
		}
	}
private:
	std::vector< Observer* const > observers;
};