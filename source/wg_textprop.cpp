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


//____ Constructor ____________________________________________________________

WgTextProp::WgTextProp()
{
	m_pLink				= 0;
	m_pFont				= 0;
	m_visibilityFlags	= 0;

	for( int i = 0 ; i < WG_NB_MODES ; i++ )
	{
		m_modeProp[i].m_bColored	= false;
		m_modeProp[i].m_bUnderlined = false;
		m_modeProp[i].m_color		= WgColor(255,255,255,255);
		m_modeProp[i].m_style		= WG_STYLE_NORMAL;
		m_modeProp[i].m_size		= 0;
	}
}

WgTextProp::WgTextProp( const WgTextPropPtr& pProp )
{
	m_pLink				= pProp->m_pLink;
	m_pFont				= pProp->m_pFont;
	m_visibilityFlags	= pProp->m_visibilityFlags;

	for( int i = 0 ; i < WG_NB_MODES ; i++ )
	{
		m_modeProp[i].m_bColored	= pProp->m_modeProp[i].m_bColored;
		m_modeProp[i].m_bUnderlined = pProp->m_modeProp[i].m_bUnderlined;
		m_modeProp[i].m_color		= pProp->m_modeProp[i].m_color;
		m_modeProp[i].m_style		= pProp->m_modeProp[i].m_style;
		m_modeProp[i].m_style		= pProp->m_modeProp[i].m_size;
	}
}


//____ Register() _____________________________________________________________

WgTextPropPtr WgTextProp::Register() const
{
	return	WgTextPropPtr(WgTextPropManager::RegisterProp(*this));
}

//____ ClearAll() _____________________________________________________________

void WgTextProp::ClearAll()
{
	m_visibilityFlags = 0;
	m_pFont = 0;
	m_pLink = 0;

	for( int i = 0 ; i < WG_NB_MODES ; i++ )
	{
		m_modeProp[i].m_bColored	= false;
		m_modeProp[i].m_color.argb = 0xFFFFFFFF;

		m_modeProp[i].m_style = WG_STYLE_NORMAL;
		m_modeProp[i].m_size = 0;
		m_modeProp[i].m_bUnderlined = false;
	}
}



//____ SetColor() _____________________________________________________________

void WgTextProp::SetColor( WgColor col, WgMode mode )
{
	if( mode == WG_MODE_ALL )
	{
		for( int i = 0 ; i < WG_NB_MODES ; i++ )
		{
			m_modeProp[i].m_bColored	= true;
			m_modeProp[i].m_color		= col;
		}
	}
	else
	{
		m_modeProp[mode].m_bColored	= true;
		m_modeProp[mode].m_color	= col;
	}
}

//____ SetStyle() _____________________________________________________________

void WgTextProp::SetStyle( WgFontStyle style, WgMode mode )
{
	assert( style<WG_NB_FONTSTYLES );

	if( mode == WG_MODE_ALL )
	{
		for( int i = 0 ; i < WG_NB_MODES ; i++ )
			m_modeProp[i].m_style = style;
	}
	else
		m_modeProp[mode].m_style = style;
}

//____ SetSize() ______________________________________________________________

void WgTextProp::SetSize( int size, WgMode mode )
{
	assert( size >= 0 && size < 2048 );

	if( mode == WG_MODE_ALL )
	{
		for( int i = 0 ; i < WG_NB_MODES ; i++ )
			m_modeProp[i].m_size = size;
	}
	else
		m_modeProp[mode].m_size = size;
}



//____ SetUnderlined() ________________________________________________________

void WgTextProp::SetUnderlined( WgMode mode )
{
	if( mode == WG_MODE_ALL )
	{
		for( int i = 0 ; i < WG_NB_MODES ; i++ )
			m_modeProp[i].m_bUnderlined	= true;
	}
	else
		m_modeProp[mode].m_bUnderlined = true;
}

//____ ClearColor() _____________________________________________________________

void WgTextProp::ClearColor( WgMode mode )
{
	if( mode == WG_MODE_ALL )
	{
		for( int i = 0 ; i < WG_NB_MODES ; i++ )
		{
			m_modeProp[i].m_bColored	= false;
			m_modeProp[i].m_color.argb = 0xFFFFFFFF;
		}
	}
	else
	{
		m_modeProp[mode].m_bColored	= false;
		m_modeProp[mode].m_color.argb = 0xFFFFFFFF;
	}
}

//____ ClearStyle() _____________________________________________________________

void WgTextProp::ClearStyle( WgMode mode )
{
	if( mode == WG_MODE_ALL )
	{
		for( int i = 0 ; i < WG_NB_MODES ; i++ )
			m_modeProp[i].m_style = WG_STYLE_NORMAL;
	}
	else
		m_modeProp[mode].m_style = WG_STYLE_NORMAL;
}

//____ ClearSize() ______________________________________________________________

void WgTextProp::ClearSize( WgMode mode )
{
	if( mode == WG_MODE_ALL )
	{
		for( int i = 0 ; i < WG_NB_MODES ; i++ )
			m_modeProp[i].m_size = 0;
	}
	else
		m_modeProp[mode].m_size = 0;
}


//____ ClearUnderlined() ________________________________________________________

void WgTextProp::ClearUnderlined( WgMode mode )
{
	if( mode == WG_MODE_ALL )
	{
		for( int i = 0 ; i < WG_NB_MODES ; i++ )
			m_modeProp[i].m_bUnderlined = false;
	}
	else
		m_modeProp[mode].m_bUnderlined = false;
}

//_____________________________________________________________________________
bool WgTextProp::IsColorStatic() const
{
	for( int i = 1 ; i < WG_NB_MODES ; i++ )
		if( m_modeProp[0].m_bColored != m_modeProp[i].m_bColored || m_modeProp[0].m_color != m_modeProp[i].m_color )
			return false;

	return true;
}

//_____________________________________________________________________________
bool WgTextProp::IsStyleStatic() const
{
	for( int i = 1 ; i < WG_NB_MODES ; i++ )
		if( m_modeProp[0].m_style != m_modeProp[i].m_style )
			return false;

	return true;
}

//_____________________________________________________________________________
bool WgTextProp::IsSizeStatic() const
{
	for( int i = 1 ; i < WG_NB_MODES ; i++ )
		if( m_modeProp[0].m_size != m_modeProp[i].m_size )
			return false;

	return true;
}


//_____________________________________________________________________________
bool WgTextProp::IsUnderlineStatic() const
{
	for( int i = 1 ; i < WG_NB_MODES ; i++ )
		if( m_modeProp[0].m_bUnderlined != m_modeProp[i].m_bUnderlined )
			return false;

	return true;
}

//_____________________________________________________________________________
bool WgTextProp::CompareColorTo( const WgTextPropPtr& pProp ) const
{
	for( int i = 1 ; i < WG_NB_MODES ; i++ )
		if( m_modeProp[i].m_bColored != pProp->m_modeProp[i].m_bColored || m_modeProp[i].m_color != pProp->m_modeProp[i].m_color )
			return false;

	return true;
}

//_____________________________________________________________________________
bool WgTextProp::CompareStyleTo( const WgTextPropPtr& pProp ) const
{
	for( int i = 1 ; i < WG_NB_MODES ; i++ )
		if( m_modeProp[i].m_style != pProp->m_modeProp[i].m_style )
			return false;

	return true;
}

//_____________________________________________________________________________
bool WgTextProp::CompareSizeTo( const WgTextPropPtr& pProp ) const
{
	for( int i = 1 ; i < WG_NB_MODES ; i++ )
		if( m_modeProp[i].m_size != pProp->m_modeProp[i].m_size )
			return false;

	return true;
}


//_____________________________________________________________________________
bool WgTextProp::CompareUnderlineTo( const WgTextPropPtr& pProp ) const
{
	for( int i = 1 ; i < WG_NB_MODES ; i++ )
		if( m_modeProp[i].m_bUnderlined != pProp->m_modeProp[i].m_bUnderlined )
			return false;

	return true;
}


//____ CalculateChecksum() ____________________________________________________

Uint8 WgTextProp::CalculateChecksum() const
{
	WgUtil::Checksum8	chk;

	chk.Add8( m_visibilityFlags );
	chk.Add( &m_pLink, sizeof(WgTextLinkPtr) );
	chk.Add( &m_pFont, sizeof(WgFont *) );

	for( int i = 0 ; i < WG_NB_MODES ; i++ )
	{
		chk.Add8( (Uint8) m_modeProp[i].m_bColored );
		chk.Add8( (Uint8) m_modeProp[i].m_bUnderlined );
		chk.Add32( m_modeProp[i].m_color.argb );
		chk.Add16( m_modeProp[i].m_style );
		chk.Add16( m_modeProp[i].m_size );
	}

	return chk.GetChecksum();
}

//____ CompareTo() ____________________________________________________________

bool WgTextProp::CompareTo( const WgTextProp * pProp ) const
{
	if( m_pFont != pProp->m_pFont || m_pLink != pProp->m_pLink ||
		m_visibilityFlags != pProp->m_visibilityFlags )
		return false;

	for( int i = 0 ; i < WG_NB_MODES ; i++ )
	{
		if( m_modeProp[i].m_style != pProp->m_modeProp[i].m_style ||
			m_modeProp[i].m_size != pProp->m_modeProp[i].m_size ||
			m_modeProp[i].m_bColored != pProp->m_modeProp[i].m_bColored ||
			m_modeProp[i].m_bUnderlined != pProp->m_modeProp[i].m_bUnderlined ||
			m_modeProp[i].m_color.argb != pProp->m_modeProp[i].m_color.argb )
			return false;
	}

	return true;
}

//____ SetCharVisibility() ____________________________________________________

bool WgTextProp::SetCharVisibility( Uint16 specialCharacter, bool bVisible )
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

//____ GetCharVisibility() ____________________________________________________

bool WgTextProp::GetCharVisibility( Uint16 specialCharacter ) const
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
//							>>> WgTextPropPtr <<<
//=============================================================================

//____ Constructor ____________________________________________________________

WgTextPropPtr::WgTextPropPtr( WgTextPropHolder * pProp )
{
	if( pProp )
		m_hProp = pProp->m_id;
	else m_hProp = 0;

	WgTextPropManager::IncRef(m_hProp, 1);
}

WgTextPropPtr::WgTextPropPtr( Uint16 hProp )
{
	m_hProp = hProp;
	WgTextPropManager::IncRef(m_hProp);
}


WgTextPropPtr::WgTextPropPtr(const WgTextPropPtr& r)
{
	m_hProp = r.m_hProp;
	WgTextPropManager::IncRef(m_hProp);
}


//____ Destructor _____________________________________________________________

WgTextPropPtr::~WgTextPropPtr()
{
	WgTextPropManager::DecRef(m_hProp, 1);
}


//____ operator= ______________________________________________________________

WgTextPropPtr& WgTextPropPtr::operator=(const WgTextPropPtr& ref)
{
	if(m_hProp != ref.m_hProp)
	{
		WgTextPropManager::DecRef(m_hProp);
		m_hProp = ref.m_hProp;
		WgTextPropManager::IncRef(m_hProp);
	}
	return *this;
}

WgTextPropPtr& WgTextPropPtr::operator=(int handle)
{
	if(m_hProp != handle)
	{
		WgTextPropManager::DecRef(m_hProp);
		m_hProp = handle;
		WgTextPropManager::IncRef(m_hProp);
	}
	return *this;
}

//____ operator* ______________________________________________________________

const WgTextProp & WgTextPropPtr::operator*() const
{
	return WgTextPropManager::GetProp(m_hProp);
}

//____ operator-> _____________________________________________________________

const WgTextProp * WgTextPropPtr::operator->() const
{
	return WgTextPropManager::GetPropPtr(m_hProp);
}

//_____________________________________________________________________________
void WgTextProp::AssertIntegrity() const
{
	for( int i = 0; i<WG_NB_MODES; i++ )
		assert( m_modeProp[i].m_style<WG_NB_FONTSTYLES );
}

//_____________________________________________________________________________

bool WgTextProp::IsEqual(WgMode mode0, WgMode mode1) const
{
	return	m_modeProp[mode0].m_bColored == m_modeProp[mode1].m_bColored &&
			m_modeProp[mode0].m_bUnderlined == m_modeProp[mode1].m_bUnderlined &&
			m_modeProp[mode0].m_color == m_modeProp[mode1].m_color &&
			m_modeProp[mode0].m_style == m_modeProp[mode1].m_style &&			
			m_modeProp[mode0].m_size == m_modeProp[mode1].m_size;			
}
