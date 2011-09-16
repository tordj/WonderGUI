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



#include <wg_gizmo_pixmap.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>


static const char	c_gizmoType[] = {"Pixmap"};

//____ Constructor ____________________________________________________________

WgGizmoPixmap::WgGizmoPixmap()
{
}

//____ Destructor _____________________________________________________________

WgGizmoPixmap::~WgGizmoPixmap()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoPixmap::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoPixmap::GetMyType()
{
	return c_gizmoType;
}

//____ SetSource() _____________________________________________________________

void WgGizmoPixmap::SetSource( const WgBlockSetPtr& pBlockSet )
{
	if( m_pGfx != pBlockSet )
	{
		m_pGfx = pBlockSet;

		if( m_pGfx && m_pGfx->IsOpaque() )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		RequestRender();
	}
}

//____ BestSize() _____________________________________________________________

WgSize WgGizmoPixmap::BestSize() const
{
	if( m_pGfx )
		return m_pGfx->GetSize();

	return MinSize();
}



//____ _onCloneContent() _______________________________________________________

void WgGizmoPixmap::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoPixmap * pOrg = (WgGizmoPixmap*) _pOrg;

	m_pGfx = pOrg->m_pGfx;
}

//____ _onRender() _____________________________________________________________

void WgGizmoPixmap::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	if( !m_pGfx )
		return;

	WgBlock	block;
	if( m_bEnabled )
		block = m_pGfx->GetBlock(WG_MODE_NORMAL, _canvas);
	else
		block = m_pGfx->GetBlock(WG_MODE_DISABLED, _canvas);

	pDevice->ClipBlitBlock( _clip, block, _canvas);
}

//____ _onAlphaTest() ___________________________________________________________

bool WgGizmoPixmap::_onAlphaTest( const WgCord& ofs )
{
	if( !m_pGfx )
		return	false;												// No visible pixel, so don't accept the mark...

	WgMode mode = WG_MODE_NORMAL;
	if( !m_bEnabled )
		mode = WG_MODE_DISABLED;

	WgSize sz = Size();

	return WgUtil::MarkTestBlock( ofs, m_pGfx->GetBlock(mode,sz), WgRect(0,0,sz.w,sz.h) );
}

//____ _onEnable() _____________________________________________________________

void WgGizmoPixmap::_onEnable()
{
	if( m_pGfx )
	{
		if( !m_pGfx->SameBlock(WG_MODE_NORMAL, WG_MODE_DISABLED) )
			RequestRender();
	}
}

//____ _onDisable() ____________________________________________________________

void WgGizmoPixmap::_onDisable()
{
	if( m_pGfx )
	{
		if( !m_pGfx->SameBlock(WG_MODE_NORMAL, WG_MODE_DISABLED) )
			RequestRender();
	}
}




