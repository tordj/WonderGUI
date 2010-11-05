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


#include <wg_item_stack.h>

static const char	Wdg_Type[] = {"TordJ/WgItemStack"};

const char * WgItemStack::Type( void ) const
{
	return GetMyType();
}

const char * WgItemStack::GetMyType()
{
	return Wdg_Type;
}


//____ WgItemStack() ____________________________________________________________

WgItemStack::WgItemStack()
{
	m_minWidth = 0;
	m_minHeight = 0;
}

WgItemStack::WgItemStack( Sint64 id ) : WgItem( id )
{
	m_minWidth = 0;
	m_minHeight = 0;
}

WgItemStack::WgItemStack( Sint64 id, int minW, int minH ) : WgItem( id )
{
	m_minWidth = minW;
	m_minHeight = minH;
}

//____ ~WgItemStack() ___________________________________________________________

WgItemStack::~WgItemStack()
{
	m_itemOffsets.clear();
}

//____ AddItem() ______________________________________________________________

Uint32 WgItemStack::AddItem( WgItem * pItem, WgOrigo origo, int ofsX, int ofsY )
{
	Uint32 pos = Wg_Interface_ItemHolder::AddItem(pItem);
	m_itemOffsets.insert(pos, ItemOffset(origo, ofsX, ofsY));
	return pos;
}

//____ InsertItem() ___________________________________________________________

Uint32 WgItemStack::InsertItem( WgItem * pItem, Uint32 pos, WgOrigo origo, int ofsX, int ofsY )
{
	Uint32 insertedPos = Wg_Interface_ItemHolder::InsertItem(pItem, pos);
	m_itemOffsets.insert(insertedPos, ItemOffset(origo, ofsX, ofsY));
	return insertedPos;
}

//____ DeleteAllItems() ________________________________________________________
void WgItemStack::DeleteAllItems()
{
	m_itemOffsets.clear();
	Wg_Interface_ItemHolder::DeleteAllItems();
}

//____ AdaptToWidth() ________________________________________________________

void WgItemStack::AdaptToWidth( Uint32 displayed_width )
{
	WgItem * p = m_items.First();

	while( p )
	{
		p->AdaptToWidth(displayed_width);
		p = p->Next();
	}
	
	refreshItems();
}

//____ AdaptToHeight() ________________________________________________________

void WgItemStack::AdaptToHeight( Uint32 displayed_height )
{
	WgItem * p = m_items.First();

	while( p )
	{
		p->AdaptToHeight(displayed_height);
		p = p->Next();
	}			
	
	refreshItems();
}

//____ Render() _______________________________________________________________

void WgItemStack::Render( const WgRect& _window, const WgRect& _clip )
{
	WgRect r;

	r = _window;

	WgItem * p = m_items.First();

	for(Uint32 i = 0; p && i < m_itemOffsets.entries(); i++)
	{
		r.w = p->Width();
		r.h = p->Height();
		r.x = _window.x + m_itemOffsets[i].origo.calcOfsX(_window.w, r.w) + m_itemOffsets[i].ofsX;
		r.y = _window.y + m_itemOffsets[i].origo.calcOfsY(_window.h, r.h) + m_itemOffsets[i].ofsY;
		p->Render( r, _clip );
		p = p->Next();
	}
}

//____ Clone() ________________________________________________________________

WgItem* WgItemStack::Clone( WgItem * _pClone )
{
	WgItemStack * pClone = (WgItemStack*) _pClone;

	if( !pClone )
		pClone = new WgItemStack();

	//TODO: Implement cloning of content.


	return WgItem::Clone(pClone);
}

//____ Enable() _______________________________________________________________

void WgItemStack::Enable( bool bEnable )
{
	WgItem * p = m_items.First();

	while( p )
	{
		p->Enable(bEnable);
		p = p->Next();
	}		
}

//____ refreshItems() _________________________________________________________

void WgItemStack::refreshItems()
{
	Uint32	contentWidth = m_minWidth;
	Uint32	contentHeight = m_minHeight;

	WgItem * p = m_items.First();
	for(Uint32 i = 0; p && i < m_itemOffsets.entries(); i++ )
	{
		Uint32 w = p->Width() + WgAbs(m_itemOffsets[i].ofsX);
		Uint32 h = p->Height() + WgAbs(m_itemOffsets[i].ofsY);

		if( w > contentWidth)	contentWidth = w;
		if( h > contentHeight)	contentHeight = h;

		p = p->Next();
	}

	Sint32	diffX = contentWidth - m_width;
	Sint32	diffY = contentHeight - m_height;

	m_width = contentWidth;
	m_height = contentHeight;

	Modified( diffX, diffY );
	
}

//____ ItemModified() _________________________________________________________

void WgItemStack::ItemModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff )
{
	//TODO: More specific and optimized handling.
	
	refreshItems();
}

//____ GetMarkedItem() _________________________________________________________

WgItem* WgItemStack::GetMarkedItem( Uint32 x, Uint32 y )
{
	return this;
}


