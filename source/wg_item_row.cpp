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


#include <wg_item_row.h>


static const char	Wdg_Type[] = {"TordJ/WgItemRow"};

const char * WgItemRow::Type( void ) const
{
	return GetMyType();
}

const char * WgItemRow::GetMyType()
{
	return Wdg_Type;
}
//____ WgItemRow() ____________________________________________________________

WgItemRow::WgItemRow()
{
	m_heightModify = 0;
	m_bMarkChildren = false;
	m_bStretchLastItem = false;
	m_bUseAllHeight = false;
	m_widthExpandUsage = 0.f;
}

WgItemRow::WgItemRow( Sint64 id ) : WgItem( id )
{
	m_heightModify = 0;
	m_bMarkChildren = false;
	m_bStretchLastItem = false;
	m_bUseAllHeight = false;
	m_widthExpandUsage = 0.f;
}


//____ ~WgItemRow() ___________________________________________________________

WgItemRow::~WgItemRow()
{
}

//____ SetMarkChildren() ______________________________________________________

void WgItemRow::SetMarkChildren( bool bMarkChildren )
{
	m_bMarkChildren = bMarkChildren;
}

//____ SetStretchLastItem() ___________________________________________________

void WgItemRow::SetStretchLastItem( bool bStretch )
{
	m_bStretchLastItem = bStretch;
}

//____ SetUseAllHeight() ___________________________________________________

void WgItemRow::SetUseAllHeight( bool bUseAllHeight )
{
	m_bUseAllHeight = bUseAllHeight;
}

//____ SetWidthExpandUsage() __________________________________________________

void WgItemRow::SetWidthExpandUsage( float usage )
{
	LIMIT(usage,0.f,1.f);
	m_widthExpandUsage = usage;
}


//____ SetHeightModify() ______________________________________________________

void WgItemRow::SetHeightModify( int pixels )
{
	m_heightModify = pixels;
	refreshItems();
}

//____ WidthExpandFactor() ________________________________________________

float WgItemRow::WidthExpandFactor( int screen_width )
{

	//TODO: Take item_spacing into account.

	int totalWidth = 0;
	WgItem * p = m_items.First();

	while( p )
	{
		totalWidth += p->Width();
		p = p->Next();
	}			

	if( totalWidth == 0 || totalWidth >= screen_width )
		return 1.f;

	float	expand_factor = (screen_width-totalWidth)/totalWidth*m_widthExpandUsage + 1.f;

	return expand_factor;
}

//____ AdaptToHeight() ________________________________________________________

void WgItemRow::AdaptToHeight( Uint32 displayed_height )
{
	WgItem * p = m_items.First();

	while( p )
	{
		p->AdaptToHeight(displayed_height);
		p = p->Next();
	}			
	
	refreshItems();
}

//____ AdaptToWidth() ________________________________________________________

void WgItemRow::AdaptToWidth( Uint32 displayed_width )
{
	float multiplier = WidthExpandFactor(displayed_width);

	WgItem * p = m_items.First();
	int totalWidth = 0;

	while( p )
	{
		int width = p->Width();

		if( m_bStretchLastItem && p->Next() == 0 )
			width = displayed_width - totalWidth;
		else
			width = (int) (width*multiplier);					// Get items wanted width.

		totalWidth += width;
		p->AdaptToWidth(width);
		p = p->Next();
	}			
	
	refreshItems();
}


//____ ActionRespond() ________________________________________________________

void WgItemRow::ActionRespond( WgEmitter * pEmitter, WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{

/*
	if( _action == WgInput::POINTER_EXIT && m_pLastMarkedItem != 0 )
	{
		m_pLastMarkedItem->ActionRespond( this, _action, _button_key, _info, _inputObj );
		m_pLastMarkedItem = 0;
		return;
	}
*/

	switch( _action )
	{
		case WgInput::KEY_DOWN:
		case WgInput::KEY_PRESS:
		case WgInput::KEY_RELEASE:
		case WgInput::KEY_REPEAT:
		case WgInput::CHARACTER:
		{
			if( m_pFocusedItem )
				m_pFocusedItem->ActionRespond( pEmitter, _action, _button_key, _info, _inputObj );
		}
		break;


		default:
			break;
	}



/*
	WgItem * pItem = GetMarkedItem( (Uint32) x, (Uint32) y );

	if( pItem != m_pLastMarkedItem && m_pLastMarkedItem != 0 )
	{
		m_pLastMarkedItem->ActionRespond( this, WgInput::POINTER_EXIT, _button_key, _info, _inputObj );
		m_pLastMarkedItem = 0;
		return;	// hmmm... should this return really be here?
	}

	if( pItem )
	{
		// HACK. Remove when message loop is implemented
		// pItem can be deleted in the ActionResponse callback. Make sure it still exist // Martin
		pItem->ActionRespond( this, _action, _button_key, _info, _inputObj );
		WgItem* p = m_items.First();
		while(p && p != pItem) p = p->GetNext();
		if(p == pItem)
			m_pLastMarkedItem = pItem;
		else
			m_pLastMarkedItem = 0;
	}
	else
	{
		m_pLastMarkedItem = 0;
	}

*/




	WgItem::ActionRespond( pEmitter, _action, _button_key, _info, _inputObj );
}


//____ Render() _______________________________________________________________

void WgItemRow::Render( const WgRect& _window, const WgRect& _clip )
{
	float multiplier = WidthExpandFactor(_window.w);

	WgRect r;

	r = _window;

	WgItem * p = m_items.First();

	while( p )
	{
		if( !m_bUseAllHeight )
			r.h = p->Height();
		
		if( m_bStretchLastItem && p->Next() == 0 )
			r.w = _window.x + _window.w - r.x;
		else
			r.w = (int) (p->Width()*multiplier);

		p->Render( r, _clip );
		r.x += p->Width() + m_itemSpacing;
		p = p->Next();
	}
}

//____ Clone() ________________________________________________________________

WgItem* WgItemRow::Clone( WgItem * _pClone )
{
	WgItemRow * pClone = (WgItemRow*) _pClone;

	if( !pClone )
		pClone = new WgItemRow();

	//TODO: Implement cloning of content.


	return WgItem::Clone(pClone);
}

//____ Enable() _______________________________________________________________

void WgItemRow::Enable( bool bEnable )
{
	WgItem * p = m_items.First();

	while( p )
	{
		p->Enable(bEnable);
		p = p->Next();
	}		
}

//____ refreshItems() _________________________________________________________

void WgItemRow::refreshItems()
{
	Uint32	contentWidth = 0;
	Uint32	contentHeight = 0;

	WgItem * p = m_items.First();

	if( p )
		contentWidth -= m_itemSpacing;		// We will get one spacing too much...

	while( p )
	{
		if( p->Height() > contentHeight )
			contentHeight = p->Height();
		
		contentWidth += p->Width() + m_itemSpacing;
		p = p->Next();
	}

	contentHeight += m_heightModify;

	Sint32	diffX = contentWidth - m_width;
	Sint32	diffY = contentHeight - m_height;

	m_width = contentWidth;
	m_height = contentHeight;

	Modified( diffX, diffY );
	
}

//____ ItemSizeModified() _____________________________________________________

void WgItemRow::ItemSizeModified( WgItem * pItem, Sint32 widthDiff , Sint32 heightDiff )
{
	//TODO: More specific and optimized handling.
	
	refreshItems();
}

//____ ItemVisibilityModified() _______________________________________________

void WgItemRow::ItemVisibilityModified( WgItem * pItem, bool bVisible )
{
	//TODO: More specific and optimized handling.
	
	refreshItems();
}


//____ GetMarkedItem() _________________________________________________________

WgItem* WgItemRow::GetMarkedItem( Uint32 x, Uint32 y )
{
	//TODO: Does not take WidthExpandFactor into account.

	if( m_bMarkChildren )
	{
		Uint32 itemX1 = 0;
		WgItem * p = m_items.First();
		while( p )
		{
			Uint32 itemX2 = itemX1 + p->Width();
			if( x >= itemX1 && x < itemX2 )
				return p->GetMarkedItem( x, y );

			itemX1 = itemX2 + m_itemSpacing;

			p = p->GetNext();
		}
	}

	return this;
}

//____ RequestItemGeo() _______________________________________________________

WgRect WgItemRow::RequestItemGeo( WgItem * pItem )
{
	WgRect r = m_pMyHolder->RequestItemGeo(this);

	WgItem * p = m_items.First();

	while( p )
	{
		r.h = p->Height();
		r.w = p->Width();

		if( p == pItem )
			return r;

		r.x += r.w + m_itemSpacing;
		p = p->Next();
	}

	return WgRect();
}

