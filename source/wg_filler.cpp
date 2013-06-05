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



#include <wg_filler.h>
#include <wg_gfxdevice.h>

static const char	c_widgetType[] = {"Filler"};

//____ Constructor ____________________________________________________________

WgFiller::WgFiller()
{
	m_defaultSize = WgSize(1,1);
	m_bOpaque = false;
	m_state = WG_STATE_NORMAL;
}

//____ Destructor _____________________________________________________________

WgFiller::~WgFiller()
{
}

//____ Type() _________________________________________________________________

const char * WgFiller::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgFiller::GetClass()
{
	return c_widgetType;
}

//____ SetPreferredSize() _______________________________________________________

void WgFiller::SetPreferredSize( const WgSize& size )
{
	if( size != m_defaultSize )
	{
		m_defaultSize = size;
		_requestResize();
	}
}

//____ SetSkin() _____________________________________________________________

void WgFiller::SetSkin( const WgSkinPtr& pSkin )
{
	m_pSkin = pSkin;
	_requestRender();
}

//____ PreferredSize() __________________________________________________________

WgSize WgFiller::PreferredSize() const
{
	return m_defaultSize;
}


//____ _onCloneContent() _______________________________________________________

void WgFiller::_onCloneContent( const WgWidget * _pOrg )
{
	WgFiller * pOrg = (WgFiller*) _pOrg;

	m_defaultSize = pOrg->m_defaultSize;
	m_pSkin = pOrg->m_pSkin;
}

//____ _onRender() _____________________________________________________________

void WgFiller::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	if( m_pSkin )
		m_pSkin->Render( pDevice, _canvas, m_state, _clip );
}

//____ _onAlphaTest() ___________________________________________________________

bool WgFiller::_onAlphaTest( const WgCoord& ofs )
{
	 return ( m_pSkin && m_pSkin->MarkTest( ofs, WgRect(0,0,Size()), m_state, m_markOpacity ) );
}


//____ _onEnable() _____________________________________________________________

void WgFiller::_onEnable()
{
	m_state = WG_STATE_NORMAL;

	if( m_pSkin )
	{
		m_bOpaque =  m_pSkin->IsOpaque(WG_STATE_NORMAL);
		if( !m_pSkin->IsStateIdentical(WG_STATE_NORMAL,WG_STATE_DISABLED) )
			_requestRender();
	}
}

//____ _onDisable() ____________________________________________________________

void WgFiller::_onDisable()
{
	m_state = WG_STATE_DISABLED;

	if( m_pSkin )
	{
		m_bOpaque =  m_pSkin->IsOpaque(WG_STATE_DISABLED);
		if( !m_pSkin->IsStateIdentical(WG_STATE_NORMAL,WG_STATE_DISABLED) )
			_requestRender();
	}
}
