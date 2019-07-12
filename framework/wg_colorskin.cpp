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



WgColorSkinPtr WgColorSkin::Create( WgColor col )
{
	return WgColorSkinPtr(new WgColorSkin(col));
}


WgColorSkin::WgColorSkin( WgColor col )
{
	m_color = col;
}

	
void WgColorSkin::Render( wg::GfxDevice * pDevice, WgState state, const WgRect& _canvas, int scale ) const
{
    
	pDevice->fill(_canvas, m_color );
}
	
bool WgColorSkin::IsOpaque() const
{
	return m_color.a == 255 ? true : false;
}

bool WgColorSkin::IsOpaque(WgState state) const
{
	return m_color.a == 255 ? true : false;
}

bool WgColorSkin::IsOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state, int scale ) const
{
	return m_color.a == 255 ? true : false;
}

WgSize WgColorSkin::MinSize(int scale) const
{
	return WgSize(0,0);
}

WgSize WgColorSkin::PreferredSize(int scale) const
{
	return WgSize(0,0);
}

WgSize WgColorSkin::ContentPadding(int scale) const
{
	return WgSize(0, 0);
}

WgCoord	WgColorSkin::ContentOfs(WgState state, int scale) const
{
	return WgCoord(0, 0);
}


WgSize  WgColorSkin::SizeForContent( const WgSize contentSize, int scale ) const
{
	return contentSize;
}

WgRect  WgColorSkin::ContentRect( const WgRect& canvas, WgState state, int scale ) const
{
	return canvas;
}

bool WgColorSkin::MarkTest( const WgCoord& ofs, const WgSize& canvasSize, WgState state, int opacityTreshold, int scale ) const
{
	return m_color.a >= opacityTreshold ? true : false;
}

bool WgColorSkin::IsStateIdentical(WgState state, WgState comparedTo) const
{
	return true;
}
