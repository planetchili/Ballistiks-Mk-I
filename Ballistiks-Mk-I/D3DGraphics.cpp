/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	D3DGraphics.cpp																		  *
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
#include "D3DGraphics.h"
#include "ChiliMath.h"
#include <assert.h>
#include <functional>
#pragma comment( lib,"d3d9.lib" )

D3DGraphics::D3DGraphics( HWND hWnd )
	:
pDirect3D( nullptr ),
pDevice( nullptr ),
pBackBuffer( nullptr ),
sysBuffer( SCREENWIDTH,SCREENHEIGHT )
{
	HRESULT result;
	
	pDirect3D = Direct3DCreate9( D3D_SDK_VERSION );
	assert( pDirect3D != nullptr );

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp,sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    result = pDirect3D->CreateDevice( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,&d3dpp,&pDevice );
	assert( !FAILED( result ) );

	result = pDevice->GetBackBuffer( 0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer );
	assert( !FAILED( result ) );
}

D3DGraphics::~D3DGraphics()
{
	if( pDevice )
	{
		pDevice->Release();
		pDevice = nullptr;
	}
	if( pDirect3D )
	{
		pDirect3D->Release();
		pDirect3D = nullptr;
	}
	if( pBackBuffer )
	{
		pBackBuffer->Release();
		pBackBuffer = nullptr;
	}
}

void D3DGraphics::BeginFrame()
{
	sysBuffer.Clear( FILLVALUE );
}

void D3DGraphics::EndFrame()
{
	HRESULT result;
	D3DLOCKED_RECT backRect;

	result = pBackBuffer->LockRect( &backRect,nullptr,0 );
	assert( !FAILED( result ) );


	sysBuffer.Present( SCREENHEIGHT,SCREENWIDTH,backRect.Pitch,(BYTE*)backRect.pBits );


	result = pBackBuffer->UnlockRect( );
	assert( !FAILED( result ) );

	result = pDevice->Present( nullptr,nullptr,nullptr,nullptr );
	assert( !FAILED( result ) );
}

void D3DGraphics::PutPixel( int x,int y,Color c )
{	
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < SCREENWIDTH );
	assert( y < SCREENHEIGHT );
	sysBuffer.PutPixel( x,y,c );
}

Color D3DGraphics::GetPixel( int x,int y ) const
{
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < SCREENWIDTH );
	assert( y < SCREENHEIGHT );
	return sysBuffer.GetPixel( x,y );
}

void D3DGraphics::PutPixelAlpha( int x,int y,Color c )
{
	// load source pixel
	const Color d = GetPixel( x,y );

	// blend channels
	const unsigned char rsltRed = ( c.r * c.x + d.r * ( 255 - c.x ) ) / 255;
	const unsigned char rsltGreen = ( c.g * c.x + d.g * ( 255 - c.x ) ) / 255;
	const unsigned char rsltBlue = ( c.b * c.x + d.b * ( 255 - c.x ) ) / 255;

	// pack channels back into pixel and fire pixel onto backbuffer
	PutPixel( x,y,{ rsltRed,rsltGreen,rsltBlue } );
}

void D3DGraphics::DrawLine( int x0,int y0,int x1,int y1,Color c )
{	
	const int dx = x1 - x0;
	const int dy = y1 - y0;

	if( dy == 0 && dx == 0 )
	{
		PutPixel( x0,y0,c );
	}
	else if( abs( dy ) > abs( dx ) )
	{
		if( dy < 0 )
		{
			int temp = x0;
			x0 = x1;
			x1 = temp;
			temp = y0;
			y0 = y1;
			y1 = temp;
		}
		const float m = (float)dx / (float)dy;
		const float b = x0 - m*y0;
		for( int y = y0; y <= y1; y = y + 1 )
		{
			int x = (int)(m*y + b + 0.5f);
			PutPixel( x,y,c );
		}
	}
	else
	{
		if( dx < 0 )
		{
			int temp = x0;
			x0 = x1;
			x1 = temp;
			temp = y0;
			y0 = y1;
			y1 = temp;
		}
		const float m = (float)dy / (float)dx;
		const float b = y0 - m*x0;
		for( int x = x0; x <= x1; x = x + 1 )
		{
			int y = (int)(m*x + b + 0.5f);
			PutPixel( x,y,c );
		}
	}
}

void D3DGraphics::DrawLineClip( Vec2 p0,Vec2 p1,Color color,const RectF& clip )
{
	enum OutCode
	{
		INSIDE = 0, // 0000
		LEFT = 1,   // 0001
		RIGHT = 2,  // 0010
		BOTTOM = 4, // 0100
		TOP = 8     // 1000
	};

	const std::function< OutCode( float,float ) > ComputeOutCode =
		[&clip]( float x,float y ) -> OutCode
	{
		OutCode code = INSIDE;   // initialised as being inside of clip window

		if( x < clip.left )           // to the left of clip window
			code = (OutCode)( code | LEFT );
		else if( x > clip.right )      // to the right of clip window
			code = (OutCode)( code | RIGHT );
		if( y < clip.top )           // below the clip window
			code = (OutCode)( code | BOTTOM );
		else if( y > clip.bottom )      // above the clip window
			code = (OutCode)( code | TOP );

		return code;
	};

	// compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
	OutCode outcode0 = ComputeOutCode( p0.x,p0.y );
	OutCode outcode1 = ComputeOutCode( p1.x,p1.y );
	bool accept = false;

	while( true )
	{
		if( !( outcode0 | outcode1 ) )
		{ // Bitwise OR is 0. Trivially accept and get out of loop
			accept = true;
			break;
		}
		else if( outcode0 & outcode1 )
		{ // Bitwise AND is not 0. Trivially reject and get out of loop
			break;
		}
		else
		{
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			float x,y;

			// At least one endpoint is outside the clip rectangle; pick it.
			OutCode outcodeOut = outcode0 ? outcode0 : outcode1;

			// Now find the intersection point;
			// use formulas y = p0.y + slope * (x - p0.x), x = p0.x + (1 / slope) * (y - p0.y)
			if( outcodeOut & TOP )
			{           // point is above the clip rectangle
				x = p0.x + ( p1.x - p0.x ) * ( clip.bottom - p0.y ) / ( p1.y - p0.y );
				y = clip.bottom;
			}
			else if( outcodeOut & BOTTOM )
			{ // point is below the clip rectangle
				x = p0.x + ( p1.x - p0.x ) * ( clip.top - p0.y ) / ( p1.y - p0.y );
				y = clip.top;
			}
			else if( outcodeOut & RIGHT )
			{  // point is to the right of clip rectangle
				y = p0.y + ( p1.y - p0.y ) * ( clip.right - p0.x ) / ( p1.x - p0.x );
				x = clip.right;
			}
			else if( outcodeOut & LEFT )
			{   // point is to the left of clip rectangle
				y = p0.y + ( p1.y - p0.y ) * ( clip.left - p0.x ) / ( p1.x - p0.x );
				x = clip.left;
			}

			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
			if( outcodeOut == outcode0 )
			{
				p0.x = x;
				p0.y = y;
				outcode0 = ComputeOutCode( p0.x,p0.y );
			}
			else
			{
				p1.x = x;
				p1.y = y;
				outcode1 = ComputeOutCode( p1.x,p1.y );
			}
		}
	}
	if( accept )
	{
		DrawLine( p0,p1,color );
	}
}

void D3DGraphics::DrawCircle( int centerX,int centerY,int radius,Color color )
{
	int x = radius,y = 0;
	int radiusError = 1 - x;
	while( x >= y )
	{
		PutPixel( centerX + x,centerY + y,color );
		PutPixel( centerX - x,centerY + y,color );
		PutPixel( centerX + x,centerY - y,color );
		PutPixel( centerX - x,centerY - y,color );
		PutPixel( centerX + y,centerY + x,color );
		PutPixel( centerX + y,centerY - x,color );
		PutPixel( centerX - y,centerY + x,color );
		PutPixel( centerX - y,centerY - x,color );

		y++;

		if( radiusError < 0 )
		{
			radiusError += 2 * y + 1;
		}
		else
		{
			x--;
			radiusError += 2 * ( y - x + 1 );
		}
	}
}

void D3DGraphics::DrawCircleClip( int centerX,int centerY,int radius,const RectI& clip,Color color )
{
	int x = radius,y = 0;
	int radiusError = 1 - x;
	while( x >= y )
	{
		Vei2 pout = { centerX + x,centerY + y };
		if( clip.Contains( pout ) )
			PutPixel( pout,color );
		pout = { centerX - x,centerY + y };
		if( clip.Contains( pout ) )
			PutPixel( pout,color );
		pout = { centerX + x,centerY - y };
		if( clip.Contains( pout ) )
			PutPixel( pout,color );
		pout = { centerX - x,centerY - y };
		if( clip.Contains( pout ) )
			PutPixel( pout,color );
		pout = { centerX + y,centerY + x };
		if( clip.Contains( pout ) )
			PutPixel( pout,color );
		pout = { centerX + y,centerY - x };
		if( clip.Contains( pout ) )
			PutPixel( pout,color );
		pout = { centerX - y,centerY + x };
		if( clip.Contains( pout ) )
			PutPixel( pout,color );
		pout = { centerX - y,centerY - x };
		if( clip.Contains( pout ) )
			PutPixel( pout,color );

		y++;

		if( radiusError < 0 )
		{
			radiusError += 2 * y + 1;
		}
		else
		{
			x--;
			radiusError += 2 * ( y - x + 1 );
		}
	}
}

void D3DGraphics::DrawFilledCircleClip( int centerX,int centerY,int radius,const RectI& clip,Color color )
{
	auto DrawScanlineClip = [this,&clip,color]( int x0,int x1,int y )
	{
		if( ( y >= clip.top ) & ( y <= clip.bottom ) )
		{
			x0 = max( x0,clip.left );
			x1 = min( x1,clip.right );
			for( int x = x0; x <= x1; x++ )
			{
				PutPixel( x,y,color );
			}
		}
	};
	
	int x = radius,y = 0;
	int radiusError = 1 - x;
	while( x >= y )
	{
		DrawScanlineClip( centerX - x,centerX + x,centerY - y );
		DrawScanlineClip( centerX - x,centerX + x,centerY + y );
		DrawScanlineClip( centerX - y,centerX + y,centerY - x );
		DrawScanlineClip( centerX - y,centerX + y,centerY + x );

		y++;

		if( radiusError < 0 )
		{
			radiusError += 2 * y + 1;
		}
		else
		{
			x--;
			radiusError += 2 * ( y - x + 1 );
		}
	}
}

void D3DGraphics::DrawFlatTriangle( float y0,float y1,float m0,float m1,
	float b0,float b1,const RectI& clipRect,Color c )
{
	// calculate start and end scanlines
	const int yStart = max( int( y0 + 0.5f ),clipRect.top );
	const int yEnd = min( int( y1 + 0.5f ),clipRect.bottom + 1 ); // does not include this scanline

	for( int y = yStart; y < yEnd; y++ )
	{
		const int x0 = max( int( m0 * ( float( y ) + 0.5f ) + b0 + 0.5f ),clipRect.left );
		const int x1 = min( int( m1 * ( float( y ) + 0.5f ) + b1 + 0.5f ),clipRect.right + 1 );

		for( int x = x0; x < x1; x++ )
		{
			PutPixel( x,y,c );
		}
	}
}

void D3DGraphics::DrawTriangle( Vec2 p0,Vec2 p1,Vec2 p2,const RectI& clipRect,Color c )
{
	// sort vectors
	if( p1.y < p0.y ) {
		p0.Swap( p1 );
	}
	if( p2.y < p1.y ) {
		p1.Swap( p2 );
	}
	if( p1.y < p0.y ) {
		p1.Swap( p0 );
	}

	// determine triangle case if flat top
	if( p0.y == p1.y )
	{
		if( p0.x > p1.x ) p0.Swap( p1 );
		const float m0 = ( p2.x - p0.x ) / ( p2.y - p0.y );
		const float m1 = ( p2.x - p1.x ) / ( p2.y - p1.y );
		const float b0 = p2.x - m0 * p2.y;
		const float b1 = p2.x - m1 * p2.y;
		DrawFlatTriangle( p0.y,p2.y,m0,m1,b0,b1,clipRect,c );
	}
	else if( p1.y == p2.y )
	{
		if( p1.x > p2.x ) p1.Swap( p2 );
		const float m0 = ( p1.x - p0.x ) / ( p1.y - p0.y );
		const float m1 = ( p2.x - p0.x ) / ( p2.y - p0.y );
		const float b0 = p1.x - m0 * p1.y;
		const float b1 = p2.x - m1 * p2.y;
		DrawFlatTriangle( p0.y,p2.y,m0,m1,b0,b1,clipRect,c );
	}
	else
	{
		// calculate the slopes and intercepts before top/bottom splitting is performed
		// need m2 b2?
		// need to match point used for x-intercept calc
		const float m0 = ( p1.x - p0.x ) / ( p1.y - p0.y );
		const float m1 = ( p2.x - p0.x ) / ( p2.y - p0.y );
		const float m2 = ( p2.x - p1.x ) / ( p2.y - p1.y );
		const float b0 = p1.x - m0 * p1.y;
		const float b1 = p2.x - m1 * p2.y;
		const float b2 = p2.x - m2 * p2.y;

		// qy == p1.y
		const float qx = ( p1.y - p0.y ) * m1 + p0.x;

		// if major right
		if( p1.x < qx )
		{
			DrawFlatTriangle( p0.y,p1.y,m0,m1,b0,b1,clipRect,c );
			DrawFlatTriangle( p1.y,p2.y,m2,m1,b2,b1,clipRect,c );
		}
		else
		{
			DrawFlatTriangle( p0.y,p1.y,m1,m0,b1,b0,clipRect,c );
			DrawFlatTriangle( p1.y,p2.y,m1,m2,b1,b2,clipRect,c );
		}
	}
}