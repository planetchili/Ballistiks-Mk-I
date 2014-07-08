#pragma once

#include "Colors.h"
#include "Font.h"
#include <gdiplus.h>
#include <string>

class Surface
{
public:
	enum Alignment
	{
		Left,
		Right,
		Center
	};
public:
	Surface( unsigned int width,unsigned int height,unsigned int pitch )
		:
		buffer( new Color[ pitch * height ] ),
		width( width ),
		height( height ),
		pitch( pitch ),
		bitmap( width,height,pitch * sizeof( Color ),
			PixelFormat32bppARGB,(BYTE*)buffer ),
		g( &bitmap )
	{}
	Surface( unsigned int width,unsigned int height ) // 16-byte alignment
		:
		Surface( width,height,
			// add padding to row ends so that surface is 16-byte aligned
			width + ( 4 - width % 4 ) % 4 )
	{}
	~Surface()
	{
		delete [] buffer;
	}
	void Save( const std::wstring& filename )
	{
		auto GetEncoderClsid = []( const WCHAR* const format,CLSID* const pClsid ) -> int
		{
			UINT  num = 0;          // number of image encoders
			UINT  size = 0;         // size of the image encoder array in bytes

			Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

			Gdiplus::GetImageEncodersSize( &num,&size );
			if( size == 0 )
				return -1;  // Failure

			pImageCodecInfo = ( Gdiplus::ImageCodecInfo* )( malloc( size ) );
			if( pImageCodecInfo == nullptr )
				return -1;  // Failure

			GetImageEncoders( num,size,pImageCodecInfo );

			for( UINT j = 0; j < num; ++j )
			{
				if( wcscmp( pImageCodecInfo[j].MimeType,format ) == 0 )
				{
					*pClsid = pImageCodecInfo[j].Clsid;
					free( pImageCodecInfo );
					return j;  // Success
				}
			}

			free( pImageCodecInfo );
			return -1;  // Failure
		};

		// get encoder class ID information for .bmp format
		CLSID bmpID;
		GetEncoderClsid( L"image/bmp",&bmpID );
		// save bitmap as windows .bmp
		bitmap.Save( filename.c_str(),&bmpID,nullptr );
	}
	inline void Clear( Color fillValue  )
	{
		memset( buffer,fillValue,pitch * height * sizeof( Color ) );
	}
	inline void Present( const unsigned int height,const unsigned int width,
		const unsigned int pitch,BYTE* const buffer ) const
	{
		for( unsigned int y = 0; y < height; y++ )
		{
			memcpy( &buffer[pitch * y],&(this->buffer)[this->pitch * y],sizeof(Color)* width );
		}
	}
	inline void PutPixel( unsigned int x,unsigned int y,Color c )
	{
		buffer[y * pitch + x] = c;
	}
	inline Color GetPixel( unsigned int x,unsigned int y ) const
	{
		return buffer[y * pitch + x];
	}
	inline void DrawString( const std::wstring& string,Vec2 pt,const Font& font,Color c )
	{
		Gdiplus::Color textColor( c.r,c.g,c.b );
		Gdiplus::SolidBrush textBrush( textColor );
		g.DrawString( string.c_str(),-1,font,
			Gdiplus::PointF( pt.x,pt.y ),&textBrush );
	}
	inline void DrawString( const std::wstring& string,const RectF& rect,const Font& font,
		Color c = WHITE,Alignment a = Center )
	{
		Gdiplus::StringFormat format;
		switch( a )
		{
		case Left:
			format.SetAlignment( Gdiplus::StringAlignmentNear );
			break;
		case Right:
			format.SetAlignment( Gdiplus::StringAlignmentFar );
			break;
		case Center:
		default:
			format.SetAlignment( Gdiplus::StringAlignmentCenter );
			break;
		}
		Gdiplus::Color textColor( c.r,c.g,c.b );
		Gdiplus::SolidBrush textBrush( textColor );
		g.DrawString( string.c_str(),-1,font,
			Gdiplus::RectF( rect.left,rect.top,rect.GetWidth(),rect.GetHeight() ),
			&format,
			&textBrush );
	}
	inline unsigned int GetWidth() const
	{
		return width;
	}
	inline unsigned int GetHeight() const
	{
		return height;
	}
	inline unsigned int GetPitch() const
	{
		return pitch;
	}
	inline Color* const GetBuffer()
	{
		return buffer;
	}
	inline const Color* const GetBufferConst() const
	{
		return buffer;
	}
private:
	Color* const buffer;
	const unsigned int width;
	const unsigned int height;
	const unsigned int pitch;
	Gdiplus::Bitmap	bitmap;
	Gdiplus::Graphics g;
};