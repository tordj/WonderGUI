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

namespace wg 
{
	
	const char Caret::CLASSNAME[] = {"Caret"};
	
	//____ Caret() _____________________________________________________________
	
	Caret::Caret()
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
	
	Caret::Caret(	Caret * pIn )
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
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Caret::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Caret::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Caret_p Caret::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Caret_p( static_cast<Caret*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setBlitMode() __________________________________________________________
	
	void Caret::setBlitMode( BlitMode mode )
	{
		m_blitMode = mode;
	}
	
	
	//____ setSizeRatio() _________________________________________________________
	
	void Caret::setSizeRatio( Mode m, float ratio )
	{
		if( m < 0 || m >= (Mode) N_MODES )
			return;
	
		m_sizeRatio[m] = ratio;
	}
	
	
	//____ setMode() ______________________________________________________________
	
	bool Caret::setMode( Mode m, const GfxAnim_p& pAnim, Coord bearing, int advance, float size_ratio )
	{
		if( m < 0 || m >= (Mode) N_MODES )
			return false;
	
		m_pAnim[m]			= pAnim;
		m_bearing[m]		= bearing;
		m_advance[m]		= advance;
		m_sizeRatio[m]		= size_ratio;
	
		return true;
	}
	
	//____ setBearing() ___________________________________________________________
	
	void Caret::setBearing( Mode m, Coord bearing )
	{
		if( m < 0 || m >= (Mode) N_MODES )
			return;
	
		m_bearing[m] = bearing;
	}
	
	
	//____ setAdvance() ___________________________________________________________
	
	void Caret::setAdvance( Mode m, int advance )
	{
		if( m < 0 || m >= (Mode) N_MODES )
			return;
	
		m_advance[m] = advance;
	}
	
	//____ setScaleWidth() ___________________________________________________________
	
	void Caret::setScaleWidth( Mode m, bool bScaleWidth )
	{
		if( m < 0 || m >= (Mode) N_MODES )
			return;
	
		m_scaleWidth[m] = bScaleWidth;
	}
	
	
	//____ setAnim() ______________________________________________________________
	
	void Caret::setAnim( Mode m, const GfxAnim_p& pAnim )
	{
		if( m < 0 || m >= (Mode) N_MODES )
			return;
	
		m_pAnim[m] = pAnim;
	}
	

} // namespace wg
