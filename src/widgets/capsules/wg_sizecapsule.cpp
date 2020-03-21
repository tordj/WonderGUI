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
#include <wg_internal.h>

#include <algorithm>

namespace wg
{
	using namespace Util;

	const TypeInfo SizeCapsule::TYPEINFO = { "SizeCapsule", &Capsule::TYPEINFO };

	//____ constructor ____________________________________________________________

	SizeCapsule::SizeCapsule()
	{
	}

	//____ Destructor _____________________________________________________________

	SizeCapsule::~SizeCapsule()
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& SizeCapsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setPreferredSize() _____________________________________________________

	void SizeCapsule::setPreferredSize( Size _size )
	{
		SizeI size = qpixToRawAligned(_size);
		if( size != m_preferred )
		{
			m_preferred = size;
			_requestResize();
		}
	}

	//____ setSizes() _____________________________________________________________

	bool SizeCapsule::setSizes( Size _min, Size _preferred, Size _max )
	{
		if ( (_preferred.w >= 0 && (_preferred.w > _max.w || _preferred.w < _min.w )) ||
			 (_preferred.h >= 0 && (_preferred.h > _max.h || _preferred.h < _min.h )) )
			return false;

		SizeI min = qpixToRawAligned(_min);
		SizeI preferred = qpixToRawAligned(_preferred);
		SizeI max = qpixToRawAligned(_max);

		if (min != m_min || preferred != m_preferred || max != m_max)
		{
			m_min = min;
			m_preferred = preferred;
			m_max = max;
			_requestResize();
		}
		return true;
	}

	//____ setMinSize() ___________________________________________________________

	void SizeCapsule::setMinSize( Size _size )
	{
		//TODO: Add error handling.

		SizeI size = qpixToRawAligned(_size);
		if( size != m_min )
		{
			m_min = size;
			_requestResize();
		}
	}

	//____ setMaxSize() ___________________________________________________________

	void SizeCapsule::setMaxSize( Size _size )
	{
		//TODO: Add error handling.

		SizeI size = qpixToRawAligned(_size);
		if( size != m_max )
		{
			m_max = size;
			_requestResize();
		}
	}

	//____ _preferredSize() ________________________________________________________

	SizeI SizeCapsule::_preferredSize() const
	{
		if (!slot._widget())
		{
			SizeI size = { std::max(0, m_preferred.w), std::max(0, m_preferred.h) };
			if (m_pSkin)
				size += m_pSkin->_contentPaddingSize();

			return size;
		}

		//

		SizeI pref;

		if (m_preferred.w >= 0)
		{
			// Preferred width is forced, we only need to adapt height.

			pref.w = m_preferred.w;
			if (m_preferred.h >= 0)
				pref.h = m_preferred.h;
			else
			{
				pref.h = _matchingHeight(pref.w);

				if (pref.h > m_max.h)
					pref.h = m_max.h;
				if (pref.h < m_min.h)
					pref.h = m_min.h;
			}
		}
		else if (m_preferred.h >= 0)
		{
			// Preferred height is forced, we only need to adapt width.

			pref.h = m_preferred.w;
			pref.w = _matchingWidth(pref.h);

			if (pref.w > m_max.w)
				pref.w = m_max.w;
			if (pref.w < m_min.w)
				pref.w = m_min.w;
		}
		else
		{
			// Preferred size not set in size capsule.
			// We take preferred from child and check against our min/max.

			pref = OO(slot)._preferredSize();

			if (pref.w > m_max.w && pref.h > m_max.h)
			{
				// Both width and height surpasses our max, get matching values for both and limit them to our min.

				int matchW = _matchingWidth(m_max.h);
				int matchH = _matchingHeight(m_max.w);

				matchW = std::max(matchW, m_min.w);
				matchH = std::max(matchH, m_min.h);

				//

				if (matchW > m_max.w && matchH > m_max.h)
					pref = m_max;							// Both matching values are too big, so we will max in both dimensions.
				else if (matchW <= m_max.w && matchH <= m_max.h)
				{
					// Both matching values are below max, so we can go either way here.
					// Lets choose the one that gives the largest area.

					if (matchW*m_max.h > matchH*m_max.w)
					{
						pref.w = matchW;
						pref.h = m_max.h;
					}
					else
					{
						pref.w = m_max.w;
						pref.h = matchH;
					}
				}
				else
				{
					// If we get here, only one matching value is small enough.

					if (matchW <= m_max.w)
					{
						pref.w = matchW;
						pref.h = m_max.h;
					}
					else
					{
						pref.w = m_max.w;
						pref.h = m_max.h;
					}
				}

			}
			else if (pref.w > m_max.w)
			{
				// Only width surpasses our max.

				pref.w = m_max.w;
				pref.h = _matchingHeight(pref.w);

				if (pref.h > m_max.h)
					pref.h = m_max.h;
				if (pref.h < m_min.h)
					pref.h = m_min.h;
			}
			else if (pref.h > m_max.h)
			{
				// Only height surpasses our max

				pref.h = m_max.h;
				pref.w = _matchingWidth(pref.h);

				if (pref.w > m_max.w)
					pref.w = m_max.w;
				if (pref.w < m_min.w)
					pref.w = m_min.w;

			}
			else
			{
				// Neither dimension surpasses our max, let's check against our min values.

				if (pref.w < m_min.w)
				{
					pref.w = m_min.w;
					pref.h = _matchingHeight(pref.w);

					if (pref.h > m_max.h)
						pref.h = m_max.h;
					if (pref.h < m_min.h)
						pref.h = m_min.h;
				}
				else if (pref.h < m_min.h)
				{
					pref.h = m_min.h;
					pref.w = _matchingWidth(pref.h);

					if (pref.w > m_max.w)
						pref.w = m_max.w;
					if (pref.w < m_min.w)
						pref.w = m_min.w;
				}
			}
		}

		if (m_pSkin)
			pref += m_pSkin->_contentPaddingSize();

		return pref;
	}

	//____ _minSize() ______________________________________________________________

	SizeI SizeCapsule::_minSize() const
	{
		if( slot._widget() )
			return SizeI::max(m_min,OO(slot)._minSize());
		else
			return m_min;
	}

	//____ _maxSize() ______________________________________________________________

	SizeI SizeCapsule::_maxSize() const
	{
		if( slot._widget() )
			return SizeI::min(m_max,OO(slot)._maxSize());
		else
			return m_max;
	}

	//____ _matchingHeight() _______________________________________________________

	int SizeCapsule::_matchingHeight( int width ) const
	{
		if( m_preferred.h >= 0 )
		{
			int h = m_preferred.h;

			if( slot._widget() )
			{
				int max = OO(slot)._maxSize().h;
				int min = OO(slot)._minSize().h;
				limit( h, min, max );
			}
			return h;
		}
		else if( slot._widget() )
		{
			int h = OO(slot)._matchingHeight(width);
			limit( h, m_min.h, m_max.h );
			return h;
		}
		else
			return m_min.h;
	}

	//____ _matchingWidth() _______________________________________________________

	int SizeCapsule::_matchingWidth( int height ) const
	{
		if( m_preferred.w >= 0 )
		{
			int w = m_preferred.w;

			if( slot._widget() )
			{
				int max = OO(slot)._maxSize().w;
				int min = OO(slot)._minSize().w;
				limit( w, min, max );
			}
			return w;
		}
		else if( slot._widget() )
		{
			int w = OO(slot)._matchingWidth(height);
			limit( w, m_min.w, m_max.w );
			return w;
		}
		else
			return m_min.w;
	}


} // namespace wg
