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
#include <wg_caret.h>

const char WgCaret::CLASSNAME[] = {"Caret"};

//____ WgCaret() _____________________________________________________________

WgCaret::WgCaret()
{
	for( int i = 0 ; i < N_MODES ; i++ )
	{
		m_pAnim[i]	= 0;
		m_advance[i] = 0;
		m_sizeRatio[i]	= 1.f;
		m_scaleWidth[i] = false;
	}
	m_blitMode = NORMAL;
}

WgCaret::WgCaret(	WgCaret * pIn )
{
	for( int i = 0 ; i < N_MODES ; i++ )
	{
		m_pAnim[i]		= pIn->m_pAnim[i];
		m_bearing[i]	= pIn->m_bearing[i];
		m_advance[i]	= pIn->m_advance[i];
		m_sizeRatio[i]	= pIn->m_sizeRatio[i];
		m_scaleWidth[i] = pIn->m_scaleWidth[i];
	}

	m_blitMode = pIn->m_blitMode;
}

//____ IsInstanceOf() _________________________________________________________

bool WgCaret::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgCaret::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgCaretPtr WgCaret::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgCaretPtr( static_cast<WgCaret*>(pObject.RawPtr()) );

	return 0;
}


//____ SetBlitMode() __________________________________________________________

void WgCaret::SetBlitMode( BlitMode mode )
{
	m_blitMode = mode;
}


//____ SetSizeRatio() _________________________________________________________

void WgCaret::SetSizeRatio( Mode m, float ratio )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_sizeRatio[m] = ratio;
}


//____ SetMode() ______________________________________________________________

bool WgCaret::SetMode( Mode m, const WgGfxAnimPtr& pAnim, WgCoord bearing, int advance, float size_ratio )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return false;

	m_pAnim[m]			= pAnim;
	m_bearing[m]		= bearing;
	m_advance[m]		= advance;
	m_sizeRatio[m]		= size_ratio;

	return true;
}

//____ SetBearing() ___________________________________________________________

void WgCaret::SetBearing( Mode m, WgCoord bearing )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_bearing[m] = bearing;
}


//____ SetAdvance() ___________________________________________________________

void WgCaret::SetAdvance( Mode m, int advance )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_advance[m] = advance;
}

//____ SetScaleWidth() ___________________________________________________________

void WgCaret::SetScaleWidth( Mode m, bool bScaleWidth )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_scaleWidth[m] = bScaleWidth;
}


//____ SetAnim() ______________________________________________________________

void WgCaret::SetAnim( Mode m, const WgGfxAnimPtr& pAnim )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_pAnim[m] = pAnim;
}

