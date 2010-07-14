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


#include <wg_item_folder.h>
#include <wg_gfx.h>
#include <wg_font.h>

static const char	Wdg_Type[] = {"TordJ/WgItemFolder"};

const char * WgItemFolder::Type( void ) const
{
	return GetMyType();
}

const char * WgItemFolder::GetMyType()
{
	return Wdg_Type;
}

WgItemFolder::WgItemFolder() :
	m_pSrcOpen(0),
	m_pSrcClosed(0),
	m_indent(0),
	m_labelBorders(0),
	m_pLabel(0)
{
	Init();
}

WgItemFolder::WgItemFolder(Uint32 id ) :
	WgItem(id),
	m_pSrcOpen(0),
	m_pSrcClosed(0),
	m_indent(0),
	m_labelBorders(0),
	m_pLabel(0)
{
	Init();
}

WgItemFolder::WgItemFolder(Uint32 id, WgBlockSetPtr _pOpen, WgBlockSetPtr _pClosed ) :
	WgItem(id),
	m_pSrcOpen(_pOpen),
	m_pSrcClosed(_pClosed),
	m_indent(0),
	m_labelBorders(0),
	m_pLabel(0)
{
	Init();
}

WgItemFolder::WgItemFolder(Uint32 id, WgBlockSetPtr _pOpen, WgBlockSetPtr _pClosed, Uint8 indentation ) :
	WgItem(id),
	m_pSrcOpen(_pOpen),
	m_pSrcClosed(_pClosed),
	m_indent(indentation),
	m_labelBorders(0),
	m_pLabel(0)
{
	Init();
}

WgItemFolder::WgItemFolder(Uint32 id, WgBlockSetPtr _pOpen, WgBlockSetPtr _pClosed, Uint8 indentation, const WgBorders& labelBorders, WgItem * pLabel ) :
	WgItem(id),
	m_pSrcOpen(_pOpen),
	m_pSrcClosed(_pClosed),
	m_indent(indentation),
	m_labelBorders(labelBorders),
	m_pLabel(pLabel)
{
	Init();
}

//____ Destructor _____________________________________________________________

WgItemFolder::~WgItemFolder()
{
	if( m_pLabel )
		delete m_pLabel;

}

//____ OpenFolder() ___________________________________________________________

void WgItemFolder::OpenFolder()
{
	if( !m_bIsOpen )
	{
		m_bIsOpen = true;
		m_width = m_openWidth;
		m_height = m_openHeight;
		Modified( m_openWidth - m_closedWidth, m_openHeight - m_closedHeight );
	}
}

//____ CloseFolder() __________________________________________________________

void WgItemFolder::CloseFolder()
{
	if( m_bIsOpen )
	{
		m_bIsOpen = false;
		m_width = m_closedWidth;
		m_height = m_closedHeight;
		Modified( m_closedWidth - m_openWidth, m_closedHeight - m_openHeight );
	}
}

//____ SetIndentation() _______________________________________________________

void WgItemFolder::SetIndentation( Uint8 pixels )
{
	if( pixels == m_indent )
		return;

	m_indent = pixels;

	WgSize sz = CalcOpenSize();

	if( m_bIsOpen )
	{
		m_width = sz.w;
		Modified(sz.w - m_openWidth,0);
	}
	m_openWidth = sz.w;
}

//____ SetOpenSource() _______________________________________________________

void WgItemFolder::SetOpenSource( WgBlockSetPtr _pOpen )
{
	m_pSrcOpen = _pOpen;

	WgSize sz = CalcClosedSize();

	if( sz.w != m_closedWidth || sz.h != m_closedHeight || m_bIsOpen )
		Modified( sz.w - m_closedWidth, sz.h - m_closedHeight );

	m_closedWidth = sz.w;
	m_closedHeight = sz.h;
	
	sz = CalcOpenSize();

	m_openWidth = sz.w;
	m_openHeight = sz.h;

	if( m_bIsOpen )
	{
		m_width = m_openWidth;
		m_height = m_openHeight;
	}
	else
	{
		m_width = m_closedWidth;
		m_height = m_closedHeight;
	}

}


//____ SetClosedSource() _______________________________________________________

void WgItemFolder::SetClosedSource( WgBlockSetPtr _pClosed )
{
	m_pSrcClosed = _pClosed;

	WgSize sz = CalcClosedSize();
	
	if( sz.w != m_closedWidth || sz.h != m_closedHeight || !m_bIsOpen )
		Modified( sz.w - m_closedWidth, sz.h - m_closedHeight );

	m_closedWidth = sz.w;
	m_closedHeight = sz.h;
	
	sz = CalcOpenSize();

	m_openWidth = sz.w;
	m_openHeight = sz.h;

	if( m_bIsOpen )
	{
		m_width = m_openWidth;
		m_height = m_openHeight;
	}
	else
	{
		m_width = m_closedWidth;
		m_height = m_closedHeight;
	}
}

//____ SetLabel() _____________________________________________________________

void WgItemFolder::SetLabel( WgItem * pItem )
{
	m_pLabel = pItem;

	WgSize sz = CalcClosedSize();
	Modified( sz.w - m_closedWidth, sz.h - m_closedHeight );
}


//____ AdaptToWidth() _________________________________________________________

void WgItemFolder::AdaptToWidth( Uint32 displayed_width )
{
	int w = displayed_width-m_indent;
	if( w < 1 )
		w = 1;

	// we have to store this new width.
	m_availableWidth = w;

	if( m_pLabel )
		m_pLabel->AdaptToWidth(displayed_width);

	AdaptItemsToWidth(w);
}


//____ ActionRespond() ________________________________________________________

void WgItemFolder::ActionRespond( WgEmitter * pEmitter, WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	if( !m_bEnabled )
		return;

	int		btn = _button_key-1;

	switch( _action )
	{
		case WgInput::BUTTON_PRESS:
		{
			if( btn == 0 )
			{
				if( m_bIsOpen )
					CloseFolder();
				else
					OpenFolder();
			}
			break;
		}

		case WgInput::POINTER_OVER:
		{
			if( !m_bMouseOver )
			{
				m_bMouseOver = true;
				Modified(0,0);
			}
			break;
		}

		case WgInput::POINTER_EXIT:
		{
			if( m_bMouseOver )
			{
				m_bMouseOver = false;
				Modified(0,0);
			}
			break;
		}

        default:
            break;

	}

	WgItem::ActionRespond( pEmitter, _action, _button_key, _info, _inputObj );
}

//____ Render() _______________________________________________________________

void WgItemFolder::Render( const WgRect& _window, const WgRect& _clip )
{
	if( !m_pSrcOpen || !m_pSrcClosed )
		return;

	WgBlockSetPtr blockset = m_bIsOpen ? m_pSrcOpen : m_pSrcClosed;

	WgMode mode;

	if( m_bMouseOver )
		mode = WG_MODE_MARKED;
	else
		mode = WG_MODE_NORMAL;

	WgRect r;

	r = _window;
	r.h = m_closedHeight;

	WgGfx::clipBlitBlock( _clip, blockset->GetBlock(mode), r );

	r.Shrink( m_labelBorders );
	m_pLabel->Render( r, _clip );

	if( m_bIsOpen )
	{
		r = _window;

		r.x += m_indent;
		r.w -= m_indent;
		r.y += blockset->GetHeight();

		WgItem * p = m_items.getFirst();

		while( p && r.y + (Sint32) p->Height() < _clip.y )
		{
			r.y += p->Height() + m_itemSpacing;
			p = p->getNext();
		}

		while( p && r.y < _clip.y + _clip.h )
		{
			r.h = p->Height();

			if( p->IsSelected() )
				WgGfx::clipFillRect( _clip, r, m_itemMarkColor );

			p->Render( r, _clip );
			r.y += p->Height() + m_itemSpacing;
			p = p->getNext();
		}
	}
}

//____ Clone() ________________________________________________________________

WgItem* WgItemFolder::Clone( WgItem * _pClone )
{

	WgItemFolder * pClone = (WgItemFolder*) _pClone;

	//TODO: Far from complete...
/*
	if( !pClone )
		pClone = new WgItemFolder( m_pSurf, m_srcOpen, m_srcClosed, m_tileOfs, m_tileLen, m_textOfsX, m_indent );


	Wg_Interface_TextHolder::CloneInterface( pClone );
*/

	return WgItem::Clone(pClone);


}

//____ Enable() _______________________________________________________________

void WgItemFolder::Enable( bool bEnable )
{
	if( bEnable == m_bEnabled )
		return;

	m_bEnabled = bEnable;

	if( m_pLabel )
		m_pLabel->Enable(bEnable);

	if( !m_bEnabled )
		CloseFolder();		// Disabled folder is always closed...
	else
		Modified( 0, 0 );
}

//____ refreshItems() _________________________________________________________

void WgItemFolder::refreshItems()
{
	WgSize sz = CalcOpenSize();

	m_openWidth = sz.w;
	m_openHeight = sz.h;

	if( m_bIsOpen )
	{
		m_width = sz.w;
		m_height = sz.h;
		Modified( sz.w - m_openWidth, sz.h - m_openHeight );
	}
}

//____ ItemAdded() _________________________________________________________

void WgItemFolder::ItemAdded( WgItem * pItem )
{
	pItem->AdaptToWidth( m_availableWidth );
	refreshItems();						// Slow way of doing this, can be optimized a lot.
}

//____ ItemModified() _________________________________________________________

void WgItemFolder::ItemModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff )
{
	refreshItems();		//TODO: Replace with more specialized code
}


//____ GetMarkedItem() ________________________________________________________

WgItem*	WgItemFolder::GetMarkedItem( Uint32 _x, Uint32 _y )
{
	Sint32	y = _y;

	if( !m_bIsOpen )
		return this;
	else
	{
		y -= m_closedHeight;
		if( y < 0 )
			return this;							// On the bar...

		WgItem * p = m_items.getFirst();

		while( p )
		{
			if( (int) p->Height() > y )
				return p->GetMarkedItem(_x, y);

			y -= p->Height() +  m_itemSpacing;

			if( y < 0 )
				return 0;							// Fell in spacing between items.

			p = p->getNext();
		}
	}
	return 0;				// Something is wrong...
}

//____ Init() _________________________________________________________________

void WgItemFolder::Init()
{
	WgSize sz = CalcClosedSize();

	m_openWidth		= sz.w;
	m_openHeight	= sz.h;

	m_closedWidth	= sz.w;
	m_closedHeight	= sz.h;

	m_bIsOpen		= false;
	m_width			= m_closedWidth;
	m_height		= m_closedHeight;

	m_bEnabled		= true;
	m_bMouseOver	= false;

	m_availableWidth = m_width;

}

//____ CalcOpenSize() ___________________________________________________

WgSize WgItemFolder::CalcOpenSize()
{
	WgSize size = CalcClosedSize();

	WgItem * p = m_items.getFirst();

	if( p )
		size.h -= m_itemSpacing;		// We will get one spacing too much...

	while( p )
	{
		if( (p->Width() + m_indent) > (Uint32) size.w )
			size.w = (p->Width() + m_indent);

		size.h += p->Height() + m_itemSpacing;
		p = p->getNext();
	}

	return size;
}


//____ CalcClosedSize() _________________________________________________

WgSize WgItemFolder::CalcClosedSize()
{
	WgSize	sz;

	if( m_pSrcOpen )
	{
		sz.w = m_pSrcOpen->GetWidth();
		sz.h = m_pSrcOpen->GetHeight();
	}
	else
	{
		sz.w = 0;
		sz.h = 0;
	}

	if( m_pSrcClosed )
	{
		if( sz.w < m_pSrcClosed->GetWidth() )
			sz.w = m_pSrcClosed->GetWidth();

		if( sz.h < m_pSrcClosed->GetHeight() )
			sz.h = m_pSrcClosed->GetHeight();
	}

	if( m_pLabel )
	{
		Sint32 w = (Sint32) (m_pLabel->Width() + m_labelBorders.GetWidth());
		Sint32 h = (Sint32) (m_pLabel->Height() + m_labelBorders.GetHeight());

		if( sz.w < w )
			sz.w = w;

		if( sz.h < h )
			sz.h = h;
	}

	return sz;
}
