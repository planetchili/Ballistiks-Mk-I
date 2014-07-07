#pragma once
#include <Windows.h>
#include <gdiplus.h>
#include <string>

class Font
{
	friend class Surface;
public:
	Font( const std::wstring& family,float size )
		:
		font( family.c_str(),size )
	{}
private:
	inline operator const Gdiplus::Font*() const
	{
		return &font;
	}
private:
	Gdiplus::Font font;
};