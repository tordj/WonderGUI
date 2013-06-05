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



#include <wg_image.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_surface.h>


static const char	c_widgetType[] = {"Image"};

//____ Constructor ____________________________________________________________

WgImage::WgImage() : m_pSurface(0)
{
}

//____ Destructor _____________________________________________________________

WgImage::~WgImage()
{
}

//____ Type() _________________________________________________________________

const char * WgImage::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgImage::GetClass()
{
	return c_widgetType;
}

//____ SetSkin() _____________________________________________________________

void WgImage::SetSkin( const WgSkinPtr& pSkin )
{
	if( m_pSkin != pSkin )
	{
		bool bResize = false;

		if( !pSkin || !m_pSkin || pSkin->ContentPadding() != m_pSkin->ContentPadding() || pSkin->PreferredSize() != m_pSkin->PreferredSize() )
			bResize = true;

		m_pSkin = pSkin;

		if( m_pSkin && m_pSkin->IsOpaque() )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		_requestRender();
		if( bResize )
			_requestResize();
	}
}

//____ SetImage() _____________________________________________________________

void WgImage::SetImage( WgSurface * pSurface, const WgRect& rect )
{
	if( pSurface != m_pSurface || rect != m_rect )
	{
		m_pSurface = pSurface;

		if( pSurface )
			m_rect = WgRect( rect, WgRect(pSurface->Size()) );
		else
			m_rect.Clear();

		_requestRender();
	}
}

void WgImage::SetImage( WgSurface * pSurface )
{
	if( pSurface != m_pSurface )
	{
		m_pSurface = pSurface;

		if( pSurface )
			m_rect = pSurface->Size();
		else
			m_rect.Clear();

		_requestRender();
	}
}



//____ PreferredSize() _____________________________________________________________

WgSize WgImage::PreferredSize() const
{
	WgSize	sz;

	if( m_pSkin )
	{
		if( m_pSurface )
			return m_pSkin->SizeForContent( m_rect.Size() );
		else
			return m_pSkin->PreferredSize();
	}
	else if( m_pSurface )
		return m_rect.Size();

	return WgSize(1,1);
}

//____ _onCloneContent() _______________________________________________________

void WgImage::_onCloneContent( const WgWidget * _pOrg )
{
	WgImage * pOrg = (WgImage*) _pOrg;

	m_pSkin		= pOrg->m_pSkin;
	m_pSurface	= pOrg->m_pSurface;
	m_rect		= pOrg->m_rect;
}

//____ _onRender() _____________________________________________________________

void WgImage::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	if( m_pSkin )
		m_pSkin->Render( pDevice, _canvas, State(), _clip );

	if( m_pSurface && !m_rect.IsEmpty() )
	{
		WgRect dest;
		if( m_pSkin )
			dest = m_pSkin->ContentRect( _canvas, State() );
		else
			dest = _canvas;

		pDevice->ClipStretchBlit( _clip, m_pSurface, m_rect, dest );
	}
}

//____ _onAlphaTest() ___________________________________________________________

bool WgImage::_onAlphaTest( const WgCoord& ofs )
{
	if( m_pSkin )
		return m_pSkin->MarkTest( ofs, Size(), State(), m_markOpacity );

	if( m_pSurface && !m_rect.IsEmpty() )
	{
		WgRect dest;
		if( m_pSkin )
			dest = m_pSkin->ContentRect( Size(), State() );
		else
			dest = Size();

		return WgUtil::MarkTestStretchRect( ofs, m_pSurface, m_rect, dest, m_markOpacity );
	}

	return false;
}

//____ _onEnable() _____________________________________________________________

void WgImage::_onEnable()
{
	if( m_pSkin )
	{
		if( !m_pSkin->IsStateIdentical( WG_STATE_NORMAL, WG_STATE_DISABLED ) )
			_requestRender();
	}
}

//____ _onDisable() ____________________________________________________________

void WgImage::_onDisable()
{
	if( m_pSkin )
	{
		if( !m_pSkin->IsStateIdentical( WG_STATE_NORMAL, WG_STATE_DISABLED ) )
			_requestRender();
	}
}
