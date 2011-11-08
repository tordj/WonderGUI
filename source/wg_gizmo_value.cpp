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
#include	<wg_gizmo_value.h>
#include	<wg_font.h>
#include	<wg_gfxdevice.h>


static const char c_gizmoType[] = {"Value"};

using namespace WgSignal;


//____ WgGizmoValue() _________________________________________________________________

WgGizmoValue::WgGizmoValue()
{
	m_bRegenText	= true;
	m_pFonts		= 0;
	m_text.setAlignment( WgOrigo::midRight() );
	m_text.SetWrap(false);
}

//____ ~WgGizmoValue() ___________________________________________________________

WgGizmoValue::~WgGizmoValue()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoValue::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoValue::GetMyType( void )
{
	return c_gizmoType;
}

//____ SetFonts() _____________________________________________________________

bool WgGizmoValue::SetFonts( WgFont * _pFonts )
{
  if( _pFonts != m_pFonts )
	{
		m_pFonts			= _pFonts;
		m_bRegenText		= true;
		RequestRender();
	}

	return true;
}


//____ SetFormat() ____________________________________________________________

void WgGizmoValue::SetFormat( const WgValueFormat& format )
{
	m_format = format;
	RequestRender();
}

//____ DefaultSize() __________________________________________________________

WgSize WgGizmoValue::DefaultSize() const
{
	//TODO: Implement!

	return WgSize(1,1);
}


//____ _valueModified() ________________________________________________________

void WgGizmoValue::_valueModified()
{
	// NOTE: We have decided to not post any event on _valueModified since it 
	// can only be done through the API.

	Emit( IntegerChanged(), m_value );
	Emit( Fraction(), FractionalValue() );

	m_bRegenText = true;
	RequestRender();
}

//____ _rangeModified() ________________________________________________________

void WgGizmoValue::_rangeModified()
{
	Emit( Fraction(), FractionalValue() );
}


//____ _onRefresh() ____________________________________________________________

void WgGizmoValue::_onRefresh( void )
{
	if( m_pFonts != 0 )
	{
		m_bRegenText = true;
		RequestRender();
	}

}

//____ _onRender() _____________________________________________________________

void WgGizmoValue::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	if( m_bRegenText )
	{
		m_text.setScaledValue( m_value, m_format.scale, m_format );
		m_bRegenText = false;
	}

	pDevice->PrintText( _clip, &m_text, _canvas );
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoValue::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoValue * pOrg = (WgGizmoValue *) _pOrg;

	Wg_Interface_ValueHolder::_onCloneContent( pOrg );

	m_pFonts		= pOrg->m_pFonts;
	m_format		= pOrg->m_format;
	m_text.setText(&pOrg->m_text);
	m_text.setAlignment(pOrg->m_text.alignment());

}

//____ _onEnable() _____________________________________________________________

void WgGizmoValue::_onEnable( void )
{
	m_text.setMode(WG_MODE_NORMAL);
	RequestRender();
}

//____ _onDisable() ____________________________________________________________

void WgGizmoValue::_onDisable( void )
{
	m_text.setMode(WG_MODE_DISABLED);
	RequestRender();
}
