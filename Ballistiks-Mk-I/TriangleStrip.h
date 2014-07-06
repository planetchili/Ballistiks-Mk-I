#pragma once
#include "PolyClosed.h"
#include "Triangle.h"
#include <assert.h>
#include <functional>
#include <list>

// solid triangle strip
// contains functionality to generate strips from polyclosed
// via offset (wall) or via ear removal (triangularization)
class TriangleStrip
{
public:
	class Drawable : public ::Drawable
	{
	public:
		Drawable( const TriangleStrip& parent,Color color,Mat3 trans ) 
			:
			::Drawable( trans ),
			color( color ),
			vertices( parent.vertices )
		{

		}
		Drawable( const TriangleStrip& parent,Color color )
			:
			color( color ),
			vertices( parent.vertices )
		{}
		virtual void Rasterize( D3DGraphics& gfx ) const override
		{
			for( auto i = vertices.begin(),end = vertices.end() - 2; i != end; i++ )
			{
				gfx.DrawTriangle(
					trans * *i,
					trans * *( i + 1 ),
					trans * *( i + 2 ),
					clip,color );
			}
		}
	private:
		const std::vector< const Vec2 >& vertices;
		const Color color;
	};
public:
	static TriangleStrip ExtractClosedWall( const std::vector< const Vec2 >& lineVerts,float width )
	{
		std::vector< const Vec2 > stripVerts;
		for( auto i = lineVerts.begin(),end = lineVerts.end() - 2; i != end; i++ )
		{
			stripVerts.push_back( *( i + 1 ) );
			const Vec2 pn = ( *( i + 0 ) - *( i + 1 ) ).CW90().Normalize();
			const Vec2 qn = ( *( i + 1 ) - *( i + 2 ) ).CW90().Normalize();
			const Vec2 bn = ( pn + qn ).Normalize();
			const Vec2 v = bn * ( width / ( pn * bn ) );
			stripVerts.push_back( *( i + 1 ) + v );
		}
		// this only covers 1 iteration but we need 2...
		const Vec2 p0 = *( lineVerts.end() - 2 );
		const Vec2 p1 = lineVerts.back();
		const Vec2 p2 = lineVerts[0];
		const Vec2 p3 = lineVerts[1];
		{
			stripVerts.push_back( p1 );
			const Vec2 pn = ( p0 - p1 ).CW90().Normalize();
			const Vec2 qn = ( p1 - p2 ).CW90().Normalize();
			const Vec2 bn = ( pn + qn ).Normalize();
			const Vec2 v = bn * ( width / ( pn * bn ) );
			stripVerts.push_back( p1 + v );
		}
		{
			stripVerts.push_back( p2 );
			const Vec2 pn = ( p1 - p2 ).CW90().Normalize();
			const Vec2 qn = ( p2 - p3 ).CW90().Normalize();
			const Vec2 bn = ( pn + qn ).Normalize();
			const Vec2 v = bn * ( width / ( pn * bn ) );
			stripVerts.push_back( p2 + v );
		}
		stripVerts.push_back( stripVerts[0] );
		stripVerts.push_back( stripVerts[1] );
		return TriangleStrip( std::move( stripVerts ) );
	}
	static std::vector< const TriangleStrip > ExtractSolidStripCollection( 
		const std::vector< const Vec2 >& line )
	{
		// make the list of input vertices
		std::list< const Vec2 > inputVerts( line.begin( ),line.end( ) );
		// make the vector of output triangle strips
		std::vector< const TriangleStrip > strips;
		// keep extracting strips from the vertice list until no more triangles remain
		while( inputVerts.size() > 2 )
		{
			strips.push_back( ExtractSolidStrip( inputVerts ) );
		}
		// return the vector of strips
		return strips;
	}
	static TriangleStrip ExtractSolidStrip( std::list< const Vec2 >& inputVerts )
	{
		// need at least 1 triangle in the set
		assert( inputVerts.size() > 2 );

		// make the output vertice vector
		std::vector< const Vec2 > vertices;
		
		// lambda function to check if vertex is prunable
		auto IsPrunable = [&inputVerts]( std::list< const Vec2 >::const_iterator b ) -> bool
		{
			// initialize iterators to the three vertices of the triangle
			auto a = std::prev( b );
			auto c = std::next( b );
			// return false if not convex to the polygon
			if( !Triangle::IsConvex( *a,*b,*c ) )
			{
				return false;
			}
			// return false if there is a vertex before the triangle inside
			for( auto iPre = inputVerts.cbegin( ); iPre != a; iPre++ )
			{
				if( Triangle::ContainsPoint( *a,*b,*c,*iPre ) )
				{
					return false;
				}
			}
			// return false if there is a vertex after the triangle inside
			for( auto iPost = std::next( c ),end = inputVerts.cend( ); iPost != end; iPost++ )
			{
				if( Triangle::ContainsPoint( *a,*b,*c,*iPost ) )
				{
					return false;
				}
			}
			// if we get this far, vertex pointed to by iterator b is prunable
			return true;
		};

		// range of i such that next and prev always exist
		for( auto end = std::prev( inputVerts.end() ),
			i = std::next( inputVerts.begin() ); i != end; i++ )
		{
			if( IsPrunable( i ) )
			{
				// i is prunable, start the triangle strip
				// add the first triangle
				vertices.push_back( *i );
				vertices.push_back( *std::prev( i ) );
				vertices.push_back( *std::next( i ) );
				// remove vertice i from the list and iterate ccw
				inputVerts.erase( std::next( --i ) );
				// keep adding to strip while there are still triangles left
				while( true )
				{
					// ccw iteration
					// if size is less than 3 (number of vertices needed for triangle)
					int size = inputVerts.size();
					if( size < 3 )
					{
						// quit the while loop
						break;
					}
					// if ccw vertice is at extent
					auto begin = inputVerts.begin();
					if( i == begin )
					{
						// rotate list so that begin is at the middle
						inputVerts.splice( begin,inputVerts,
							std::next( begin,size / 2 + size % 2 ),inputVerts.end() );
					}
					// if ccw vertice is not prunable
					if( !IsPrunable( i ) )
					{
						// quit the while loop
						break;
					}
					else
					{
						// add new vertice (triangle)
						vertices.push_back( *std::prev( i ) );
						// remove vertice i from the list and iterate ccw
						inputVerts.erase( std::prev( ++i ) );
					}

					// cw iteration
					// if size is less than 3 (number of vertices needed for triangle)
					size = inputVerts.size( );
					if( size < 3 )
					{
						// quit the while loop
						break;
					}
					// if cw vertice is at extent
					auto last = std::prev( inputVerts.end( ) );
					if( i == last )
					{
						// wtf bro @size=5
						// rotate list so that last is at the middle
						begin = inputVerts.begin();
						inputVerts.splice( begin,inputVerts,std::next( begin,size / 2 ),inputVerts.end() );
					}
					// if cw vertice is not prunable
					if( !IsPrunable( i ) )
					{
						// quit the while loop
						break;
					}
					else
					{
						// add new vertice (triangle)
						vertices.push_back( *std::next( i ) );
						// remove vertice i from the list and iterate ccw
						inputVerts.erase( std::next( --i ) );
					}
				}
				// quit the for loop
				break;
			}
		}
		// return the triangle strip
		return TriangleStrip( std::move( vertices ) );
	}
public:
	TriangleStrip( const PolyClosed& poly )
		:
		TriangleStrip( (std::vector< const Vec2 >&&)PolyClosed( poly ) )
	{}
	TriangleStrip( TriangleStrip&& strip )
		:
		vertices( std::move( strip.vertices ) )
	{}
	virtual ~TriangleStrip()
	{}
	Drawable GetDrawable( Color color = WHITE ) const
	{
		return Drawable( *this,color );
	}
	Drawable GetDrawable( Mat3 trans,Color color = WHITE ) const
	{
		return Drawable( *this,color,trans );
	}
private:
	TriangleStrip( std::vector< const Vec2 >&& vertices )
		:
		vertices( vertices )
	{}
private:
	std::vector< const Vec2 > vertices;	
};