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

namespace wg 
{
	
	const char ColorSkin::CLASSNAME[] = {"ColorSkin"};
	
	
	//____ create() _______________________________________________________________
	
	ColorSkin_p ColorSkin::create( Color col )
	{
		return ColorSkin_p(new ColorSkin(col));
	}
	
	//____ Constructor ____________________________________________________________
	
	ColorSkin::ColorSkin( Color col )
	{
		m_color = col;
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool ColorSkin::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Skin::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * ColorSkin::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	ColorSkin_p ColorSkin::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ColorSkin_p( static_cast<ColorSkin*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
		
	void ColorSkin::render( GfxDevice * pDevice, const Rect& _canvas, State state, const Rect& _clip ) const
	{
		pDevice->fill( _clip, m_color );
	}
		
	bool ColorSkin::isOpaque() const
	{
		return m_color.a == 255 ? true : false;
	}
	
	bool ColorSkin::isOpaque(State state) const
	{
		return m_color.a == 255 ? true : false;
	}
	
	bool ColorSkin::isOpaque( const Rect& rect, const Size& canvasSize, State state ) const
	{
		return m_color.a == 255 ? true : false;
	}
	
	Size ColorSkin::minSize() const
	{
		return Size(0,0);
	}
	
	Size ColorSkin::preferredSize() const
	{
		return Size(0,0);
	}
	
	Size ColorSkin::contentPadding() const
	{
		return Size(0,0);
	}

	Coord ColorSkin::contentOfs( State state ) const
	{
		return Coord(0,0);
	}

	
	Size  ColorSkin::sizeForContent( const Size contentSize ) const
	{
		return contentSize;
	}
	
	Rect  ColorSkin::contentRect( const Rect& canvas, State state ) const
	{
		return canvas;
	}
	
	bool ColorSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold ) const
	{
		return ( canvas.contains(ofs) && ((int)m_color.a) >= opacityTreshold );
	}
	
	bool ColorSkin::isStateIdentical( State state, State comparedTo ) const
	{
		return true;
	}

} // namespace wg
