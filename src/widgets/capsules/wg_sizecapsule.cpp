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

#include <wg_sizecapsule.h>

namespace wg 
{
	
	const char SizeCapsule::CLASSNAME[] = {"SizeCapsule"};
	
	//____ Constructor ____________________________________________________________
	
	SizeCapsule::SizeCapsule() : m_max(INT_MAX,INT_MAX)
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	SizeCapsule::~SizeCapsule()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool SizeCapsule::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Capsule::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * SizeCapsule::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	SizeCapsule_p SizeCapsule::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SizeCapsule_p( static_cast<SizeCapsule*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setPreferredSize() _____________________________________________________
	
	void SizeCapsule::setPreferredSize( Size size )
	{
		if( size != m_preferred )
		{
			m_preferred = size;
			_requestResize();
		}
	}
	
	//____ setSizes() _____________________________________________________________
	
	void SizeCapsule::setSizes( Size min, Size preferred, Size max )
	{
		m_min = min;
		m_preferred = preferred;
		m_max = max;
		_requestResize();
	}
	
	//____ setMinSize() ___________________________________________________________
	
	void SizeCapsule::setMinSize( Size size )
	{
		if( size != m_min )
		{
			m_min = size;
			_requestResize();
		}
	}
	
	//____ setMaxSize() ___________________________________________________________
	
	void SizeCapsule::setMaxSize( Size size )
	{
		if( size != m_max )
		{
			m_max = size;
			_requestResize();
		}
	}
	
	//____ preferredSize() ________________________________________________________
	
	Size SizeCapsule::preferredSize() const 
	{
		if( m_pChild )
		{
			Size pref = m_pChild->preferredSize();
	
			if( m_preferred.w != 0 )
			{
				pref.w = m_preferred.w;
				if( m_pScaler )
					pref.w = (int) (pref.w * m_pScaler->scaleX());
			}
			if( m_preferred.h != 0 )
			{
				pref.h = m_preferred.h;
				if( m_pScaler )
					pref.h = (int) (pref.h * m_pScaler->scaleY());
			}
	
			// Constrain against min/max, taking MatchingWidth/MatchingHeight into account.
			//TODO: Check so we don't have any corner cases that breaks the constraints and
			// and that priorities between preferred height/width are reasonable.
	
			Size min = minSize();
			Size max = maxSize();
	
			if( pref.w < min.w )
			{
				pref.w = min.w;
				pref.h = matchingHeight(pref.w);
			}
	
			if( pref.h < min.h )
			{
				pref.h = min.h;
				pref.w = matchingWidth(pref.h);
				if( pref.w < min.w )
					pref.w = min.w;
			}
	
			if( pref.w > max.w )
			{
				if( pref.h > max.h )
					pref = max;
				else
				{
					pref.w = max.w;
					pref.h = matchingHeight(pref.w);
					if( pref.h > max.h )
						pref.h = max.h;
				}
			}
			else if( pref.h > max.h )
			{
				if( pref.w > max.w )
					pref = max;
				else
				{
					pref.h = max.h;
					pref.w = matchingWidth(pref.h);
					if( pref.w > max.w )
						pref.w = max.w;
				}
			}
			return pref;
		}
		else
		{
			Size pref = m_preferred;
			if( m_pScaler )
			{
				pref.w = (int) (pref.w * m_pScaler->scaleX());
				pref.h = (int) (pref.h * m_pScaler->scaleY());
			}
			return pref;
		}
	}
	
	//____ minSize() ______________________________________________________________
	
	Size SizeCapsule::minSize() const 
	{
	
		if( m_pChild )
			return Size::max(m_min,m_pChild->minSize());
		else
			return m_min;
	}
	
	//____ maxSize() ______________________________________________________________
	
	Size SizeCapsule::maxSize() const 
	{
		if( m_pChild )
			return Size::min(m_max,m_pChild->maxSize());
		else
			return m_max; 
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int SizeCapsule::matchingHeight( int width ) const
	{
		if( m_preferred.h != 0 )
		{
			int h = m_preferred.h;
			
			if( m_pScaler )
				h = (int) (h * m_pScaler->scaleY() );
	
			if( m_pChild )
			{
				int max = m_pChild->maxSize().h;
				int min = m_pChild->minSize().h;
				limit( h, min, max );
			}
			return h;
		}
		else if( m_pChild )
		{
			int h = m_pChild->matchingHeight(width);
			limit( h, m_min.h, m_max.h );
			return h;
		}
		else
			return m_min.h;
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int SizeCapsule::matchingWidth( int height ) const
	{
		if( m_preferred.w != 0 )
		{
			int w = m_preferred.w;		
	
			if( m_pScaler )
				w = (int) (w * m_pScaler->scaleX() );
	
			if( m_pChild )
			{
				int max = m_pChild->maxSize().w;
				int min = m_pChild->minSize().w;
				limit( w, min, max );
			}
			return w;
		}
		else if( m_pChild )
		{
			int w = m_pChild->matchingWidth(height);
			limit( w, m_min.w, m_max.w );
			return w;
		}
		else
			return m_min.w;
	}
	
	//____ _onScaleChanged() ______________________________________________________
	
	void SizeCapsule::_onScaleChanged()
	{
		_requestResize();
	}

} // namespace wg
