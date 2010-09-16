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


#include <wg_item_pixmap.h>
#include <wg_gfx.h>


static const char	Wdg_Type[] = {"TordJ/WgItemPixmap"};

const char * WgItemPixmap::Type( void ) const
{
	return GetMyType();
}

const char * WgItemPixmap::GetMyType()
{
	return Wdg_Type;
}

//____ WgItemPixmap() _________________________________________________________

WgItemPixmap::WgItemPixmap( ) :
	WgItem(0),
	m_margin(0),
	m_forcedSize(0, 0)
{
	UpdateSize();
}

WgItemPixmap::WgItemPixmap( Sint64 id ) :
	WgItem(id),
	m_margin(0),
	m_forcedSize(0, 0)
{
	UpdateSize();
}

WgItemPixmap::WgItemPixmap( Sint64 id, WgBorders margin ) :
	WgItem(id),
	m_margin(margin),
	m_forcedSize(0, 0)
{
	UpdateSize();
}

WgItemPixmap::WgItemPixmap( Sint64 id, WgBorders margin, WgBlockSetPtr block ) :
	WgItem(id),
	m_block(block),
	m_margin(margin),
	m_forcedSize(0, 0)
{
	UpdateSize();
}

//____ ~WgItemPixmap() ________________________________________________________

WgItemPixmap::~WgItemPixmap()
{
}

void WgItemPixmap::SetSource( WgBlockSetPtr block )
{
	m_block = block;
	UpdateSize();
}

void WgItemPixmap::SetMargin( WgBorders margin )
{
	m_margin = margin;
	UpdateSize();
}

//____ SetOrigo() _______________________________________________________________
void WgItemPixmap::SetOrigo( const WgOrigo& origo )
{
	m_origo = origo;
	Modified(0,0);
}

//____ ForceSize() _______________________________________________________________
void WgItemPixmap::ForceSize( WgSize size )
{
	m_forcedSize = size;
	UpdateSize();
}

//____ UpdateSize() _______________________________________________________________
void WgItemPixmap::UpdateSize()
{
	if(!m_block)
		return;

	Uint32 width = m_margin.left + m_margin.right;
	Uint32 height = m_margin.top + m_margin.bottom;

	if( m_forcedSize.w > 0 )
		width += m_forcedSize.w;
	else
		width += m_block->GetWidth();

	if( m_forcedSize.h > 0 )
		height += m_forcedSize.h;
	else
		height += m_block->GetHeight();

	Sint32 widthModif = width - m_width;
	Sint32 heightModif = height - m_height;

	m_width = width;
	m_height = height;

	if( 0 != widthModif || 0 != heightModif )
		Modified(widthModif, heightModif);
}

//____ Render() _______________________________________________________________
void WgItemPixmap::Render( const WgRect& _window, const WgRect& _clip )
{
	if( m_block && m_block->GetSurface() )
	{
		WgRect r = _window;
		r.x += m_margin.left;
		r.y += m_margin.top;
		r.w -= m_margin.left + m_margin.right;
		r.h -= m_margin.top + m_margin.bottom;

		WgSize contentSize( r.w, r.h );
		if( m_forcedSize.w > 0 )
			contentSize.w = m_forcedSize.w;
		if( m_forcedSize.h > 0 )
			contentSize.h = m_forcedSize.h;

		r.x += m_origo.calcOfsX( r.w,contentSize.w );
		r.y += m_origo.calcOfsY( r.h,contentSize.h );
		
		r.w = contentSize.w;
		r.h = contentSize.h;
		
		WgGfx::clipBlitBlock( _clip, m_block->GetBlock(m_mode), r);
	}
}

//____ Clone() ________________________________________________________________

WgItem* WgItemPixmap::Clone( WgItem * _pClone )
{
	//TODO: This method doesn't work if we get a vaild object as a parameter,
	//      so consequently we can't use this as a super-class.

	WgItemPixmap * pClone = (WgItemPixmap*) _pClone;

	if( !pClone )
		pClone = new WgItemPixmap( m_id, m_margin, m_block );

	pClone->ForceSize(m_forcedSize);


	return WgItem::Clone(pClone);

}

