#pragma once
#include "PhysicalCircle.h"

class Ball : public PhysicalCircle
{
public:
	class Drawable : public ::Drawable
	{
	public:
		Drawable( const Ball& parent )
			:
			::Drawable( Mat3::Translation( parent.pos ) ),
			parent( parent )
		{}
		virtual void Rasterize( D3DGraphics& gfx ) const override
		{
			const float scaledRadius = parent.radius * trans.ExtractScaleIsometric();
			const Vec2 screenPos = trans * Vec2 { 0.0f,0.0f };
			gfx.DrawFilledCircleClip( screenPos,(int)scaledRadius,clip,parent.color1 );
			gfx.DrawCircleClip( screenPos,(int)scaledRadius,clip,parent.color0 );
		}
	private:
		const Ball& parent;
	};
public:
	Ball( Vec2 pos )
		:
		PhysicalCircle( BALL_RADIUS,BALL_DENSITY,pos,BALL_DRAG )
	{}
	Drawable GetDrawable() const
	{
		return Drawable( *this );
	}
private:
	const Color color0 = BALL_COLOR0;
	const Color color1 = BALL_COLOR1;
};