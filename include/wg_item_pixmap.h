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

#ifndef WG_ITEM_PIXMAP_DOT_H
#define WG_ITEM_PIXMAP_DOT_H

#include <wg_item.h>
#include <wg_surface.h>

#include <wg_tooltip.h>

class WgItemPixmap : public WgItem
{
public:
	WgItemPixmap( );
	WgItemPixmap( Sint64 id );
	WgItemPixmap( Sint64 id, WgBorders margin );
	WgItemPixmap( Sint64 id, WgBorders margin, WgBlockSetPtr source );
	~WgItemPixmap();

	void			SetSource( WgBlockSetPtr block );
	WgBlockSetPtr	GetSource() const { return m_block; }

	void		SetMargin( WgBorders margin );

	void		ForceSize( Uint32 width, Uint32 height )	{ ForceSize(WgSize(width, height)); }
	void		ForceSize( WgSize size );
	WgSize		GetForceSize() const						{ return m_forcedSize; }

	WgBorders	Margin() const								{ return m_margin; }
	Uint8		MarginLeft() const							{ return m_margin.left; }
	Uint8		MarginRight() const							{ return m_margin.right; }
	Uint8		MarginTop() const							{ return m_margin.top; }
	Uint8		MarginBottom() const						{ return m_margin.bottom; }

	DECLARE_TOOLTIP_SUPPORT();

	virtual const char *Type( void ) const;
	static const char * GetMyType();
protected:

	void			UpdateSize();

 	virtual void	Render( const WgRect& _window, const WgRect& _clip );
	virtual WgItem* Clone( WgItem * _pClone = 0 );

private:

	WgBlockSetPtr	m_block;
	WgBorders		m_margin;
	WgSize			m_forcedSize;
};


#endif // WG_ITEM_BITMAP_DOT_H

