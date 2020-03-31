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

#include <wg_cicondisplay.h>
#include <wg_skin.h>
#include <wg_util.h>

namespace wg
{
	const TypeInfo CIconDisplay::TYPEINFO = { "CIconDisplay", &GeoComponent::TYPEINFO };

	//____ constructor ____________________________________________________________

	CIconDisplay::CIconDisplay( GeoComponent::Holder * pHolder ) : GeoComponent(pHolder)
	{
		m_origo			= Origo::NorthWest;
		m_scale			= 0.f;
		m_bOverlap		= true;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CIconDisplay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ set() ___________________________________________________________________

	bool CIconDisplay::set( Skin * pSkin, Origo origo, BorderI padding, float scale, bool bOverlap )
	{
		m_pSkin 	= pSkin;
		m_origo 	= origo;
		m_padding 	= padding;
		m_scale 	= scale;
		m_bOverlap 	= bOverlap;

		_requestResize();
		return true;
	}

	//____ clear() _________________________________________________________________

	void CIconDisplay::clear()
	{
		m_pSkin 	= 0;
		m_origo 	= Origo::West;
		m_padding 	= BorderI(0);
		m_scale 	= 0.f;
		m_bOverlap 	= false;

		_requestResize();
	}

	//____ _setScale() _________________________________________________________

	bool CIconDisplay::_setScale( float scaleFactor )
	{
		if( scaleFactor > 1.f || scaleFactor < 0.f )
			return false;

		if( scaleFactor != m_scale )
		{
			m_scale = scaleFactor;
			_requestResize();
		}

		return true;
	}

	//____ _setOrigo() ___________________________________________________

	void CIconDisplay::_setOrigo( Origo origo )
	{
		if( origo != m_origo )
		{
			m_origo = origo;
			_requestRender();
		}
	}

	//____ _setPadding() _______________________________________________________

	void CIconDisplay::_setPadding( Border borders )
	{
		if( borders != m_padding )
		{
			m_padding = borders;
			_requestResize();
		}
	}

	//____ _setOverlap() _________________________________________________________

	void CIconDisplay::_setOverlap( bool bOverlap )
	{
		if( bOverlap != m_bOverlap )
		{
			m_bOverlap = bOverlap;
			_requestResize();
		}
	}

	//____ _setSkin() ______________________________________________________________

	void CIconDisplay::_setSkin( Skin * pSkin )
	{
		if( pSkin != m_pSkin )
		{
			m_pSkin = pSkin;
			_requestResize();
		}
	}

	//____ _getIconRect() _________________________________________________________

	/*
		Gets an icon-rect for the icon excluding borders, relative to upper left corner of content area.

	*/

	Rect CIconDisplay::_getIconRect( const Rect& contentRect ) const
	{
		if( m_pSkin )
			return _getIconRect(contentRect, m_pSkin->preferredSize());
		else
			return RectI();
	}

	Rect CIconDisplay::_getIconRect( const Rect& contentRect, const Size& iconSize ) const
	{
		Rect rect;

		MU w = iconSize.w;
		MU h = iconSize.h;

		if( w.qpix > 0 && h.qpix > 0 )
		{
			MU bgW = contentRect.w - m_padding.width();
			MU bgH = contentRect.h - m_padding.height();

			if( m_scale != 0.f )
			{
				if( (w.qpix / (float) bgW.qpix) > (h.qpix / (float) bgH.qpix) )
				{
					h = (MU) ((h * bgW * m_scale) / w);
					w = (MU) (bgW * m_scale);
				}
				else
				{
					w = (MU) ((w * bgH * m_scale) / h);
					h = (MU) (bgH * m_scale);
				}
			}

			//

			w += m_padding.width();
			h += m_padding.height();

			rect = Util::origoToRect( m_origo, contentRect.size(), Size(w,h) );
			rect += contentRect.pos();
			rect -= m_padding;
		}

		return rect;
	}



	//____ _getTextRect() _____________________________________________________

	Rect CIconDisplay::_getTextRect( const Rect& contentRect, const Rect& iconRect ) const
	{
		Rect textRect = contentRect;

		if( !m_bOverlap && iconRect.w.qpix > 0 && iconRect.h.qpix > 0 )
		{
			switch( m_origo )
			{
				case Origo::NorthWest:
				case Origo::SouthWest:
				case Origo::West:
				{
					MU diff = iconRect.x - contentRect.x + iconRect.w + m_padding.right;
					textRect.x += diff;
					textRect.w -= diff;
					if( textRect.w.qpix < 0 )
						textRect.w.qpix = 0;
					break;
				}
				case Origo::NorthEast:
				case Origo::East:
				case Origo::SouthEast:
				{
					textRect.w = iconRect.x - contentRect.x - m_padding.left;
					if( textRect.w.qpix < 0 )
						textRect.w.qpix = 0;
					break;
				}

				case Origo::North:
				case Origo::Center:
				{
					int diff = iconRect.y - contentRect.y + iconRect.h + m_padding.bottom;
					textRect.y += diff;
					textRect.h -= diff;
					if( textRect.h.qpix < 0 )
						textRect.h.qpix = 0;
					break;
				}
				case Origo::South:
				{
					textRect.h = iconRect.y - contentRect.y - m_padding.top;
					if( textRect.h.qpix < 0 )
						textRect.h.qpix = 0;
					break;
				}
			}
		}

		return textRect;
	}

	//____ _onCloneContent() ________________________________________________________

	void CIconDisplay::_onCloneContent( const CIconDisplay * _pOrg )
	{
		m_origo			= _pOrg->m_origo;
		m_scale			= _pOrg->m_scale;
		m_bOverlap		= _pOrg->m_bOverlap;
		m_padding		= _pOrg->m_padding;
		m_pSkin			= _pOrg->m_pSkin;
	}

	//____ _preferredSize() ________________________________________________________

	Size CIconDisplay::_preferredSize() const
	{
		if( m_pSkin )
			return m_pSkin->preferredSize() + m_padding;

		return Size();
	}


} // namespace wg
