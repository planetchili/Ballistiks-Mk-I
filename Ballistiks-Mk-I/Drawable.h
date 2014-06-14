#pragma once
#include "Mat3.h"
#include "Rect.h"
#include <limits.h>

class Drawable
{
public:
	Drawable()
		:
		Drawable( Mat3::Identity() )
	{}
	Drawable( Mat3 trans )
		:
		trans( trans ),
		clip( INT_MIN,( INT_MAX - 100 ),INT_MIN,( INT_MAX - 100 ) )
	{}
	void Transform( const Mat3& mat )
	{
		trans = mat * trans;
	}
	void Clip( const RectF& rect )
	{
		clip.ClipTo( rect );
	}
	virtual void Rasterize( class D3DGraphics& gfx ) const = 0;
protected:
	Mat3 trans;
	RectF clip;
};