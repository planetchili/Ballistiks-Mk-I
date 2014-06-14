#pragma once
#include "Vec2.h"

class Triangle
{
public:
	template< typename T >
	static inline bool IsConvex( _Vec2<T> v0,_Vec2<T> v1,_Vec2<T> v2 )
	{
		return ( v1 - v0 ).Cross( v2 - v1 ) >= ( T )0.0;
	}
	template< typename T >
	static inline bool ContainsPoint( _Vec2<T> v0,_Vec2<T> v1,_Vec2<T> v2,_Vec2<T> p )
	{
		auto IsSameSide = [p]( _Vec2<T> a,_Vec2<T> b,_Vec2<T> c ) -> bool
		{
			const _Vec2<T> ab = b - a;
			return ab.Cross( p - a ) * ab.Cross( c - a ) >= ( T )0.0;
		};
		return IsSameSide( v0,v1,v2 ) && IsSameSide( v1,v2,v0 )
			&& IsSameSide( v2,v0,v1 );
	}
private:
	Triangle() = delete;
};