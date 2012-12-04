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

static const char	c_gizmoType[] = {"Filler"};

//____ Constructor ____________________________________________________________

WgFiller::WgFiller()
{
	m_defaultSize = WgSize(1,1);
	m_bOpaque = false;
	m_mode = WG_MODE_NORMAL;
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
	return c_gizmoType;
}

//____ SetDefaultSize() _______________________________________________________

void WgFiller::SetDefaultSize( const WgSize& size )
{
	if( size != m_defaultSize )
	{
		m_defaultSize = size;
		_requestResize();
	}
}

//____ SetColors() _____________________________________________________________

void WgFiller::SetColors( const WgColorsetPtr& pColors )
{
	WgColor oldC = m_pColors?m_pColors->Color(m_mode):WgColor(0,0,0,0);
	WgColor newC = pColors?pColors->Color(m_mode):WgColor(0,0,0,0);

	m_pColors = pColors;

	if( oldC != newC )
	{
		m_bOpaque = (newC.a == 255);
		_requestRender();
	}
}

//____ DefaultSize() __________________________________________________________

WgSize WgFiller::DefaultSize() const
{
	return m_defaultSize;
}


//____ _onCloneContent() _______________________________________________________

void WgFiller::_onCloneContent( const WgGizmo * _pOrg )
{
	WgFiller * pOrg = (WgFiller*) _pOrg;

	m_defaultSize = pOrg->m_defaultSize;
	m_pColors = pOrg->m_pColors;
}

//____ _onRender() _____________________________________________________________

void WgFiller::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	if( m_pColors )
		pDevice->Fill( _clip, m_pColors->Color(m_mode) );
}

//____ _onAlphaTest() ___________________________________________________________

bool WgFiller::_onAlphaTest( const WgCoord& ofs )
{
	if( m_pColors && m_pColors->Color(m_mode).a != 0 )
		return true;

	return false;
}

//____ _isColorChanged() ______________________________________________________

bool WgFiller::_isColorChanged( WgMode newMode ) const
{
	if( newMode != m_mode && m_pColors && m_pColors->Color(m_mode) != m_pColors->Color(newMode) )
		return true;

	return false;
}

//____ _onEnable() _____________________________________________________________

void WgFiller::_onEnable()
{
	if( _isColorChanged(WG_MODE_NORMAL) )
		_requestRender();

	if( m_pColors && m_pColors->Color(WG_MODE_NORMAL).a == 255 )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	m_mode = WG_MODE_NORMAL;
}

//____ _onDisable() ____________________________________________________________

void WgFiller::_onDisable()
{
	if( _isColorChanged(WG_MODE_DISABLED) )
		_requestRender();

	if( m_pColors && m_pColors->Color(WG_MODE_DISABLED).a == 255 )
		m_bOpaque = true;
	else
		m_bOpaque = false;

	m_mode = WG_MODE_DISABLED;
}




