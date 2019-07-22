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

	SizeCapsule_p SizeCapsule::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SizeCapsule_p( static_cast<SizeCapsule*>(pObject) );

		return 0;
	}


	//____ setPreferredSize() _____________________________________________________

	void SizeCapsule::setPreferredSize( SizeI size )
	{
		if( size != m_preferred )
		{
			m_preferred = size;
			_requestResize();
		}
	}

	//____ setSizes() _____________________________________________________________

	void SizeCapsule::setSizes( SizeI min, SizeI preferred, SizeI max )
	{
		m_min = min;
		m_preferred = preferred;
		m_max = max;
		_requestResize();
	}

	//____ setMinSize() ___________________________________________________________

	void SizeCapsule::setMinSize( SizeI size )
	{
		if( size != m_min )
		{
			m_min = size;
			_requestResize();
		}
	}

	//____ setMaxSize() ___________________________________________________________

	void SizeCapsule::setMaxSize( SizeI size )
	{
		if( size != m_max )
		{
			m_max = size;
			_requestResize();
		}
	}

	//____ preferredSize() ________________________________________________________

	SizeI SizeCapsule::preferredSize() const
	{
		if( m_child.pWidget )
		{
			SizeI pref = m_child.pWidget->preferredSize();

			if( m_preferred.w != 0 )
				pref.w = m_preferred.w;

			if( m_preferred.h != 0 )
				pref.h = m_preferred.h;

			// Constrain against min/max, taking MatchingWidth/MatchingHeight into account.
			//TODO: Check so we don't have any corner cases that breaks the constraints and
			// and that priorities between preferred height/width are reasonable.

			SizeI min = minSize();
			SizeI max = maxSize();

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
			return m_preferred;
		}
	}

	//____ minSize() ______________________________________________________________

	SizeI SizeCapsule::minSize() const
	{

		if( m_child.pWidget )
			return SizeI::max(m_min,m_child.pWidget->minSize());
		else
			return m_min;
	}

	//____ maxSize() ______________________________________________________________

	SizeI SizeCapsule::maxSize() const
	{
		if( m_child.pWidget )
			return SizeI::min(m_max,m_child.pWidget->maxSize());
		else
			return m_max;
	}

	//____ matchingHeight() _______________________________________________________

	int SizeCapsule::matchingHeight( int width ) const
	{
		if( m_preferred.h != 0 )
		{
			int h = m_preferred.h;

			if( m_child.pWidget )
			{
				int max = m_child.pWidget->maxSize().h;
				int min = m_child.pWidget->minSize().h;
				limit( h, min, max );
			}
			return h;
		}
		else if( m_child.pWidget )
		{
			int h = m_child.pWidget->matchingHeight(width);
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

			if( m_child.pWidget )
			{
				int max = m_child.pWidget->maxSize().w;
				int min = m_child.pWidget->minSize().w;
				limit( w, min, max );
			}
			return w;
		}
		else if( m_child.pWidget )
		{
			int w = m_child.pWidget->matchingWidth(height);
			limit( w, m_min.w, m_max.w );
			return w;
		}
		else
			return m_min.w;
	}


} // namespace wg
