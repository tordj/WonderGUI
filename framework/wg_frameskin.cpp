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

#include <wg_frameskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>



WgSkinPtr WgFrameSkin::Create( int thickness, WgColor col )
{
	return WgSkinPtr(new WgFrameSkin(thickness, col));
}


WgFrameSkin::WgFrameSkin( int thickness, WgColor col )
{
    m_thickness = thickness;
	m_color = col;
}

	
void WgFrameSkin::Render( wg::GfxDevice * pDevice, WgState state, const WgRect& _canvas, int scale ) const
{
	int thickness = m_thickness * scale >> WG_SCALE_BINALS;

    WgRect top( _canvas.x, _canvas.y, _canvas.w, thickness);
    WgRect left( _canvas.x, _canvas.y+thickness, thickness, _canvas.h - thickness*2);
    WgRect right( _canvas.x + _canvas.w - thickness, _canvas.y+thickness, thickness, _canvas.h - thickness*2);
    WgRect bottom( _canvas.x, _canvas.y + _canvas.h - thickness, _canvas.w, thickness);
    
	pDevice->fill( top, m_color );
	pDevice->fill( left, m_color );
	pDevice->fill( right, m_color );
	pDevice->fill( bottom, m_color );
}
	
bool WgFrameSkin::IsOpaque() const
{
	return false;
}

bool WgFrameSkin::IsOpaque(WgState state) const
{
	return false;
}

bool WgFrameSkin::IsOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state, int scale ) const
{
	return false;
}

WgSize WgFrameSkin::MinSize(int scale) const
{
	return WgSize(0,0);
}

WgSize WgFrameSkin::PreferredSize(int scale) const
{
	return WgSize(0,0);
}

WgSize WgFrameSkin::ContentPadding(int scale) const
{
	return WgSize(0, 0);
}

WgCoord	WgFrameSkin::ContentOfs(WgState state, int scale) const
{
	return WgCoord(0, 0);
}


WgSize  WgFrameSkin::SizeForContent( const WgSize contentSize, int scale ) const
{
	return contentSize;
}

WgRect  WgFrameSkin::ContentRect( const WgRect& canvas, WgState state, int scale ) const
{
	return canvas;
}

bool WgFrameSkin::MarkTest( const WgCoord& ofs, const WgSize& canvasSize, WgState state, int opacityTreshold, int scale ) const
{
	int thickness = m_thickness * scale >> WG_SCALE_BINALS;

	if( ofs.x < m_thickness || ofs.y < m_thickness || ofs.x > canvasSize.w - m_thickness || ofs.y > canvasSize.h - m_thickness )
		return m_color.a >= opacityTreshold ? true : false;
		
	return false;
}

bool WgFrameSkin::IsStateIdentical(WgState state, WgState comparedTo) const
{
	return true;
}
