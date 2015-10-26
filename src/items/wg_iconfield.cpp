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

namespace wg 
{
	
	//____ Constructor ____________________________________________________________
	
	IconField::IconField( IconHolder * pHolder ) : Field(pHolder)
	{
		m_origo			= Origo::NorthWest;
		m_scale			= 0.f;
		m_bOverlap		= true;
	}
	
	//____ set() ___________________________________________________________________
	
	bool IconField::set( const Skin_p& pSkin, Origo origo, Border padding, float scale, bool bOverlap )
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
	
	void IconField::clear()
	{
		m_pSkin 	= 0;
		m_origo 	= Origo::West;
		m_padding 	= Border(0);
		m_scale 	= 0.f;
		m_bOverlap 	= false;
	
		_onResize();
	}
	
	//____ setScale() _________________________________________________________
	
	bool IconField::setScale( float scaleFactor )
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
	
	void IconField::setOrigo( Origo origo )
	{
		if( origo != m_origo )
		{
			m_origo = origo;
			_onDirty();
		}
	}
	
	//____ setPadding() _______________________________________________________
	
	void IconField::setPadding( Border borders )
	{
		if( borders != m_padding )
		{
			m_padding = borders;
			_onResize();
		}
	}
	
	//____ setOverlap() _________________________________________________________
	
	void IconField::setOverlap( bool bOverlap )
	{
		if( bOverlap != m_bOverlap )
		{
			m_bOverlap = bOverlap;
			_onResize();
		}
	}
	
	//____ setSkin() ______________________________________________________________
	
	void IconField::setSkin( const Skin_p& pSkin )
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
	
	Rect IconField::getIconRect( const Rect& contentRect ) const
	{
		if( m_pSkin )
			return getIconRect(contentRect, m_pSkin->preferredSize());
		else
			return Rect();
	}
	
	Rect IconField::getIconRect( const Rect& contentRect, const Size& iconSize ) const
	{
		Rect rect;
	
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
	
			rect = WgUtil::origoToRect( m_origo, contentRect.size(), Size(w,h) );
			rect += contentRect.pos();
			rect -= m_padding;
		}
	
		return rect;
	}
	
	
	
	//____ getTextRect() _____________________________________________________
	
	Rect IconField::getTextRect( const Rect& contentRect, const Rect& iconRect ) const
	{
		Rect textRect = contentRect;
	
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
	
	void IconField::onCloneContent( const IconField * _pOrg )
	{
		m_origo			= _pOrg->m_origo;
		m_scale			= _pOrg->m_scale;
		m_bOverlap		= _pOrg->m_bOverlap;
		m_padding		= _pOrg->m_padding;
		m_pSkin			= _pOrg->m_pSkin;
	}
	
	//____ preferredSize() ________________________________________________________
	
	Size IconField::preferredSize() const
	{
		if( m_pSkin )
			return m_pSkin->preferredSize() + m_padding;
	
		return Size();
	}
	

} // namespace wg
