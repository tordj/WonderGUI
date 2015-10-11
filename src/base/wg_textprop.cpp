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

namespace wg 
{
	
	using namespace WgUtil;
	
	//____ Constructor ____________________________________________________________
	
	Textprop::Textprop()
	{
		clearAll();
	}
	
	Textprop::Textprop( const Textprop_p& pProp )
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
	
	Textprop_p Textprop::reg() const
	{
		return	Textprop_p(TextpropManager::registerProp(*this));
	}
	
	//____ clearAll() _____________________________________________________________
	
	void Textprop::clearAll()
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
	
	void Textprop::setColor( Color col )
	{
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
		{
			m_stateProp[i].m_bColored	= true;
			m_stateProp[i].m_color		= col;
		}
	}
	
	void Textprop::setColor( Color col, State state )
	{
		int i = _stateToIndex(state);
		m_stateProp[i].m_bColored	= true;
		m_stateProp[i].m_color		= col;
	}
	
	
	//____ setBgColor() ___________________________________________________________
	
	void Textprop::setBgColor( Color col )
	{
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
		{
			m_stateProp[i].m_bBgColor	= true;
			m_stateProp[i].m_bgColor		= col;
		}
	}
	
	void Textprop::setBgColor( Color col, State state )
	{
		int i = _stateToIndex(state);
		m_stateProp[i].m_bBgColor	= true;
		m_stateProp[i].m_bgColor	= col;
	}
	
	
	
	//____ setStyle() _____________________________________________________________
	
	void Textprop::setStyle( FontAlt style )
	{
		assert( style<WG_NB_FONTSTYLES );
	
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
			m_stateProp[i].m_style = style;
	}
	
	void Textprop::setStyle( FontAlt style, State state )
	{
		assert( style<WG_NB_FONTSTYLES );
	
		int i = _stateToIndex(state);
		m_stateProp[i].m_style = style;
	}
	
	//____ setSize() ______________________________________________________________
	
	void Textprop::setSize( int size )
	{
		assert( size >= 0 && size < 2048 );
	
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
			m_stateProp[i].m_size = size;
	}
	
	void Textprop::setSize( int size, State state )
	{
		assert( size >= 0 && size < 2048 );
	
		int i = _stateToIndex(state);
		m_stateProp[i].m_size = size;
	}
	
	
	//____ setUnderlined() ________________________________________________________
	
	void Textprop::setUnderlined()
	{
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
			m_stateProp[i].m_bUnderlined	= true;
	}
	
	void Textprop::setUnderlined( State state )
	{
		int i = _stateToIndex(state);
		m_stateProp[i].m_bUnderlined = true;
	}
	
	//____ clearColor() _____________________________________________________________
	
	void Textprop::clearColor()
	{
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
		{
			m_stateProp[i].m_bColored	= false;
			m_stateProp[i].m_color.argb = 0xFFFFFFFF;
		}
	}
	
	void Textprop::clearColor( State state )
	{
		int i = _stateToIndex(state);
		m_stateProp[i].m_bColored	= false;
		m_stateProp[i].m_color.argb = 0xFFFFFFFF;
	}
	
	//____ clearBgColor() _________________________________________________________
	
	void Textprop::clearBgColor()
	{
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
		{
			m_stateProp[i].m_bBgColor	= false;
			m_stateProp[i].m_bgColor.argb = 0xFFFFFFFF;
		}
	}
	
	void Textprop::clearBgColor( State state )
	{
		int i = _stateToIndex(state);
		m_stateProp[i].m_bBgColor	= false;
		m_stateProp[i].m_bgColor.argb = 0xFFFFFFFF;
	}
	
	
	//____ clearStyle() _____________________________________________________________
	
	void Textprop::clearStyle()
	{
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
			m_stateProp[i].m_style = WG_FONT_NORMAL;
	}
	
	void Textprop::clearStyle( State state )
	{
		int i = _stateToIndex(state);
		m_stateProp[i].m_style = WG_FONT_NORMAL;
	}
	
	
	//____ clearSize() ______________________________________________________________
	
	void Textprop::clearSize()
	{
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
			m_stateProp[i].m_size = 0;
	}
	
	void Textprop::clearSize( State state )
	{
		int i = _stateToIndex(state);
		m_stateProp[i].m_size = 0;
	}
	
	
	//____ clearUnderlined() ________________________________________________________
	
	void Textprop::clearUnderlined()
	{
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
			m_stateProp[i].m_bUnderlined = false;
	}
	
	void Textprop::clearUnderlined( State state )
	{
		int i = _stateToIndex(state);
		m_stateProp[i].m_bUnderlined = false;
	}
	
	
	//____ isColorStatic() _________________________________________________________
	
	bool Textprop::isColorStatic() const
	{
		for( int i = 1 ; i < WG_NB_STATES ; i++ )
			if( m_stateProp[0].m_bColored != m_stateProp[i].m_bColored || m_stateProp[0].m_color != m_stateProp[i].m_color )
				return false;
	
		return true;
	}
	
	//____ isBgColorStatic() _______________________________________________________
	
	bool Textprop::isBgColorStatic() const
	{
		for( int i = 1 ; i < WG_NB_STATES ; i++ )
			if( m_stateProp[0].m_bBgColor != m_stateProp[i].m_bBgColor || m_stateProp[0].m_bgColor != m_stateProp[i].m_bgColor )
				return false;
	
		return true;
	}
	
	
	//____ isStyleStatic() _________________________________________________________
	
	bool Textprop::isStyleStatic() const
	{
		for( int i = 1 ; i < WG_NB_STATES ; i++ )
			if( m_stateProp[0].m_style != m_stateProp[i].m_style )
				return false;
	
		return true;
	}
	
	//____ isSizeStatic() __________________________________________________________
	
	bool Textprop::isSizeStatic() const
	{
		for( int i = 1 ; i < WG_NB_STATES ; i++ )
			if( m_stateProp[0].m_size != m_stateProp[i].m_size )
				return false;
	
		return true;
	}
	
	
	//____ isUnderlineStatic() _____________________________________________________
	
	bool Textprop::isUnderlineStatic() const
	{
		for( int i = 1 ; i < WG_NB_STATES ; i++ )
			if( m_stateProp[0].m_bUnderlined != m_stateProp[i].m_bUnderlined )
				return false;
	
		return true;
	}
	
	//____ compareColorTo() ________________________________________________________
	
	bool Textprop::compareColorTo( const Textprop_p& pProp ) const
	{
		for( int i = 1 ; i < WG_NB_STATES ; i++ )
			if( m_stateProp[i].m_bColored != pProp->m_stateProp[i].m_bColored || m_stateProp[i].m_color != pProp->m_stateProp[i].m_color )
				return false;
	
		return true;
	}
	
	//____ compareBgColorTo() ______________________________________________________
	
	bool Textprop::compareBgColorTo( const Textprop_p& pProp ) const
	{
		for( int i = 1 ; i < WG_NB_STATES ; i++ )
			if( m_stateProp[i].m_bBgColor != pProp->m_stateProp[i].m_bBgColor || m_stateProp[i].m_bgColor != pProp->m_stateProp[i].m_bgColor )
				return false;
	
		return true;
	}
	
	
	//____ compareStyleTo() ________________________________________________________
	
	bool Textprop::compareStyleTo( const Textprop_p& pProp ) const
	{
		for( int i = 1 ; i < WG_NB_STATES ; i++ )
			if( m_stateProp[i].m_style != pProp->m_stateProp[i].m_style )
				return false;
	
		return true;
	}
	
	//____ compareSizeTo() _________________________________________________________
	
	bool Textprop::compareSizeTo( const Textprop_p& pProp ) const
	{
		for( int i = 1 ; i < WG_NB_STATES ; i++ )
			if( m_stateProp[i].m_size != pProp->m_stateProp[i].m_size )
				return false;
	
		return true;
	}
	
	
	//____ compareUnderlineTo() ____________________________________________________
	bool Textprop::compareUnderlineTo( const Textprop_p& pProp ) const
	{
		for( int i = 1 ; i < WG_NB_STATES ; i++ )
			if( m_stateProp[i].m_bUnderlined != pProp->m_stateProp[i].m_bUnderlined )
				return false;
	
		return true;
	}
	
	
	//____ _calculateChecksum() ____________________________________________________
	
	uint8_t Textprop::_calculateChecksum() const
	{
		WgUtil::Checksum8	chk;
	
		chk.add8( m_visibilityFlags );
		chk.add8( m_breakLevel );
		chk.add( &m_pLink, sizeof(TextLink_p) );
		chk.add( &m_pFont, sizeof(Font_p) );
	
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
		{
			chk.add8( (uint8_t) m_stateProp[i].m_bColored );
			chk.add8( (uint8_t) m_stateProp[i].m_bBgColor );
			chk.add8( (uint8_t) m_stateProp[i].m_bUnderlined );
			chk.add32( m_stateProp[i].m_color.argb );
			chk.add32( m_stateProp[i].m_bgColor.argb );
			chk.add16( m_stateProp[i].m_style );
			chk.add16( m_stateProp[i].m_size );
		}
	
		return chk.getChecksum();
	}
	
	//____ _compareTo() ____________________________________________________________
	
	bool Textprop::_compareTo( const Textprop * pProp ) const
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
	
	bool Textprop::setCharVisibility( uint16_t specialCharacter, bool bVisible )
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
	
	bool Textprop::charVisibility( uint16_t specialCharacter ) const
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
	//							>>> Textprop_p <<<
	//=============================================================================
	
	//____ Constructor ____________________________________________________________
	
	Textprop_p::Textprop_p( TextpropHolder * pProp )
	{
		if( pProp )
			m_hProp = pProp->m_id;
		else
			m_hProp = 0;
	
		TextpropManager::incRef(m_hProp, 1);
	}
	
	Textprop_p::Textprop_p( uint16_t hProp )
	{
		m_hProp = hProp;
		TextpropManager::incRef(m_hProp);
	}
	
	
	Textprop_p::Textprop_p(const Textprop_p& r)
	{
		m_hProp = r.m_hProp;
		TextpropManager::incRef(m_hProp);
	}
	
	
	//____ Destructor _____________________________________________________________
	
	Textprop_p::~Textprop_p()
	{
		TextpropManager::decRef(m_hProp, 1);
	}
	
	
	//____ operator= ______________________________________________________________
	
	Textprop_p& Textprop_p::operator=(const Textprop_p& ref)
	{
		if(m_hProp != ref.m_hProp)
		{
			TextpropManager::decRef(m_hProp);
			m_hProp = ref.m_hProp;
			TextpropManager::incRef(m_hProp);
		}
		return *this;
	}
	
	Textprop_p& Textprop_p::operator=(int handle)
	{
		if(m_hProp != handle)
		{
			TextpropManager::decRef(m_hProp);
			m_hProp = handle;
			TextpropManager::incRef(m_hProp);
		}
		return *this;
	}
	
	//____ operator* ______________________________________________________________
	
	const Textprop & Textprop_p::operator*() const
	{
		return TextpropManager::getProp(m_hProp);
	}
	
	//____ operator-> _____________________________________________________________
	
	const Textprop * Textprop_p::operator->() const
	{
		return TextpropManager::getPropPtr(m_hProp);
	}
	
	//_____________________________________________________________________________
	void Textprop::assertIntegrity() const
	{
		for( int i = 0; i<WG_NB_STATES; i++ )
			assert( m_stateProp[i].m_style<WG_NB_FONTSTYLES );
	}
	
	//_____________________________________________________________________________
	
	bool Textprop::isEqual(State state0, State state1) const
	{
		return	m_stateProp[state0].m_bColored == m_stateProp[state1].m_bColored &&
				m_stateProp[state0].m_bBgColor == m_stateProp[state1].m_bBgColor &&
				m_stateProp[state0].m_bUnderlined == m_stateProp[state1].m_bUnderlined &&
				m_stateProp[state0].m_color == m_stateProp[state1].m_color &&
				m_stateProp[state0].m_bgColor == m_stateProp[state1].m_bgColor &&
				m_stateProp[state0].m_style == m_stateProp[state1].m_style &&
				m_stateProp[state0].m_size == m_stateProp[state1].m_size;
	}

} // namespace wg
