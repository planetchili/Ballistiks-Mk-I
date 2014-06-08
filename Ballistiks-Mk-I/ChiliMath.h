/******************************************************************************************
*	Chili DirectX Framework Version 14.03.22											  *
*	ChiliMath.h																			  *
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
#include <math.h>
#include <vector>
#include <assert.h>

#define PI (float)M_PI

template <typename T>
T sgn( T val )
{
	return (T)( (T)0 <= val ) - ( val < (T)0 );
}

template <typename T>
T sq( T val )
{
	return val * val;
}

// !assumes at least one real root exists!
template <typename T>
std::vector< T >&& quadratic_roots( T a,T b,T c )
{
	const T intermediate = sqrt( sq( b ) - (T)4 * a * c );
	std::vector< T > roots;
	roots.push_back( ( -b + intermediate ) / ( (T)2 * a ) );
	roots.push_back( ( -b - intermediate ) / ( (T)2 * a ) );
	return std::move( roots );
}

inline unsigned int factorial( unsigned int n )
{
	unsigned int result = 1;
	for( ; n > 1; n-- )
	{
		result *= n;
	}
	return result;
}

inline unsigned int choose( unsigned int n,unsigned int k )
{
	assert( n >= k );
	unsigned int num = 1;
	for( ; n > k; n-- )
	{
		num *= n;
	}
	return num / factorial( n - k );
}

template <typename T>
T gaussian( T x,T o )
{
	return ( ( T )1.0 / sqrt( ( T )2.0 * PI * sq( o ) ) ) 
		* exp( -sq( x ) / ( ( T )2.0 * sq( o ) ) );
}