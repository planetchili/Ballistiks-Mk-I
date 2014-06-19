#pragma once
#include "Vec2.h"
#include "Colors.h"
#include "D3DGraphics.h"
#include "dxflib\dl_creationadapter.h"
#include "dxflib\dl_dxf.h"
#include "Mat3.h"
#include "Drawable.h"
#include "PhysicalCircle.h"
#include <vector>
#include <memory>
#include <algorithm>


class PolyClosed
{
private:
	class Loader : public DL_CreationAdapter
	{
	public:
		Loader( std::string filename )
		{
			auto pDxf = std::make_unique< DL_Dxf >( );
			pDxf->in( filename,this );
		}
		virtual void addVertex( const DL_VertexData& data ) override
		{
			vertices.push_back( { (float)data.x,-(float)data.y } );
		}
		operator std::vector< const Vec2 >&&() 
		{
			return std::move( vertices );
		}
	private:
		std::vector< const Vec2 > vertices;
	};
public:
	class Drawable : public ::Drawable
	{
	public:
		Drawable( const PolyClosed& parent,Color color,Mat3 trans )
			:
			::Drawable( trans ),
			parent( parent ),
			color( color )
		{}
		Drawable( const PolyClosed& parent,Color color )
			:
			parent( parent ),
			color( color )
		{}
		virtual void Rasterize( D3DGraphics& gfx ) const override
		{
			for( auto i = parent.vertices.begin( ),end = parent.vertices.end( ) - 1; 
				i != end; i++ )
			{
				gfx.DrawLineClip( trans * *i,trans * *( i + 1 ),color,clip );
			}
			gfx.DrawLineClip( trans * parent.vertices.back( ),
				trans * parent.vertices.front( ),color,clip );
		}
	private:
		const PolyClosed& parent;
		const Color color;
	};
public:
	enum ReboundType
	{
		ReboundInternal,
		ReboundExternal,
		ReboundNone
	};
public:
	PolyClosed( PolyClosed&& poly )
		:
		PolyClosed( std::move( poly.vertices ) )
	{}
	PolyClosed( std::vector< const Vec2 >&& vertices )
		:
		vertices( vertices )
	{
		if( !IsClockwise( ) )
		{
			std::reverse( this->vertices.begin( ),this->vertices.end( ) );
		}
	}
	PolyClosed( std::string filename )
		:
		PolyClosed( Loader( filename ) )
	{}
	Drawable GetDrawable( Color color = WHITE ) const
	{
		return Drawable( *this,color );
	}
	Drawable GetDrawable( Mat3 trans,Color color = WHITE ) const
	{
		return Drawable( *this,color,trans );
	}
	operator const std::vector< const Vec2 >&( )
	{
		return vertices;
	}
	bool HandleCollision( PhysicalCircle& obj,ReboundType dir )
	{
		bool collided = false;

		const RectF objAABB = obj.GetAABB( );
		const Vec2 c = obj.GetCenter( );
		const float r = obj.GetRadius( );

		Vec2 prev = vertices.back( );
		for( auto i = vertices.begin( ),end = vertices.end( ); i < end; i++ )
		{
			const Vec2 cur = *i;
			const RectF lineAABB( prev,cur );
			if( objAABB.Overlaps( lineAABB ) )
			{
				bool velGate = true;
				Vec2 objVel, lineNormal;
				if( dir != ReboundNone )
				{
					objVel = obj.GetVel( );
					lineNormal =
						dir == ReboundInternal ? ( cur - prev ).CW90( ).Normalize( )
										: ( cur - prev ).CCW90().Normalize( );
					velGate = objVel * lineNormal < 0.0f;
				}
				if( velGate )
				{
					const std::vector< Vec2 > points = CircleLineIntersection( c,cur,prev,r );
					if( points.size( ) == 2 )
					{
						const bool cons0 = lineAABB.Contains( points[0] );
						const bool cons1 = lineAABB.Contains( points[1] );

						if( cons0 || cons1 )
						{
							collided = true;
							if( dir != ReboundNone )
							{
								if( !( cons0 && cons1 ) && !lineAABB.Contains( points[0].MidpointWith( points[1] ) ) )
								{
									const Vec2 d1 = c - prev;
									const Vec2 d2 = c - cur;
									const float dSquared1 = d1.LenSq();
									const float dSquared2 = d2.LenSq();
									float dSquaredClosest;
									Vec2 dClosest;
									if( dSquared1 <= dSquared2 )
									{
										dClosest = d1;
										dSquaredClosest = dSquared1;
									}
									else
									{
										dClosest = d2;
										dSquaredClosest = dSquared2;
									}

									if( dClosest * objVel < 0.0f )
									{
										obj.Rebound( dClosest / sqrt( dSquaredClosest ) );
									}
								}
								else
								{
									obj.Rebound( lineNormal );
								}
							}
						}
					}
				}
			}
			prev = cur;
		}
		return collided;
	}
	static PolyClosed GenerateSemicircle( float radius,int nVertices )
	{
		assert( nVertices % 2 != 0 );
		std::vector< const Vec2 > vertices;
		float dTheta = PI / ( nVertices - 1 );
		const int nIterations = nVertices / 2;
		for( int n = 0; n < nIterations; n++ )
		{
			const Vec2 l = { -radius,0.0f };
			const Vec2 r = { radius,0.0f };
			vertices.push_back( l.Rotation( -dTheta * n ) );
			vertices.push_back( r.Rotation( dTheta * n ) );
		}
		vertices.push_back( { 0.0f,radius } );
		return PolyClosed( move( vertices ) );
	}
private:
	bool IsClockwise() const
	{
		float area = 0.0f;
		Vec2 prev = vertices.back();
		for( auto i = vertices.begin( ),end = vertices.end();
			i != end; i++ )
		{
			const Vec2 cur = *i;
			area += ( cur.x - prev.x ) * ( cur.y + prev.y );
			prev = cur;
		}
		return area <= 0.0f;
	}
private:
	std::vector< const Vec2 > vertices;
};