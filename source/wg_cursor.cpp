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
#include <wg_cursor.h>


//____ WgCursor() _____________________________________________________________

WgCursor::WgCursor()
{
	for( int i = 0 ; i < N_MODES ; i++ )
	{
		m_pAnim[i]	= 0;
		m_advance[i] = 0;
		m_scaleMode[i] = FIXED_SIZE;
		m_sizeRatio[i]	= 1.f;
	}
	m_blitMode = NORMAL;
}

WgCursor::WgCursor(	WgCursor& in )
{
	for( int i = 0 ; i < N_MODES ; i++ )
	{
		m_pAnim[i]		= in.m_pAnim[i];
		m_bearing[i]	= in.m_bearing[i];
		m_advance[i]	= in.m_advance[i];
		m_scaleMode[i]	= in.m_scaleMode[i];
		m_sizeRatio[i]	= in.m_sizeRatio[i];
	}

	m_blitMode = in.m_blitMode;
}

//____ SetBlitMode() __________________________________________________________

void WgCursor::SetBlitMode( BlitMode mode )
{
	m_blitMode = mode;
}


//____ SetGfxBorders() ________________________________________________________

void WgCursor::SetGfxBorders( Mode m, WgBorders borders )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_gfxBorders[m] = borders;
}

//____ SetScaleMode() _________________________________________________________

void WgCursor::SetScaleMode( Mode m, ScaleMode mode )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_scaleMode[m] = mode;
}

//____ SetSizeRatio() _________________________________________________________

void WgCursor::SetSizeRatio( Mode m, float ratio )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_sizeRatio[m] = ratio;
}



//____ SetMode() ______________________________________________________________

bool WgCursor::SetMode( Mode m, WgGfxAnim * pAnim, WgCoord bearing, int advance, ScaleMode mode, float size_ratio, WgBorders gfxBorders )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return false;

	m_pAnim[m]			= pAnim;
	m_bearing[m]		= bearing;
	m_advance[m]		= advance;
	m_scaleMode[m]		= mode;
	m_sizeRatio[m]		= size_ratio;
	m_gfxBorders[m]		= gfxBorders;

	return true;
}

//____ SetBearing() ___________________________________________________________

void WgCursor::SetBearing( Mode m, WgCoord bearing )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_bearing[m] = bearing;
}


//____ SetAdvance() ___________________________________________________________

void WgCursor::SetAdvance( Mode m, int advance )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_advance[m] = advance;
}


//____ SetAnim() ______________________________________________________________

void WgCursor::SetAnim( Mode m, WgGfxAnim * pAnim )
{
	if( m < 0 || m >= (Mode) N_MODES )
		return;

	m_pAnim[m] = pAnim;
}

