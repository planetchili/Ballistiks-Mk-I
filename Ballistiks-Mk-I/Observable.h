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
protected:
	void Notify()
	{
		for( Observer* const o : observers )
		{
			o->OnNotify();
		}
	}
private:
	std::vector< Observer* const > observers;
};