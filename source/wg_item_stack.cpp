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
#include <wg_gfx.h>

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

	m_tintColor = WgColor(255,255,255);
	m_blendMode = WG_BLENDMODE_BLEND;
	m_tintMode	= TINTMODE_OPAQUE;
}

WgItemStack::WgItemStack( Sint64 id ) : WgItem( id )
{
	m_minWidth = 0;
	m_minHeight = 0;

	m_tintColor = WgColor(255,255,255);
	m_blendMode = WG_BLENDMODE_BLEND;
	m_tintMode	= TINTMODE_OPAQUE;
}

WgItemStack::WgItemStack( Sint64 id, int minW, int minH ) : WgItem( id )
{
	m_minWidth = minW;
	m_minHeight = minH;

	m_tintColor = WgColor(255,255,255);
	m_blendMode = WG_BLENDMODE_BLEND;
	m_tintMode	= TINTMODE_OPAQUE;
}

//____ ~WgItemStack() ___________________________________________________________

WgItemStack::~WgItemStack()
{
	m_itemOffsets.clear();
}

//____ SetColor() _____________________________________________________________

void WgItemStack::SetColor( const WgColor& col )
{
	if( m_tintColor != col )
	{
		m_tintColor = col;
		Modified(0,0);
	}
}

//____ SetBlendMode() _________________________________________________________

void WgItemStack::SetBlendMode( WgBlendMode mode )
{
	if( m_blendMode != mode )
	{
		m_blendMode = mode;
		Modified(0,0);
	}
}

//____ SetTintMode() __________________________________________________________

void WgItemStack::SetTintMode( WgTintMode mode )
{
	if( m_tintMode != mode )
	{
		m_tintMode = mode;
		Modified(0,0);
	}
}



//____ AddItem() ______________________________________________________________

Uint32 WgItemStack::AddItem( WgItem * pItem, WgOrigo origo, int ofsX, int ofsY )
{
	Wg_Interface_ItemHolder::AddItem(pItem);
	Uint32 pos = pItem->Index();
	m_itemOffsets.insert(pos, ItemOffset(origo, ofsX, ofsY));
	return pos;
}

//____ InsertItem() ___________________________________________________________

Uint32 WgItemStack::InsertItem( WgItem * pItem, Uint32 pos, WgOrigo origo, int ofsX, int ofsY )
{
	Wg_Interface_ItemHolder::InsertItem(pItem, pos);
	Uint32 insertedPos = pItem->Index();
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

	// Set our tint color and blend mode.

	WgBlendMode		oldBM = WgGfx::blendMode();
	WgColor			oldTC = WgGfx::tintColor();
	
	WgGfx::setBlendMode(m_blendMode);

	if( m_tintMode == TINTMODE_OPAQUE )
		WgGfx::setTintColor(m_tintColor);
	else	// MULTIPLY
		WgGfx::setTintColor(m_tintColor*oldTC);


	// Render children

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


	// Reset old blend mode and tint color

	WgGfx::setBlendMode(oldBM);
	WgGfx::setTintColor(oldTC);
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

//____ ItemSizeModified() _________________________________________________________

void WgItemStack::ItemSizeModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff )
{
	//TODO: More specific and optimized handling.
	
	refreshItems();
}

//____ ItemVisibilityModified() _______________________________________________

void WgItemStack::ItemVisibilityModified( WgItem * pItem, bool bVisible )
{
	//TODO: More specific and optimized handling.
	
	refreshItems();
}

//____ GetMarkedItem() _________________________________________________________

WgItem* WgItemStack::GetMarkedItem( Uint32 x, Uint32 y )
{
	return this;
}



//____ RequestItemGeo() _______________________________________________________

WgRect WgItemStack::RequestItemGeo( WgItem * pItem )
{
	WgRect window = m_pMyHolder->RequestItemGeo(this);

	WgItem * p = m_items.First();


	for(Uint32 i = 0; p && i < m_itemOffsets.entries(); i++)
	{
		if( p == pItem )
		{
			WgRect r;
			r.w = p->Width();
			r.h = p->Height();
			r.x = window.x + m_itemOffsets[i].origo.calcOfsX(window.w, r.w) + m_itemOffsets[i].ofsX;
			r.y = window.y + m_itemOffsets[i].origo.calcOfsY(window.h, r.h) + m_itemOffsets[i].ofsY;
			return r;
		}

		p = p->Next();
	}

	return WgRect();
}




