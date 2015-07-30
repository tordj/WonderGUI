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

#include <wg_iconfield.h>
#include <wg_skin.h>
#include <wg_util.h>

//____ Constructor ____________________________________________________________

WgIconField::WgIconField( WgIconHolder * pHolder ) : WgField(pHolder)
{
	m_origo			= WG_NORTHWEST;
	m_scale			= 0.f;
	m_bOverlap		= true;
}

//____ set() ___________________________________________________________________

bool WgIconField::set( const WgSkinPtr& pSkin, WgOrigo origo, WgBorder padding, float scale, bool bOverlap )
{
	m_pSkin 	= pSkin;
	m_origo 	= origo;
	m_padding 	= padding;
	m_scale 	= scale;
	m_bOverlap 	= bOverlap;

	_onResize();
	return true;
}

//____ clear() _________________________________________________________________

void WgIconField::clear()
{
	m_pSkin 	= 0;
	m_origo 	= WG_WEST;
	m_padding 	= WgBorder(0);
	m_scale 	= 0.f;
	m_bOverlap 	= false;

	_onResize();
}

//____ setScale() _________________________________________________________

bool WgIconField::setScale( float scaleFactor )
{
	if( scaleFactor > 1.f || scaleFactor < 0.f )
		return false;

	if( scaleFactor != m_scale )
	{
		m_scale = scaleFactor;
		_onResize();
	}

	return true;
}

//____ setOrigo() ___________________________________________________

void WgIconField::setOrigo( WgOrigo origo )
{
	if( origo != m_origo )
	{
		m_origo = origo;
		_onDirty();
	}
}

//____ setPadding() _______________________________________________________

void WgIconField::setPadding( WgBorder borders )
{
	if( borders != m_padding )
	{
		m_padding = borders;
		_onResize();
	}
}

//____ setOverlap() _________________________________________________________

void WgIconField::setOverlap( bool bOverlap )
{
	if( bOverlap != m_bOverlap )
	{
		m_bOverlap = bOverlap;
		_onResize();
	}
}

//____ setSkin() ______________________________________________________________

void WgIconField::setSkin( const WgSkinPtr& pSkin )
{
	if( pSkin != m_pSkin )
	{
		m_pSkin = pSkin;
		_onResize();
	}
}

//____ getIconRect() _________________________________________________________

/*
	Gets an icon-rect for the icon excluding borders, relative to upper left corner of content area.

*/

WgRect WgIconField::getIconRect( const WgRect& contentRect ) const
{
	if( m_pSkin )
		return getIconRect(contentRect, m_pSkin->preferredSize());
	else
		return WgRect();
}

WgRect WgIconField::getIconRect( const WgRect& contentRect, const WgSize& iconSize ) const
{
	WgRect rect;

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

		rect = WgUtil::origoToRect( m_origo, contentRect.size(), WgSize(w,h) );
		rect += contentRect.pos();
		rect -= m_padding;
	}

	return rect;
}



//____ getTextRect() _____________________________________________________

WgRect WgIconField::getTextRect( const WgRect& contentRect, const WgRect& iconRect ) const
{
	WgRect textRect = contentRect;

	if( !m_bOverlap && iconRect.w > 0 && iconRect.h > 0 )
	{
		switch( m_origo )
		{
			case WG_NORTHWEST:
			case WG_SOUTHWEST:
			case WG_WEST:
			{
				int diff = iconRect.x - contentRect.x + iconRect.w + m_padding.right;
				textRect.x += diff;
				textRect.w -= diff;
				if( textRect.w < 0 )
					textRect.w = 0;
				break;
			}
			case WG_NORTHEAST:
			case WG_EAST:
			case WG_SOUTHEAST:
			{
				textRect.w = iconRect.x - contentRect.x - m_padding.left;
				if( textRect.w < 0 )
					textRect.w = 0;
				break;
			}

			case WG_NORTH:
			case WG_CENTER:
			{
				int diff = iconRect.y - contentRect.y + iconRect.h + m_padding.bottom;
				textRect.y += diff;
				textRect.h -= diff;
				if( textRect.h < 0 )
					textRect.h = 0;
				break;
			}
			case WG_SOUTH:
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

void WgIconField::onCloneContent( const WgIconField * _pOrg )
{
	m_origo			= _pOrg->m_origo;
	m_scale			= _pOrg->m_scale;
	m_bOverlap		= _pOrg->m_bOverlap;
	m_padding		= _pOrg->m_padding;
	m_pSkin			= _pOrg->m_pSkin;
}

//____ preferredSize() ________________________________________________________

WgSize WgIconField::preferredSize() const
{
	if( m_pSkin )
		return m_pSkin->preferredSize() + m_padding;

	return WgSize();
}

