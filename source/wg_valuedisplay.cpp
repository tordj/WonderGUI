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

//____ SetTextProperties() _____________________________________________________________

void WgValueDisplay::SetTextProperties( const WgTextpropPtr& _pProp )
{
	if( _pProp != m_text.getProperties() )
	{
		m_text.setProperties(_pProp);
		_regenText();
		_requestRender();
	}
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
	return WgSize(m_text.width(),m_text.height());
}


//____ _valueModified() ________________________________________________________

void WgValueDisplay::_valueModified()
{
	// NOTE: We have decided to not post any event on _valueModified since it
	// can only be done through the API.

	_regenText();
	_requestRender();
}

//____ _rangeModified() ________________________________________________________

void WgValueDisplay::_rangeModified()
{
}


//____ _onRefresh() ____________________________________________________________

void WgValueDisplay::_onRefresh( void )
{
	_regenText();
	_requestRender();
}

//____ _regenText() ____________________________________________________________

void WgValueDisplay::_regenText()
{
		m_text.setScaledValue( m_value, m_format.scale, m_format );
}

//____ _onRender() _____________________________________________________________

void WgValueDisplay::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	pDevice->PrintText( _clip, &m_text, _canvas );
}

//____ _onCloneContent() _______________________________________________________

void WgValueDisplay::_onCloneContent( const WgWidget * _pOrg )
{
	WgValueDisplay * pOrg = (WgValueDisplay *) _pOrg;

	Wg_Interface_ValueHolder::_onCloneContent( pOrg );

	m_format		= pOrg->m_format;
	m_text.setText(&pOrg->m_text);
	m_text.setAlignment(pOrg->m_text.alignment());
	m_text.setProperties(pOrg->m_text.getProperties());
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
