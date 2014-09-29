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

WgIconField::WgIconField()
{
	m_origo			= WG_NORTHWEST;
	m_scale			= 0.f;
	m_bOverlap		= true;
}

//____ Set() ___________________________________________________________________

bool WgIconField::Set( const WgSkinPtr& pSkin, WgOrigo origo, WgBorder padding, float scale, bool bOverlap )
{
	m_pSkin 	= pSkin;
	m_origo 	= origo;
	m_padding 	= padding;
	m_scale 	= scale;
	m_bOverlap 	= bOverlap;
	m_pHolder->_fieldModified(this);
	return true;
}

//____ Clear() _________________________________________________________________

void WgIconField::Clear()
{
	m_pSkin 	= 0;
	m_origo 	= WG_WEST;
	m_padding 	= WgBorder(0);
	m_scale 	= 0.f;
	m_bOverlap 	= false;
	m_pHolder->_fieldModified(this);	
}

//____ SetScale() _________________________________________________________

bool WgIconField::SetScale( float scaleFactor )
{
	if( scaleFactor > 1.f || scaleFactor < 0.f )
		return false;

	if( scaleFactor != m_scale )
	{
		m_scale = scaleFactor;
		m_pHolder->_fieldModified(this);
	}

	return true;
}

//____ SetOrigo() ___________________________________________________

void WgIconField::SetOrigo( WgOrigo origo )
{
	if( origo != m_origo )
	{
		m_origo = origo;
		m_pHolder->_fieldModified(this);
	}
}

//____ SetPadding() _______________________________________________________

void WgIconField::SetPadding( WgBorder borders )
{
	if( borders != m_padding )
	{
		m_padding = borders;
		m_pHolder->_fieldModified(this);
	}
}

//____ SetOverlap() _________________________________________________________

void WgIconField::SetOverlap( bool bOverlap )
{
	if( bOverlap != m_bOverlap )
	{
		m_bOverlap = bOverlap;
		m_pHolder->_fieldModified(this);
	}
}

//____ SetSkin() ______________________________________________________________

void WgIconField::SetSkin( const WgSkinPtr& pSkin )
{
	if( pSkin != m_pSkin )
	{
		m_pSkin = pSkin;
		m_pHolder->_fieldModified(this);
	}
}

//____ GetIconRect() _________________________________________________________

/*
	Gets an icon-rect for the icon excluding borders, relative to upper left corner of content area.

*/

WgRect WgIconField::GetIconRect( const WgRect& contentRect ) const
{
	if( m_pSkin )
		return GetIconRect(contentRect, m_pSkin->PreferredSize());
	else
		return WgRect();
}

WgRect WgIconField::GetIconRect( const WgRect& contentRect, const WgSize& iconSize ) const
{
	WgRect rect;

	int w = iconSize.w;
	int h = iconSize.h;

	if( w > 0 && h > 0 )
	{
		int bgW = contentRect.w - m_padding.Width();
		int bgH = contentRect.h - m_padding.Height();

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

		w += m_padding.Width();
		h += m_padding.Height();

		rect = WgUtil::OrigoToRect( m_origo, contentRect.Size(), WgSize(w,h) );
		rect += contentRect.Pos();
		rect -= m_padding;
	}

	return rect;
}



//____ GetTextRect() _____________________________________________________

WgRect WgIconField::GetTextRect( const WgRect& contentRect, const WgRect& iconRect ) const
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

//____ OnCloneContent() ________________________________________________________

void WgIconField::OnCloneContent( const WgIconField * _pOrg )
{
	m_origo			= _pOrg->m_origo;
	m_scale			= _pOrg->m_scale;
	m_bOverlap		= _pOrg->m_bOverlap;
	m_padding		= _pOrg->m_padding;
	m_pSkin			= _pOrg->m_pSkin;
}

//____ PreferredSize() ________________________________________________________

WgSize WgIconField::PreferredSize() const
{
	if( m_pSkin )
		return m_pSkin->PreferredSize() + m_padding;

	return WgSize();
}

