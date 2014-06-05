/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	D3DGraphics.h																		  *
 *	Copyright 2014 PlanetChili <http://www.planetchili.net>								  *
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

#include <d3d9.h>
#include "Vec2.h"
#include "Colors.h"

class D3DGraphics
{
public:
	D3DGraphics( HWND hWnd );
	~D3DGraphics();
	void PutPixel( int x,int y,D3DCOLOR c );
	D3DCOLOR GetPixel( int x,int y ) const;
	inline void DrawLine( Vec2& pt1,Vec2& pt2,D3DCOLOR c )
	{
		DrawLine( (int)pt1.x,(int)pt1.y,(int)pt2.x,(int)pt2.y,c );
	}
	void DrawLine( int x1,int y1,int x2,int y2,D3DCOLOR c );
	void DrawCircle( int centerX,int centerY,int radius,D3DCOLOR c );
	void BeginFrame();
	void EndFrame();
public:
	static const unsigned int	SCREENWIDTH =	800;
	static const unsigned int	SCREENHEIGHT =	600;
private:
	const D3DCOLOR		FILLVALUE =		BLACK;
	IDirect3D9*			pDirect3D;
	IDirect3DDevice9*	pDevice;
	IDirect3DSurface9*	pBackBuffer;
	D3DCOLOR*			pSysBuffer;
};