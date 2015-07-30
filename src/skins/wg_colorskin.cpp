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

#include <wg_colorskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>

const char WgColorSkin::CLASSNAME[] = {"ColorSkin"};


//____ create() _______________________________________________________________

WgColorSkin_p WgColorSkin::create( WgColor col )
{
	return WgColorSkin_p(new WgColorSkin(col));
}

//____ Constructor ____________________________________________________________

WgColorSkin::WgColorSkin( WgColor col )
{
	m_color = col;
}

//____ isInstanceOf() _________________________________________________________

bool WgColorSkin::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgSkin::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgColorSkin::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgColorSkin_p WgColorSkin::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgColorSkin_p( static_cast<WgColorSkin*>(pObject.rawPtr()) );

	return 0;
}


	
void WgColorSkin::render( WgGfxDevice * pDevice, const WgRect& _canvas, WgState state, const WgRect& _clip ) const
{
	pDevice->fill( _clip, m_color );
}
	
bool WgColorSkin::isOpaque() const
{
	return m_color.a == 255 ? true : false;
}

bool WgColorSkin::isOpaque(WgState state) const
{
	return m_color.a == 255 ? true : false;
}

bool WgColorSkin::isOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state ) const
{
	return m_color.a == 255 ? true : false;
}

WgSize WgColorSkin::minSize() const
{
	return WgSize(0,0);
}

WgSize WgColorSkin::preferredSize() const
{
	return WgSize(0,0);
}

WgSize WgColorSkin::contentPadding() const
{
	return WgSize(0,0);
}

WgSize  WgColorSkin::sizeForContent( const WgSize contentSize ) const
{
	return contentSize;
}

WgRect  WgColorSkin::contentRect( const WgRect& canvas, WgState state ) const
{
	return canvas;
}

bool WgColorSkin::markTest( const WgCoord& ofs, const WgRect& canvas, WgState state, int opacityTreshold ) const
{
	return ( canvas.contains(ofs) && ((int)m_color.a) >= opacityTreshold );
}

bool WgColorSkin::isStateIdentical( WgState state, WgState comparedTo ) const
{
	return true;
}
