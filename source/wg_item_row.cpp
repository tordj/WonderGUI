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
	m_pMarkedItem = 0;
	m_heightModify = 0;
	m_bMarkChildren = true;
	m_scaleOp = SCALE_NONE;
	m_bUseAllHeight = false;
	m_minWidthFraction = 0.f;
	m_minWidthFractionLimit = INT_MAX;
}

WgItemRow::WgItemRow( Sint64 id ) : WgItem( id )
{
	m_pMarkedItem = 0;
	m_heightModify = 0;
	m_bMarkChildren = true;
	m_scaleOp = SCALE_NONE;
	m_bUseAllHeight = false;
	m_minWidthFraction = 0.f;
	m_minWidthFractionLimit = INT_MAX;
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

//____ SetScaleOp() ___________________________________________________

void WgItemRow::SetScaleOp( ScaleOp scaleOperation )
{
	m_scaleOp = scaleOperation;
}

//____ SetUseAllHeight() ___________________________________________________

void WgItemRow::SetUseAllHeight( bool bUseAllHeight )
{
	m_bUseAllHeight = bUseAllHeight;
}


//____ SetMinWidthFraction() __________________________________________________

void WgItemRow::SetMinWidthFraction( float fraction, int limit )
{
	LIMIT(fraction,0.f,1.f);
	m_minWidthFraction = fraction;
	m_minWidthFractionLimit = limit;
}


//____ SetHeightModify() ______________________________________________________

void WgItemRow::SetHeightModify( int pixels )
{
	m_heightModify = pixels;
	refreshItems();
}

//____ GetTooltipString() _____________________________________________________

WgString WgItemRow::GetTooltipString() const
{
	if( m_pMarkedItem )
		return m_pMarkedItem->GetTooltipString();
	else
		return WgString();
}


//____ ItemWidth() ___________________________________________________________

int WgItemRow::ItemWidth( WgItem * pItem, int screen_width )
{
	int width;
	switch( m_scaleOp )
	{
		case SCALE_NONE:
			width = pItem->Width();
			break;
		case SCALE_FIRST:
			if( pItem->Prev() == 0 )
				width = pItem->Width() + screen_width - m_width;
			else
				width = pItem->Width();
			break;
		case SCALE_LAST:
			if( pItem->Next() == 0 )
				width = pItem->Width() + screen_width - m_width;
			else
				width = pItem->Width();
			break;

	}




	int minWidth = (int) (m_minWidthFraction*screen_width);
	if( minWidth > m_minWidthFractionLimit )
		minWidth = m_minWidthFractionLimit;

	if( width < minWidth )
		width = minWidth;
	
	
	
	/*
	// Very naive implementation of maxWidth, doesn't work under all circumstances.

	int maxWidth = screen_width - minWidth*m_nItems;
	if( width > maxWidth )
		width = maxWidth;
*/
	return width;
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
	WgItem * p = m_items.First();
	int totalWidth = 0;

	while( p )
	{
		int width =  ItemWidth(p, displayed_width);					// Get items wanted width.

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
	if( _action == WgInput::POINTER_EXIT && m_pMarkedItem != 0 )
	{
		m_pMarkedItem->ActionRespond( this, _action, _button_key, _info, _inputObj );
		m_pMarkedItem = 0;
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



	WgCoord mousePos = WgCoord(_info.x, _info.y) - m_pMyHolder->RequestItemGeo(this).Pos();
//	int x = _info.x;
//	int y = _info.y;
	
	
	//	Abs2local( &x, &y );

	if( !HasItem( m_pMarkedItem ) )
		m_pMarkedItem = 0;

	WgItem * pItem = GetMarkedItem( mousePos.x, mousePos.y );
	if( pItem != this )
	{
		if( pItem != m_pMarkedItem && m_pMarkedItem != 0 )
			m_pMarkedItem->ActionRespond( pEmitter, WgInput::POINTER_EXIT, _button_key, _info, _inputObj );

		if( pItem && m_pMarkedItem != pItem )
			pItem->ActionRespond( pEmitter, WgInput::POINTER_ENTER, _button_key, _info, _inputObj );

		if( pItem && _action != WgInput::POINTER_ENTER )							// Pointer enter already handled above.
			pItem->ActionRespond( pEmitter, _action, _button_key, _info, _inputObj );

		m_pMarkedItem = pItem;
	}
	else
		m_pMarkedItem = 0;
/*
	WgItem * pItem = GetMarkedItem( (Uint32) x, (Uint32) y );

	if( pItem != m_pMarkedItem && m_pMarkedItem != 0 )
	{
		m_pMarkedItem->ActionRespond( this, WgInput::POINTER_EXIT, _button_key, _info, _inputObj );
		m_pMarkedItem = 0;
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
			m_pMarkedItem = pItem;
		else
			m_pMarkedItem = 0;
	}
	else
	{
		m_pMarkedItem = 0;
	}

*/




	WgItem::ActionRespond( pEmitter, _action, _button_key, _info, _inputObj );
}


//____ Render() _______________________________________________________________

void WgItemRow::Render( const WgRect& _window, const WgRect& _clip )
{
	WgRect r;

	r = _window;

	WgItem * p = m_items.First();

	while( p )
	{
		if( !m_bUseAllHeight )
			r.h = p->Height();
		
		r.w = ItemWidth(p, _window.w);

		p->Render( r, _clip );
		r.x += r.w + m_itemSpacing;
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
	if( m_bMarkChildren )
	{
		int screen_width = GetHolder()->RequestItemGeo(this).w;

		Uint32 itemX1 = 0;
		WgItem * p = m_items.First();
		while( p )
		{
			Uint32 itemX2 = itemX1 + ItemWidth(p, screen_width);
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

