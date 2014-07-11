/******************************************************************************************
*	Chili DirectX Framework Version 14.03.22											  *
*	Rect.h																				  *
*	Copyright 2014 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once

#include "Vec2.h"
#include <minmax.h>

template < typename T >
class _Rect
{
public:
	inline	_Rect( ) {}
	inline	_Rect( T top,T bottom,T left,T right )
		:
	top( top ),
	bottom( bottom ),
	left( left ),
	right( right )
	{}
	inline	_Rect( _Vec2< T > p0,_Vec2< T > p1 )
		:
		top( min( p0.y,p1.y ) ),
		bottom( max( p0.y,p1.y ) ),
		left( min( p0.x,p1.x ) ),
		right( max( p0.x,p1.x ) )
	{}
	inline	_Rect( const _Rect& rect )
		:
	top( rect.top ),
	bottom( rect.bottom ),
	left( rect.left ),
	right( rect.right )
	{}
	inline	void Translate( _Vec2< T > d )
	{
		Translate( d.x,d.y );
	}
	inline	void Translate( T dx,T dy )
	{
		top += dy;
		bottom += dy;
		left += dx;
		right += dx;
	}
	template < typename T2 >
	inline	operator _Rect< T2 >( ) const
	{
		return { (T2)top,(T2)bottom,(T2)left,(T2)right };
	}
	inline	void ClipTo( const _Rect& rect )
	{
		top = max( top,rect.top );
		bottom = min( bottom,rect.bottom );
		left = max( left,rect.left );
		right = min( right,rect.right );
	}
	inline	T GetWidth( ) const
	{
		return right - left;
	}
	inline	T GetHeight( ) const
	{
		return bottom - top;
	}
	inline	bool Contains( _Vec2< T > p ) const
	{
		// branchless (but is it faster...?)
		return bool( ( p.y >= top ) & ( p.y <= bottom ) & ( p.x >= left ) & ( p.x <= right ) );
	}
	inline	bool Overlaps( _Rect r ) const
	{
		// branchless (but is it faster...? or do I even care?)
		return bool( ( r.bottom > top ) & (r.top < bottom ) & ( r.right > left ) & (r.left < right ) );
	}
	std::vector< const _Vec2< T > > ExtractVertices() const
	{
		std::vector< const _Vec2< T > > vertices;
		vertices.push_back( { left,top } );
		vertices.push_back( { right,top } );
		vertices.push_back( { right,bottom } );
		vertices.push_back( { left,bottom } );
		return vertices;
	}

public:
	T top;
	T bottom;
	T left;
	T right;
};

typedef _Rect< int > RectI;
typedef _Rect< float > RectF;