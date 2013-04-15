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



WgFrameSkinPtr WgFrameSkin::Create( int thickness, WgColor col )
{
	return WgFrameSkinPtr(new WgFrameSkin(thickness, col));
}


WgFrameSkin::WgFrameSkin( int thickness, WgColor col )
{
    m_thickness = thickness;
	m_color = col;
}

	
void WgFrameSkin::Render( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip )
{
    WgRect top( WgRect(_canvas.x, _canvas.y, _canvas.w, m_thickness), _clip );
    WgRect left( WgRect(_canvas.x, _canvas.y+m_thickness, m_thickness, _canvas.h - m_thickness*2), _clip );
    WgRect right( WgRect(_canvas.x + _canvas.w - m_thickness, _canvas.y+m_thickness, m_thickness, _canvas.h - m_thickness*2), _clip );
    WgRect bottom( WgRect(_canvas.x, _canvas.y + _canvas.h - m_thickness, _canvas.w, m_thickness), _clip );
    
	pDevice->Fill( top, m_color );
	pDevice->Fill( left, m_color );
	pDevice->Fill( right, m_color );
	pDevice->Fill( bottom, m_color );
}
	
bool WgFrameSkin::IsOpaque() const
{
	return false;
}
