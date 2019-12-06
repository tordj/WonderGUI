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

	//____ Constructor ____________________________________________________________

	CIconDisplay::CIconDisplay( GeoComponent::Holder * pHolder ) : GeoComponent(pHolder)
	{
		m_origo			= Origo::NorthWest;
		m_scale			= 0.f;
		m_bOverlap		= true;
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

	//____ setScale() _________________________________________________________

	bool CIconDisplay::setScale( float scaleFactor )
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

	//____ setOrigo() ___________________________________________________

	void CIconDisplay::setOrigo( Origo origo )
	{
		if( origo != m_origo )
		{
			m_origo = origo;
			_requestRender();
		}
	}

	//____ setPadding() _______________________________________________________

	void CIconDisplay::setPadding( BorderI borders )
	{
		if( borders != m_padding )
		{
			m_padding = borders;
			_requestResize();
		}
	}

	//____ setOverlap() _________________________________________________________

	void CIconDisplay::setOverlap( bool bOverlap )
	{
		if( bOverlap != m_bOverlap )
		{
			m_bOverlap = bOverlap;
			_requestResize();
		}
	}

	//____ setSkin() ______________________________________________________________

	void CIconDisplay::setSkin( Skin * pSkin )
	{
		if( pSkin != m_pSkin )
		{
			m_pSkin = pSkin;
			_requestResize();
		}
	}

	//____ getIconRect() _________________________________________________________

	/*
		Gets an icon-rect for the icon excluding borders, relative to upper left corner of content area.

	*/

	RectI CIconDisplay::getIconRect( const RectI& contentRect ) const
	{
		if( m_pSkin )
			return getIconRect(contentRect, m_pSkin->_preferredSize());
		else
			return RectI();
	}

	RectI CIconDisplay::getIconRect( const RectI& contentRect, const SizeI& iconSize ) const
	{
		RectI rect;

		int w = iconSize.w;
		int h = iconSize.h;

		if( w > 0 && h > 0 )
		{
			int bgW = contentRect.w - m_padding.width();
			int bgH = contentRect.h - m_padding.height();

			if( m_scale != 0.f )
			{
				if( (w / (float) bgW) > (h / (float) bgH) )
				{
					h = (int) ((h * bgW * m_scale) / w);
					w = (int) (bgW * m_scale);
				}
				else
				{
					w = (int) ((w * bgH * m_scale) / h);
					h = (int) (bgH * m_scale);
				}
			}

			//

			w += m_padding.width();
			h += m_padding.height();

			rect = Util::origoToRect( m_origo, contentRect.size(), SizeI(w,h) );
			rect += contentRect.pos();
			rect -= m_padding;
		}

		return rect;
	}



	//____ getTextRect() _____________________________________________________

	RectI CIconDisplay::getTextRect( const RectI& contentRect, const RectI& iconRect ) const
	{
		RectI textRect = contentRect;

		if( !m_bOverlap && iconRect.w > 0 && iconRect.h > 0 )
		{
			switch( m_origo )
			{
				case Origo::NorthWest:
				case Origo::SouthWest:
				case Origo::West:
				{
					int diff = iconRect.x - contentRect.x + iconRect.w + m_padding.right;
					textRect.x += diff;
					textRect.w -= diff;
					if( textRect.w < 0 )
						textRect.w = 0;
					break;
				}
				case Origo::NorthEast:
				case Origo::East:
				case Origo::SouthEast:
				{
					textRect.w = iconRect.x - contentRect.x - m_padding.left;
					if( textRect.w < 0 )
						textRect.w = 0;
					break;
				}

				case Origo::North:
				case Origo::Center:
				{
					int diff = iconRect.y - contentRect.y + iconRect.h + m_padding.bottom;
					textRect.y += diff;
					textRect.h -= diff;
					if( textRect.h < 0 )
						textRect.h = 0;
					break;
				}
				case Origo::South:
				{
					textRect.h = iconRect.y - contentRect.y - m_padding.top;
					if( textRect.h < 0 )
						textRect.h = 0;
					break;
				}
			}
		}

		return textRect;
	}

	//____ onCloneContent() ________________________________________________________

	void CIconDisplay::onCloneContent( const CIconDisplay * _pOrg )
	{
		m_origo			= _pOrg->m_origo;
		m_scale			= _pOrg->m_scale;
		m_bOverlap		= _pOrg->m_bOverlap;
		m_padding		= _pOrg->m_padding;
		m_pSkin			= _pOrg->m_pSkin;
	}

	//____ preferredSize() ________________________________________________________

	SizeI CIconDisplay::preferredSize() const
	{
		if( m_pSkin )
			return m_pSkin->_preferredSize() + m_padding;

		return SizeI();
	}


} // namespace wg
