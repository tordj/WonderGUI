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

	void SizeCapsule::setPreferredSize( Size size )
	{
		if( size != m_preferred )
		{
			m_preferred = size;
			_requestResize();
		}
	}

	//____ setSizes() _____________________________________________________________

	bool SizeCapsule::setSizes( Size min, Size preferred, Size max )
	{

		if ( (preferred.w >= 0 && (preferred.w > max.w || preferred.w < min.w )) ||
			 (preferred.h >= 0 && (preferred.h > max.h || preferred.h < min.h )) )
			return false;

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

	void SizeCapsule::setMinSize( Size size )
	{
		//TODO: Add error handling.

		if( size != m_min )
		{
			m_min = size;
			_requestResize();
		}
	}

	//____ setMaxSize() ___________________________________________________________

	void SizeCapsule::setMaxSize( Size size )
	{
		//TODO: Add error handling.

		if( size != m_max )
		{
			m_max = size;
			_requestResize();
		}
	}

	//____ _preferredSize() ________________________________________________________

	SizeSPX SizeCapsule::_preferredSize(int _scale) const
	{
		int scale = _fixScale(_scale);

		SizeSPX preferred	= align(ptsToSpx(m_preferred, scale));
		SizeSPX min			= align(ptsToSpx(m_min, scale));
		SizeSPX max			= align(ptsToSpx(m_max, scale));

		if (!slot._widget())
		{
			SizeSPX size = { std::max(0, preferred.w), std::max(0, preferred.h) };
			size += m_skin.contentPaddingSize(scale);

			return size;
		}

		//

		SizeSPX pref;

		if (preferred.w >= 0)
		{
			// Preferred width is forced, we only need to adapt height.

			pref.w = preferred.w;
			if (preferred.h >= 0)
				pref.h = preferred.h;
			else
			{
				pref.h = _matchingHeight(pref.w, scale);

				if (pref.h > max.h)
					pref.h = max.h;
				if (pref.h < min.h)
					pref.h = min.h;
			}
		}
		else if (preferred.h >= 0)
		{
			// Preferred height is forced, we only need to adapt width.

			pref.h = preferred.w;
			pref.w = _matchingWidth(pref.h, scale);

			if (pref.w > max.w)
				pref.w = max.w;
			if (pref.w < min.w)
				pref.w = min.w;
		}
		else
		{
			// Preferred size not set in size capsule.
			// We take preferred from child and check against our min/max.

			pref = slot._widget()->_preferredSize(_scale);

			if (pref.w > max.w && pref.h > max.h)
			{
				// Both width and height surpasses our max, get matching values for both and limit them to our min.

				spx matchW = _matchingWidth(max.h,scale);
				spx matchH = _matchingHeight(max.w,scale);

				matchW = std::max(matchW, min.w);
				matchH = std::max(matchH, min.h);

				//

				if (matchW > max.w && matchH > max.h)
					pref = max;							// Both matching values are too big, so we will max in both dimensions.
				else if (matchW <= max.w && matchH <= max.h)
				{
					// Both matching values are below max, so we can go either way here.
					// Lets choose the one that gives the largest area.

					if (matchW*max.h > matchH*max.w)
					{
						pref.w = matchW;
						pref.h = max.h;
					}
					else
					{
						pref.w = max.w;
						pref.h = matchH;
					}
				}
				else
				{
					// If we get here, only one matching value is small enough.

					if (matchW <= max.w)
					{
						pref.w = matchW;
						pref.h = max.h;
					}
					else
					{
						pref.w = max.w;
						pref.h = max.h;
					}
				}

			}
			else if (pref.w > max.w)
			{
				// Only width surpasses our max.

				pref.w = max.w;
				pref.h = _matchingHeight(pref.w,scale);

				if (pref.h > max.h)
					pref.h = max.h;
				if (pref.h < min.h)
					pref.h = min.h;
			}
			else if (pref.h > max.h)
			{
				// Only height surpasses our max

				pref.h = max.h;
				pref.w = _matchingWidth(pref.h,scale);

				if (pref.w > max.w)
					pref.w = max.w;
				if (pref.w < min.w)
					pref.w = min.w;

			}
			else
			{
				// Neither dimension surpasses our max, let's check against our min values.

				if (pref.w < min.w)
				{
					pref.w = min.w;
					pref.h = _matchingHeight(pref.w,scale);

					if (pref.h > max.h)
						pref.h = max.h;
					if (pref.h < min.h)
						pref.h = min.h;
				}
				else if (pref.h < min.h)
				{
					pref.h = min.h;
					pref.w = _matchingWidth(pref.h,scale);

					if (pref.w > max.w)
						pref.w = max.w;
					if (pref.w < min.w)
						pref.w = min.w;
				}
			}
		}

		pref += m_skin.contentPaddingSize(scale);

		return pref;
	}

	//____ _minSize() ______________________________________________________________

	SizeSPX SizeCapsule::_minSize(int scale) const
	{
		scale = _fixScale(scale);

		if( slot._widget() )
			return SizeSPX::max(align(ptsToSpx(m_min,scale)),slot._widget()->_minSize(scale));
		else
			return align(ptsToSpx(m_min, scale));
	}

	//____ _maxSize() ______________________________________________________________

	SizeSPX SizeCapsule::_maxSize(int scale) const
	{
		scale = _fixScale(scale);

		if( slot._widget() )
			return SizeSPX::min(align(ptsToSpx(m_max, scale)),slot._widget()->_maxSize(scale));
		else
			return align(ptsToSpx(m_max, scale));
	}

	//____ _matchingHeight() _______________________________________________________

	spx SizeCapsule::_matchingHeight( spx width, int scale ) const
	{
		if( m_preferred.h >= 0 )
		{
			scale = _fixScale(scale);

			pts h = m_preferred.h;

			if( slot._widget() )
			{
				pts max = slot._widget()->maxSize().h;
				pts min = slot._widget()->minSize().h;
				limit( h, min, max );
			}
			return h;
		}
		else if( slot._widget() )
		{
			pts h = slot._widget()->_matchingHeight(width,scale);
			limit( h, m_min.h, m_max.h );
			return h;
		}
		else
			return m_min.h;
	}

	//____ _matchingWidth() _______________________________________________________

	spx SizeCapsule::_matchingWidth( spx height, int scale ) const
	{

		if( m_preferred.w >= 0 )
		{
			scale = _fixScale(scale);

			pts w = m_preferred.w;

			if( slot._widget() )
			{
				pts max = slot._widget()->maxSize().w;
				pts min = slot._widget()->minSize().w;
				limit( w, min, max );
			}
			return w;
		}
		else if( slot._widget() )
		{
			pts w = slot._widget()->_matchingWidth(height,scale);
			limit( w, m_min.w, m_max.w );
			return w;
		}
		else
			return m_min.w;
	}


} // namespace wg
