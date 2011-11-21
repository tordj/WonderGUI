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

#include <wg_iconholder.h>
#include <wg_util.h>

//____ Constructor ____________________________________________________________

WgIconHolder::WgIconHolder()
{
	m_iconOrientation	= WG_NORTHWEST;
	m_iconScale			= 0.f;
	m_bIconPushText		= true;
}

//____ SetIconScale() _________________________________________________________

bool WgIconHolder::SetIconScale( float scaleFactor )
{
	if( scaleFactor > 1.f || scaleFactor < 0.f )
		return false;

	if( scaleFactor != m_iconScale )
	{
		m_iconScale = scaleFactor;
		_iconModified();
	}

	return true;
}

//____ SetIconOrientation() ___________________________________________________

void WgIconHolder::SetIconOrientation( WgOrientation orientation )
{
	if( orientation != m_iconOrientation )
	{
		m_iconOrientation = orientation;
		_iconModified();
	}
}

//____ SetIconBorders() _______________________________________________________

void WgIconHolder::SetIconBorders( WgBorders borders )
{
	if( borders != m_iconBorders )
	{
		m_iconBorders = borders;
		_iconModified();
	}
}

//____ SetIconPushingText() _________________________________________________________

void WgIconHolder::SetIconPushingText( bool bPush )
{
	if( bPush != m_bIconPushText )
	{
		m_bIconPushText = bPush;
		_iconModified();
	}
}


//____ _getIconRect() _________________________________________________________

/*
	Gets an icon-rect for the icon excluding borders, relative to upper left corner of content area.

*/

WgRect WgIconHolder::_getIconRect( const WgRect& contentRect, const WgBlockSetPtr& pBlocks ) const
{
	WgRect rect;

	if( pBlocks )
	{
		int w = pBlocks->Width();
		int h = pBlocks->Height();

		int bgW = contentRect.w - m_iconBorders.Width();
		int bgH = contentRect.h - m_iconBorders.Height();

		if( m_iconScale != 0.f )
		{
			if( (w / (float) bgW) > (h / (float) bgH) )
			{
				h = (int) ((h * bgW * m_iconScale) / h);
				w = (int) (bgW * m_iconScale);
			}
			else
			{
				w = (int) ((w * bgH * m_iconScale) / h);
				h = (int) (bgH * m_iconScale);
			}
		}

		// 

		rect = WgUtil::OrientationToRect( m_iconOrientation, contentRect.Size(), WgSize(w,h) );
		rect += contentRect.Pos();
	}

	return rect;
}

//____ _getTextRect() _____________________________________________________

WgRect WgIconHolder::_getTextRect( const WgRect& contentRect, const WgRect& iconRect ) const
{
	WgRect textRect = contentRect;

	if( m_bIconPushText && iconRect.w > 0 && iconRect.h > 0 )
	{
		switch( m_iconOrientation )
		{
			case WG_NORTHWEST:
			case WG_SOUTHWEST:
			case WG_WEST:
			{
				int diff = iconRect.x + iconRect.w + m_iconBorders.right;
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
				textRect.w = iconRect.x - m_iconBorders.left;
				if( textRect.w < 0 )
					textRect.w = 0;
				break;
			}

			case WG_NORTH:
			case WG_CENTER:
			{
				int diff = iconRect.y + iconRect.h + m_iconBorders.bottom;
				textRect.y += diff;
				textRect.h -= diff;
				if( textRect.h < 0 )
					textRect.h = 0;
				break;
			}
			case WG_SOUTH:
			{
				textRect.h = iconRect.y - m_iconBorders.top;
				if( textRect.h < 0 )
					textRect.h = 0;
				break;
			}
		}
	}

	return textRect;
}

