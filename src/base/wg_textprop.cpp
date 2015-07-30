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

#include <wg_textprop.h>
#include <wg_util.h>
#include <wg_color.h>
#include <wg_char.h>
#include <wg_font.h>

#include <memory.h>

using namespace WgUtil;

//____ Constructor ____________________________________________________________

WgTextprop::WgTextprop()
{
	clearAll();
}

WgTextprop::WgTextprop( const WgTextpropPtr& pProp )
{
	m_pLink				= pProp->m_pLink;
	m_pFont				= pProp->m_pFont;
	m_visibilityFlags	= pProp->m_visibilityFlags;
	m_breakLevel		= pProp->m_breakLevel;

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_stateProp[i].m_bColored	= pProp->m_stateProp[i].m_bColored;
		m_stateProp[i].m_bBgColor	= pProp->m_stateProp[i].m_bBgColor;
		m_stateProp[i].m_bUnderlined = pProp->m_stateProp[i].m_bUnderlined;
		m_stateProp[i].m_color		= pProp->m_stateProp[i].m_color;
		m_stateProp[i].m_bgColor		= pProp->m_stateProp[i].m_bgColor;
		m_stateProp[i].m_style		= pProp->m_stateProp[i].m_style;
		m_stateProp[i].m_size		= pProp->m_stateProp[i].m_size;
	}
}


//____ reg() _____________________________________________________________

WgTextpropPtr WgTextprop::reg() const
{
	return	WgTextpropPtr(WgTextpropManager::registerProp(*this));
}

//____ clearAll() _____________________________________________________________

void WgTextprop::clearAll()
{
	m_visibilityFlags = 0;
	m_pFont = 0;
	m_pLink = 0;
	m_breakLevel = -1;

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_stateProp[i].m_bColored	= false;
		m_stateProp[i].m_color.argb = 0xFFFFFFFF;

		m_stateProp[i].m_bBgColor	= false;
		m_stateProp[i].m_bgColor.argb = 0xFFFFFFFF;

		m_stateProp[i].m_style = WG_FONT_NORMAL;
		m_stateProp[i].m_size = 0;
		m_stateProp[i].m_bUnderlined = false;
	}
}



//____ setColor() _____________________________________________________________

void WgTextprop::setColor( WgColor col )
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_stateProp[i].m_bColored	= true;
		m_stateProp[i].m_color		= col;
	}
}

void WgTextprop::setColor( WgColor col, WgState state )
{
	int i = _stateToIndex(state);
	m_stateProp[i].m_bColored	= true;
	m_stateProp[i].m_color		= col;
}


//____ setBgColor() ___________________________________________________________

void WgTextprop::setBgColor( WgColor col )
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_stateProp[i].m_bBgColor	= true;
		m_stateProp[i].m_bgColor		= col;
	}
}

void WgTextprop::setBgColor( WgColor col, WgState state )
{
	int i = _stateToIndex(state);
	m_stateProp[i].m_bBgColor	= true;
	m_stateProp[i].m_bgColor	= col;
}



//____ setStyle() _____________________________________________________________

void WgTextprop::setStyle( WgFontAlt style )
{
	assert( style<WG_NB_FONTSTYLES );

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_stateProp[i].m_style = style;
}

void WgTextprop::setStyle( WgFontAlt style, WgState state )
{
	assert( style<WG_NB_FONTSTYLES );

	int i = _stateToIndex(state);
	m_stateProp[i].m_style = style;
}

//____ setSize() ______________________________________________________________

void WgTextprop::setSize( int size )
{
	assert( size >= 0 && size < 2048 );

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_stateProp[i].m_size = size;
}

void WgTextprop::setSize( int size, WgState state )
{
	assert( size >= 0 && size < 2048 );

	int i = _stateToIndex(state);
	m_stateProp[i].m_size = size;
}


//____ setUnderlined() ________________________________________________________

void WgTextprop::setUnderlined()
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_stateProp[i].m_bUnderlined	= true;
}

void WgTextprop::setUnderlined( WgState state )
{
	int i = _stateToIndex(state);
	m_stateProp[i].m_bUnderlined = true;
}

//____ clearColor() _____________________________________________________________

void WgTextprop::clearColor()
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_stateProp[i].m_bColored	= false;
		m_stateProp[i].m_color.argb = 0xFFFFFFFF;
	}
}

void WgTextprop::clearColor( WgState state )
{
	int i = _stateToIndex(state);
	m_stateProp[i].m_bColored	= false;
	m_stateProp[i].m_color.argb = 0xFFFFFFFF;
}

//____ clearBgColor() _________________________________________________________

void WgTextprop::clearBgColor()
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_stateProp[i].m_bBgColor	= false;
		m_stateProp[i].m_bgColor.argb = 0xFFFFFFFF;
	}
}

void WgTextprop::clearBgColor( WgState state )
{
	int i = _stateToIndex(state);
	m_stateProp[i].m_bBgColor	= false;
	m_stateProp[i].m_bgColor.argb = 0xFFFFFFFF;
}


//____ clearStyle() _____________________________________________________________

void WgTextprop::clearStyle()
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_stateProp[i].m_style = WG_FONT_NORMAL;
}

void WgTextprop::clearStyle( WgState state )
{
	int i = _stateToIndex(state);
	m_stateProp[i].m_style = WG_FONT_NORMAL;
}


//____ clearSize() ______________________________________________________________

void WgTextprop::clearSize()
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_stateProp[i].m_size = 0;
}

void WgTextprop::clearSize( WgState state )
{
	int i = _stateToIndex(state);
	m_stateProp[i].m_size = 0;
}


//____ clearUnderlined() ________________________________________________________

void WgTextprop::clearUnderlined()
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_stateProp[i].m_bUnderlined = false;
}

void WgTextprop::clearUnderlined( WgState state )
{
	int i = _stateToIndex(state);
	m_stateProp[i].m_bUnderlined = false;
}


//____ isColorStatic() _________________________________________________________

bool WgTextprop::isColorStatic() const
{
	for( int i = 1 ; i < WG_NB_STATES ; i++ )
		if( m_stateProp[0].m_bColored != m_stateProp[i].m_bColored || m_stateProp[0].m_color != m_stateProp[i].m_color )
			return false;

	return true;
}

//____ isBgColorStatic() _______________________________________________________

bool WgTextprop::isBgColorStatic() const
{
	for( int i = 1 ; i < WG_NB_STATES ; i++ )
		if( m_stateProp[0].m_bBgColor != m_stateProp[i].m_bBgColor || m_stateProp[0].m_bgColor != m_stateProp[i].m_bgColor )
			return false;

	return true;
}


//____ isStyleStatic() _________________________________________________________

bool WgTextprop::isStyleStatic() const
{
	for( int i = 1 ; i < WG_NB_STATES ; i++ )
		if( m_stateProp[0].m_style != m_stateProp[i].m_style )
			return false;

	return true;
}

//____ isSizeStatic() __________________________________________________________

bool WgTextprop::isSizeStatic() const
{
	for( int i = 1 ; i < WG_NB_STATES ; i++ )
		if( m_stateProp[0].m_size != m_stateProp[i].m_size )
			return false;

	return true;
}


//____ isUnderlineStatic() _____________________________________________________

bool WgTextprop::isUnderlineStatic() const
{
	for( int i = 1 ; i < WG_NB_STATES ; i++ )
		if( m_stateProp[0].m_bUnderlined != m_stateProp[i].m_bUnderlined )
			return false;

	return true;
}

//____ compareColorTo() ________________________________________________________

bool WgTextprop::compareColorTo( const WgTextpropPtr& pProp ) const
{
	for( int i = 1 ; i < WG_NB_STATES ; i++ )
		if( m_stateProp[i].m_bColored != pProp->m_stateProp[i].m_bColored || m_stateProp[i].m_color != pProp->m_stateProp[i].m_color )
			return false;

	return true;
}

//____ compareBgColorTo() ______________________________________________________

bool WgTextprop::compareBgColorTo( const WgTextpropPtr& pProp ) const
{
	for( int i = 1 ; i < WG_NB_STATES ; i++ )
		if( m_stateProp[i].m_bBgColor != pProp->m_stateProp[i].m_bBgColor || m_stateProp[i].m_bgColor != pProp->m_stateProp[i].m_bgColor )
			return false;

	return true;
}


//____ compareStyleTo() ________________________________________________________

bool WgTextprop::compareStyleTo( const WgTextpropPtr& pProp ) const
{
	for( int i = 1 ; i < WG_NB_STATES ; i++ )
		if( m_stateProp[i].m_style != pProp->m_stateProp[i].m_style )
			return false;

	return true;
}

//____ compareSizeTo() _________________________________________________________

bool WgTextprop::compareSizeTo( const WgTextpropPtr& pProp ) const
{
	for( int i = 1 ; i < WG_NB_STATES ; i++ )
		if( m_stateProp[i].m_size != pProp->m_stateProp[i].m_size )
			return false;

	return true;
}


//____ compareUnderlineTo() ____________________________________________________
bool WgTextprop::compareUnderlineTo( const WgTextpropPtr& pProp ) const
{
	for( int i = 1 ; i < WG_NB_STATES ; i++ )
		if( m_stateProp[i].m_bUnderlined != pProp->m_stateProp[i].m_bUnderlined )
			return false;

	return true;
}


//____ _calculateChecksum() ____________________________________________________

Uint8 WgTextprop::_calculateChecksum() const
{
	WgUtil::Checksum8	chk;

	chk.add8( m_visibilityFlags );
	chk.add8( m_breakLevel );
	chk.add( &m_pLink, sizeof(WgTextLinkPtr) );
	chk.add( &m_pFont, sizeof(WgFontPtr) );

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		chk.add8( (Uint8) m_stateProp[i].m_bColored );
		chk.add8( (Uint8) m_stateProp[i].m_bBgColor );
		chk.add8( (Uint8) m_stateProp[i].m_bUnderlined );
		chk.add32( m_stateProp[i].m_color.argb );
		chk.add32( m_stateProp[i].m_bgColor.argb );
		chk.add16( m_stateProp[i].m_style );
		chk.add16( m_stateProp[i].m_size );
	}

	return chk.getChecksum();
}

//____ _compareTo() ____________________________________________________________

bool WgTextprop::_compareTo( const WgTextprop * pProp ) const
{
	if( m_pFont != pProp->m_pFont || m_pLink != pProp->m_pLink ||
		m_visibilityFlags != pProp->m_visibilityFlags ||
		m_breakLevel != pProp->m_breakLevel )
		return false;

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		if( m_stateProp[i].m_style != pProp->m_stateProp[i].m_style ||
			m_stateProp[i].m_size != pProp->m_stateProp[i].m_size ||
			m_stateProp[i].m_bColored != pProp->m_stateProp[i].m_bColored ||
			m_stateProp[i].m_bBgColor != pProp->m_stateProp[i].m_bBgColor ||
			m_stateProp[i].m_bUnderlined != pProp->m_stateProp[i].m_bUnderlined ||
			m_stateProp[i].m_color.argb != pProp->m_stateProp[i].m_color.argb ||
			m_stateProp[i].m_bgColor.argb != pProp->m_stateProp[i].m_bgColor.argb )
			return false;
	}

	return true;
}

//____ setCharVisibility() ____________________________________________________

bool WgTextprop::setCharVisibility( Uint16 specialCharacter, bool bVisible )
{
	switch( specialCharacter )
	{
		case '\n':
			m_bShowLF = bVisible;
			break;
		case '\t':
			m_bShowTAB = bVisible;
			break;
		case WG_BREAK_PERMITTED:
			m_bShowBreakPermitted = bVisible;
			break;
		case WG_HYPHEN_BREAK_PERMITTED:
			m_bShowHyphenBreakPermitted = bVisible;
			break;
		case WG_NO_BREAK_SPACE:
			m_bShowNoBreakSpace = bVisible;
			break;

		default:
			return false;
	};

	return true;
}

//____ charVisibility() ________________________________________________________

bool WgTextprop::charVisibility( Uint16 specialCharacter ) const
{
	switch( specialCharacter )
	{
		case ' ':
			return m_bShowSpace;
		case '\n':
			return m_bShowLF;
		case '\t':
			return m_bShowTAB;
		case WG_BREAK_PERMITTED:
			return m_bShowBreakPermitted;
		case WG_HYPHEN_BREAK_PERMITTED:
			return m_bShowHyphenBreakPermitted;
		case WG_NO_BREAK_SPACE:
			return m_bShowNoBreakSpace;
		default:
			return true;		// All other characters are always visible
	};
}

//=============================================================================
//							>>> WgTextpropPtr <<<
//=============================================================================

//____ Constructor ____________________________________________________________

WgTextpropPtr::WgTextpropPtr( WgTextpropHolder * pProp )
{
	if( pProp )
		m_hProp = pProp->m_id;
	else
		m_hProp = 0;

	WgTextpropManager::incRef(m_hProp, 1);
}

WgTextpropPtr::WgTextpropPtr( Uint16 hProp )
{
	m_hProp = hProp;
	WgTextpropManager::incRef(m_hProp);
}


WgTextpropPtr::WgTextpropPtr(const WgTextpropPtr& r)
{
	m_hProp = r.m_hProp;
	WgTextpropManager::incRef(m_hProp);
}


//____ Destructor _____________________________________________________________

WgTextpropPtr::~WgTextpropPtr()
{
	WgTextpropManager::decRef(m_hProp, 1);
}


//____ operator= ______________________________________________________________

WgTextpropPtr& WgTextpropPtr::operator=(const WgTextpropPtr& ref)
{
	if(m_hProp != ref.m_hProp)
	{
		WgTextpropManager::decRef(m_hProp);
		m_hProp = ref.m_hProp;
		WgTextpropManager::incRef(m_hProp);
	}
	return *this;
}

WgTextpropPtr& WgTextpropPtr::operator=(int handle)
{
	if(m_hProp != handle)
	{
		WgTextpropManager::decRef(m_hProp);
		m_hProp = handle;
		WgTextpropManager::incRef(m_hProp);
	}
	return *this;
}

//____ operator* ______________________________________________________________

const WgTextprop & WgTextpropPtr::operator*() const
{
	return WgTextpropManager::getProp(m_hProp);
}

//____ operator-> _____________________________________________________________

const WgTextprop * WgTextpropPtr::operator->() const
{
	return WgTextpropManager::getPropPtr(m_hProp);
}

//_____________________________________________________________________________
void WgTextprop::assertIntegrity() const
{
	for( int i = 0; i<WG_NB_STATES; i++ )
		assert( m_stateProp[i].m_style<WG_NB_FONTSTYLES );
}

//_____________________________________________________________________________

bool WgTextprop::isEqual(WgState state0, WgState state1) const
{
	return	m_stateProp[state0].m_bColored == m_stateProp[state1].m_bColored &&
			m_stateProp[state0].m_bBgColor == m_stateProp[state1].m_bBgColor &&
			m_stateProp[state0].m_bUnderlined == m_stateProp[state1].m_bUnderlined &&
			m_stateProp[state0].m_color == m_stateProp[state1].m_color &&
			m_stateProp[state0].m_bgColor == m_stateProp[state1].m_bgColor &&
			m_stateProp[state0].m_style == m_stateProp[state1].m_style &&
			m_stateProp[state0].m_size == m_stateProp[state1].m_size;
}
