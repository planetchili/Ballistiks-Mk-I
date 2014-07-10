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
	inline void AddObserver( std::shared_ptr< Observer > obs )
	{
		observers.push_back( std::weak_ptr< Observer >( obs ) );
	}
	inline void RemoveObserver( std::shared_ptr< Observer > obs )
	{
		for( int i = 0,end = observers.size(); i < end; i++ )
		{
			const auto locked = observers[i].lock();
			if( locked )
			{
				if( locked == obs )
				{
					const auto j = observers.begin() + i;
					observers.erase( j );
					return;
				}
			}
			else
			{
				const auto j = observers.begin() + i;
				observers.erase( j );
				i--;
				end--;
			}
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
		for( int i = 0,end = observers.size(); i < end; i++ )
		{
			const auto locked = observers[i].lock();
			if( locked )
			{
				locked->OnNotify();
			}
			else
			{
				const auto j = observers.begin() + i;
				observers.erase( j );
				i--;
				end--;
			}
		}
	}
private:
	std::vector< std::weak_ptr< Observer > > observers;
	bool enabled = true;
};