#pragma once
#include "Vec2.h"
#include "Colors.h"
#include "D3DGraphics.h"
#include "dxflib\dl_creationadapter.h"
#include "dxflib\dl_dxf.h"
#include "Mat3.h"
#include "Drawable.h"
#include <vector>
#include <memory>


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
		Drawable( const PolyClosed& parent )
			:
			parent( parent )
		{}
		virtual void Rasterize( D3DGraphics& gfx ) const override
		{
			for( auto i = parent.vertices.begin( ),end = parent.vertices.end( ) - 1; 
				i != end; i++ )
			{
				gfx.DrawLineClip( trans * *i,trans * *( i + 1 ),parent.color,clip );
			}
			gfx.DrawLineClip( trans * parent.vertices.back( ),
				trans * parent.vertices.front( ),parent.color,clip );
		}
	private:
		const PolyClosed& parent;
	};
public:
	PolyClosed( std::initializer_list< Vec2 > vList,Color color = WHITE )
		:
		vertices( vList ),
		color( color )
	{}
	PolyClosed( std::string filename,Color color = WHITE )
		:
		vertices( Loader( filename ) ),
		color( color )
	{}
	Drawable GetDrawable() const
	{
		return Drawable( *this );
	}

private:
	Color color;
	const std::vector< const Vec2 > vertices;
};