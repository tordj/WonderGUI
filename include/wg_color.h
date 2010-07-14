/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------
	
  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef	WG_COLOR_DOT_H
#define	WG_COLOR_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include	<wg_types.h>
#endif

//____ Class WgColor _______________________________________________________________________

class WgColor
{
public:
	inline WgColor() {};
	inline WgColor( const WgColor& col );
	inline WgColor( Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255 );
	inline WgColor( Uint32 rgba );
	inline WgColor( Uint32 rgb, Uint8 a );

	inline bool operator==( const WgColor& k ) const;
	inline bool operator!=( const WgColor& k ) const;

	inline WgColor operator+( const WgColor& k ) const;
	inline WgColor operator-( const WgColor& k ) const;
	
	inline WgColor operator*( float f ) const;
	inline WgColor operator*( const WgColor& k ) const;

	static WgColor blend( const WgColor& start, const WgColor& dest, float grade );

	inline static WgColor& White() {return g_white;};
	inline static WgColor& Black() {return g_black;};


	union
	{
		struct
		{
			Uint8 b;
			Uint8 g;
			Uint8 r;
			Uint8 a;
		};
		Uint32 argb;		
	};


	static WgColor	g_white;
	static WgColor	g_black;
};


//-------------------------------------------------------------------
inline bool WgColor::operator==( const WgColor& k ) const
{
	if( argb == k.argb )
		return true;

	return false;
}

//-------------------------------------------------------------------
inline bool WgColor::operator!=( const WgColor& k ) const
{
	if( argb != k.argb )
		return true;

	return false;
}

//-------------------------------------------------------------------
inline WgColor WgColor::operator+( const WgColor& k ) const
{
	WgColor kNewColor;
	kNewColor.r = r + k.r;
	kNewColor.g = g + k.g;
	kNewColor.b = b + k.b;
	kNewColor.a = a + k.a;
	return kNewColor;
}

//-------------------------------------------------------------------
inline WgColor WgColor::operator-( const WgColor& k ) const
{
	WgColor kNewColor;
	kNewColor.r = r - k.r;
	kNewColor.g = g - k.g;
	kNewColor.b = b - k.b;
	kNewColor.a = a - k.a;
	return kNewColor;
}

//-------------------------------------------------------------------
inline WgColor WgColor::operator*( float f ) const
{
	WgColor kNewColor;
	kNewColor.r = (Uint8)( (float)r * f );
	kNewColor.g = (Uint8)( (float)g * f );
	kNewColor.b = (Uint8)( (float)b * f );
	kNewColor.a = (Uint8)( (float)a * f );
	return kNewColor;
}

//-------------------------------------------------------------------
inline WgColor WgColor::operator*( const WgColor& k ) const
{
	WgColor kNewColor;
	kNewColor.r = (Uint8)(((int)r * (int)k.r )/255);
	kNewColor.g = (Uint8)(((int)g * (int)k.g )/255);
	kNewColor.b = (Uint8)(((int)b * (int)k.b )/255);
	kNewColor.a = (Uint8)(((int)a * (int)k.a )/255);
	return kNewColor;
}



//____ WgColor::WgColor() _________________________________________________________________

inline WgColor::WgColor( const WgColor& _col )
{
	argb = _col.argb;
}

inline WgColor::WgColor( Uint32 _argb )
{
	argb = _argb;
}

inline WgColor::WgColor( Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a )
{
	a = _a;
	r = _r;
	g = _g;
	b = _b;
}

inline WgColor::WgColor( Uint32 rgb, Uint8 _a )
{
	argb = rgb;
	a = _a;
}

#endif // WG_COLOR_DOT_H
