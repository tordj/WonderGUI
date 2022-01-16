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
	const TypeInfo CIconDisplay::TYPEINFO = { "CIconDisplay", &WidgetComponent::TYPEINFO };

	using namespace Util;

	//____ constructor ____________________________________________________________

	CIconDisplay::CIconDisplay( Widget * pWidget ) : WidgetComponent(pWidget)
	{
		m_placement		= Placement::NorthWest;
		m_scaleFactor	= 0.f;
		m_bOverlap		= true;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CIconDisplay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ set() ___________________________________________________________________

	bool CIconDisplay::set( Skin * pSkin, Placement placement, Border padding, float scaleFactor, bool bOverlap )
	{
		m_pSkin 	= pSkin;
		m_placement	= placement;
		m_padding 	= padding;
		m_scaleFactor = scaleFactor;
		m_bOverlap 	= bOverlap;

		_requestResize();
		return true;
	}

	//____ clear() _________________________________________________________________

	void CIconDisplay::clear()
	{
		m_pSkin 	= 0;
		m_placement	= Placement::West;
		m_padding 	= Border();
		m_scaleFactor	= 0.f;
		m_bOverlap 	= false;

		_requestResize();
	}

	//____ _setScaleFactor() _________________________________________________________

	bool CIconDisplay::_setScaleFactor( float scaleFactor )
	{
		if( scaleFactor > 1.f || scaleFactor < 0.f )
			return false;

		if( scaleFactor != m_scaleFactor )
		{
			m_scaleFactor = scaleFactor;
			_requestResize();
		}

		return true;
	}

	//____ _setPlacement() ___________________________________________________

	void CIconDisplay::_setPlacement( Placement placement )
	{
		if( placement != m_placement )
		{
			m_placement = placement;
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

	RectSPX CIconDisplay::_getIconRect( const RectSPX& contentRect, int scale ) const
	{
		if( m_pSkin )
			return _getIconRect(contentRect, m_pSkin->_preferredSize(scale), scale);
		else
			return RectSPX();
	}

	RectSPX CIconDisplay::_getIconRect( const RectSPX& contentRect, const SizeSPX& iconSize, int scale ) const
	{
		RectSPX rect;

		spx w = iconSize.w;
		spx h = iconSize.h;

		BorderSPX padding = align(ptsToSpx(m_padding, scale));

		if( w > 0 && h > 0 )
		{
			spx bgW = contentRect.w - padding.width();
			spx bgH = contentRect.h - padding.height();

			if( m_scaleFactor != 0.f )
			{
				if( (w / (float) bgW) > (h / (float) bgH) )
				{
					h = (spx) ((h * bgW * m_scaleFactor) / w);
					w = (spx) (bgW * m_scaleFactor);
				}
				else
				{
					w = (spx) ((w * bgH * m_scaleFactor) / h);
					h = (spx) (bgH * m_scaleFactor);
				}
			}

			//

			w += padding.width();
			h += padding.height();

			rect = Util::placementToRect( m_placement, contentRect.size(), SizeSPX(w,h) );
			rect += contentRect.pos();
			rect -= padding;
		}

		return rect;
	}



	//____ _getTextRect() _____________________________________________________

	RectSPX CIconDisplay::_getTextRect( const RectSPX& contentRect, const RectSPX& iconRect, int scale ) const
	{
		RectSPX textRect = contentRect;

		if( !m_bOverlap && iconRect.w > 0 && iconRect.h > 0 )
		{
			BorderSPX padding = align(ptsToSpx(m_padding, scale));

			switch( m_placement )
			{
				case Placement::NorthWest:
				case Placement::SouthWest:
				case Placement::West:
				{
					spx diff = iconRect.x - contentRect.x + iconRect.w + padding.right;
					textRect.x += diff;
					textRect.w -= diff;
					if( textRect.w < 0 )
						textRect.w = 0;
					break;
				}
				case Placement::NorthEast:
				case Placement::East:
				case Placement::SouthEast:
				{
					textRect.w = iconRect.x - contentRect.x - padding.left;
					if( textRect.w < 0 )
						textRect.w = 0;
					break;
				}

				case Placement::North:
				case Placement::Center:
				{
					int diff = iconRect.y - contentRect.y + iconRect.h + padding.bottom;
					textRect.y += diff;
					textRect.h -= diff;
					if( textRect.h < 0 )
						textRect.h = 0;
					break;
				}
				case Placement::South:
				{
					textRect.h = iconRect.y - contentRect.y - padding.top;
					if( textRect.h < 0 )
						textRect.h = 0;
					break;
				}
			}
		}

		return align(textRect);
	}

	//____ _preferredSize() ________________________________________________________

	SizeSPX CIconDisplay::_preferredSize(int scale) const
	{
		if( m_pSkin )
			return m_pSkin->_preferredSize(scale) + align(ptsToSpx(m_padding,scale));

		return SizeSPX();
	}


} // namespace wg
