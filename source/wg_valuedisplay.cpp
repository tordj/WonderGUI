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

#include	<math.h>
#include	<wg_valuedisplay.h>
#include	<wg_font.h>
#include	<wg_gfxdevice.h>


static const char c_widgetType[] = {"ValueDisplay"};


//____ WgValueDisplay() _________________________________________________________________

WgValueDisplay::WgValueDisplay()
{
	m_bRegenText	= true;
	m_pFonts		= 0;
	m_text.setAlignment( WG_EAST );
	m_text.SetWrap(false);
}

//____ ~WgValueDisplay() ___________________________________________________________

WgValueDisplay::~WgValueDisplay()
{
}

//____ Type() _________________________________________________________________

const char * WgValueDisplay::Type( void ) const
{
	return GetClass();
}

const char * WgValueDisplay::GetClass( void )
{
	return c_widgetType;
}

//____ SetFonts() _____________________________________________________________

bool WgValueDisplay::SetFonts( WgFont * _pFonts )
{
  if( _pFonts != m_pFonts )
	{
		m_pFonts			= _pFonts;
		m_bRegenText		= true;
		_requestRender();
	}

	return true;
}


//____ SetFormat() ____________________________________________________________

void WgValueDisplay::SetFormat( const WgValueFormat& format )
{
	m_format = format;
	_requestRender();
}

//____ PreferredSize() __________________________________________________________

WgSize WgValueDisplay::PreferredSize() const
{
	//TODO: Implement!

	return WgSize(1,1);
}


//____ _valueModified() ________________________________________________________

void WgValueDisplay::_valueModified()
{
	// NOTE: We have decided to not post any event on _valueModified since it
	// can only be done through the API.

	m_bRegenText = true;
	_requestRender();
}

//____ _rangeModified() ________________________________________________________

void WgValueDisplay::_rangeModified()
{
}


//____ _onRefresh() ____________________________________________________________

void WgValueDisplay::_onRefresh( void )
{
	if( m_pFonts != 0 )
	{
		m_bRegenText = true;
		_requestRender();
	}

}

//____ _onRender() _____________________________________________________________

void WgValueDisplay::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	if( m_bRegenText )
	{
		m_text.setScaledValue( m_value, m_format.scale, m_format );
		m_bRegenText = false;
	}

	pDevice->PrintText( _clip, &m_text, _canvas );
}

//____ _onCloneContent() _______________________________________________________

void WgValueDisplay::_onCloneContent( const WgWidget * _pOrg )
{
	WgValueDisplay * pOrg = (WgValueDisplay *) _pOrg;

	Wg_Interface_ValueHolder::_onCloneContent( pOrg );

	m_pFonts		= pOrg->m_pFonts;
	m_format		= pOrg->m_format;
	m_text.setText(&pOrg->m_text);
	m_text.setAlignment(pOrg->m_text.alignment());

}

//____ _onEnable() _____________________________________________________________

void WgValueDisplay::_onEnable( void )
{
	m_text.setMode(WG_MODE_NORMAL);
	_requestRender();
}

//____ _onDisable() ____________________________________________________________

void WgValueDisplay::_onDisable( void )
{
	m_text.setMode(WG_MODE_DISABLED);
	_requestRender();
}
