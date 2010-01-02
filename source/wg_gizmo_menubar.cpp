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



#include	<wg_gizmo_menubar.h>
#include	<wdg_menu.h>

#include	<wg_types.h>
#include	<wg_char.h>

#ifndef WG_SURFACE_DOT_H
#	include	<wg_surface.h>
#endif

#ifndef WG_GFXDEVICE_DOT_H
#	include	<wg_gfxdevice.h>
#endif

#ifndef WG_CHAR_DOT_H
//#	include	<wg_char.h>
#endif

#include <wg_texttool.h>

static const char	s_type[] = {"TordJ/MenuBar"};


//____ WgGizmoMenubar() _________________________________________________________________

WgGizmoMenubar::WgGizmoMenubar( void )
{
	m_selectedItem	= 0;		// 0 = no item is selected.
	m_markedItem	= 0;		// 0 = no item is marked:

	m_pBgGfx		= 0;
	m_pEntryGfx		= 0;
}

//____ ~WgGizmoMenubar() __________________________________________________________

WgGizmoMenubar::~WgGizmoMenubar( void )
{
}


//____ Type() __________________________________________________________________

const char * WgGizmoMenubar::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoMenubar::GetMyType( void )
{
	return s_type;
}

//____ SetBgSource() __________________________________________________________

bool WgGizmoMenubar::SetBgSource( const WgBlockSetPtr& pBlocks )
{
	m_pBgGfx	= pBlocks;

	RequestRender();
	return true;
}

//____ SetEntrySource() _______________________________________________________

bool WgGizmoMenubar::SetEntrySource( const WgBlockSetPtr& pBlocks, const WgTextPropPtr& pTextProperties )
{
	m_pEntryGfx			= pBlocks;
	m_pTextProp			= pTextProperties;

	RequestRender();
	return true;
}

//____ AddMenu() ______________________________________________________________

bool WgGizmoMenubar::AddMenu( const char * pTitle, Wdg_Menu * pMenu, Uint16 navKey )
{
	// Sanity check

	if( !pTitle || !pMenu )
		return false;

	// Convert/copy title

	WgMenuBarItem * pItem = new WgMenuBarItem();

	Uint32 strlen = WgTextTool::countChars( pTitle );
	pItem->m_pText = new WgChar[strlen+1];

	WgTextTool::readString(pTitle, pItem->m_pText, strlen+1);

	// Calculate linewidth

	Uint32 lineWidthNormal = WgTextTool::lineWidth( m_pTextProp, WG_MODE_NORMAL, pItem->m_pText );
	Uint32 lineWidthMarked = WgTextTool::lineWidth( m_pTextProp, WG_MODE_MARKED, pItem->m_pText );

	if( lineWidthNormal > lineWidthMarked )
		pItem->m_width = lineWidthNormal;
	else
		pItem->m_width = lineWidthMarked;

	// Set simple members

	pItem->m_navKey		= navKey;
	pItem->m_pMenu		= pMenu;
	pItem->m_pMenuBar	= this;

	// Register callbacks

	pItem->m_pMenu->AddCallback( WgSignal::PointerOutsideModalPos(), cbMoveOutsideModal, this );
	pItem->m_pMenu->AddCallback( WgSignal::MenuClosed(), cbMenuClosed, pItem );
	pItem->m_pMenu->AddCallback( WgSignal::MenuOpened(), cbMenuOpened, pItem );

	// Finish up

	m_items.push_back(pItem);
	RequestRender();
	return true;
}

//____ GetMenuTitle() ________________________________________________________
WgChar *WgGizmoMenubar::GetMenuTitle(Wdg_Menu * pMenu) const
{
	for( WgMenuBarItem * pI = m_items.getFirst(); pI; pI = pI->getNext() )
	{
		if(pI->m_pMenu == pMenu)
			return pI->m_pText;
	}
	return 0;
}

//____ ShowMenu() ________________________________________________________
bool WgGizmoMenubar::ShowMenu(Wdg_Menu * pMenu)
{
	for( WgMenuBarItem * pI = m_items.getFirst(); pI; pI = pI->getNext() )
	{
		if(pI->m_pMenu == pMenu)
		{
			pI->Show();
			return true;
		}
	}
	return false;
}

//____ HideMenu() ________________________________________________________
bool WgGizmoMenubar::HideMenu(Wdg_Menu * pMenu)
{
	for( WgMenuBarItem * pI = m_items.getFirst(); pI; pI = pI->getNext() )
	{
		if(pI->m_pMenu == pMenu)
		{
			pI->Hide();
			return true;
		}
	}
	return false;
}

//____ OnRender() ________________________________________________________

void WgGizmoMenubar::OnRender( WgGfxDevice * pDevice, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Render background

	if( m_pBgGfx )
	{
		WgBlock block;

		if( m_bEnabled )
			block = m_pBgGfx->GetBlock(WG_MODE_NORMAL);
		else
			block = m_pBgGfx->GetBlock(WG_MODE_DISABLED);

		pDevice->ClipBlitBlock( _clip, block, _window );
	}


	// Go throught the MenuBarItems and print their text and render their rectangles.

	WgMenuBarItem * pI = m_items.getFirst();
	Uint32 posX = _window.x;
	Uint32 posY = _window.y;

	if( m_pBgGfx )										// Take backgrounds content borders into account.
	{
		posX += m_pBgGfx->GetContentBorders().left;
		posY += m_pBgGfx->GetContentBorders().top;
	}

	Uint32 itemNb = 1;
	while( pI )
	{
		if( pI->IsVisible() )
		{
			WgMode	mode = WG_MODE_DISABLED;
			if( m_bEnabled && pI->m_bEnabled )
			{
				mode = WG_MODE_NORMAL;

				if( itemNb == m_selectedItem )
					mode = WG_MODE_SELECTED;
				else if( itemNb == m_markedItem )
					mode = WG_MODE_MARKED;
			}

			WgBorders b = GetEntryBorders();

			if( m_pEntryGfx )
			{
				WgRect	dest( posX, posY, pI->m_width + b.GetWidth(), _window.h );
				pDevice->ClipBlitBlock( _clip, m_pEntryGfx->GetBlock(mode), dest );
			}


			pDevice->ClipPrintLine( _clip, m_pTextProp, mode, posX + b.left, posX + b.left, posY + b.top, pI->m_pText );

			posX += pI->m_width + b.GetWidth();
		}
		itemNb++;
		pI = pI->getNext();
	}
}


//____ OnAction() _____________________________________________________________

void WgGizmoMenubar::OnAction( WgEmitter * pEmitter, WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	switch( _action )
	{
		case WgInput::POINTER_OVER:
		case WgInput::BUTTON_PRESS:
		{
			Uint32 item = GetItemAtAbsPos( _info.x, _info.y );

			if( item && !m_items.get(item-1)->m_bEnabled )
				item = 0;								// Item is disabled and can't be marked.

			if(m_markedItem != item)
			{
				m_markedItem = item;
				RequestRender();
			}

			// If a menu entry already is selected we should
			// switch to the new marked one.

			if( m_selectedItem )
			{
				if( m_markedItem && m_selectedItem != m_markedItem )
				{
					CloseMenu( m_selectedItem );
					OpenMenu( m_markedItem );
				}
			}

			// If this was a press, we need to select the item and open the menu.

			//TODO: A click on an already open entry should close the menu.

			if( item && _action == WgInput::BUTTON_PRESS )
			{
				OpenMenu( item );
			}

		}
		break;

		case WgInput::POINTER_EXIT:
			m_markedItem = 0;
			RequestRender();
		break;

        default:
            break;
	}
}

//____ OnRefresh() _______________________________________________________

void WgGizmoMenubar::OnRefresh( void )
{
	RequestRender();
}

//____ OnMarkTest() ___________________________________________________

bool WgGizmoMenubar::OnMarkTest( const WgCord& ofs )
{
	return true;
}

//____ OnCloneContent() _______________________________________________________

void WgGizmoMenubar::OnCloneContent( const WgGizmo * _pOrg )
{
	//TODO: Implement!
}

//____ GetEntryBorders() ____________________________________________________

WgBorders WgGizmoMenubar::GetEntryBorders() const
{
	if( m_pEntryGfx )
		return m_pEntryGfx->GetContentBorders();
	else
		return WgBorders(10,10,0,0);		// 10 pixels on each side as default margin. Should do something more intelligent here, like taking fonts avgSpacing into account...
}



//____ OpenMenu() _____________________________________________________________

bool WgGizmoMenubar::OpenMenu( Uint32 nb )
{
	WgMenuBarItem * pItem = m_items.get(nb-1);
	if( pItem == 0 || pItem->m_pMenu == 0 )
		return false;

	WgCord pos = Abs2local( WgCord(0, 0) );

	Sint32	x = pos.x;
	Sint32	y = pos.y;

	if( m_pBgGfx )
	{
		pos.x += m_pBgGfx->GetContentBorders().left;
		pos.y += m_pBgGfx->GetContentBorders().top;
	}

	int bordersWidth = GetEntryBorders().GetWidth();

	WgMenuBarItem * pI = m_items.getFirst();
	while( pI != pItem )
	{
		if( pI->IsVisible() )
			x += pI->m_width + bordersWidth;

		pI = pI->getNext();
	}

	WgRect	r(x, y, pI->m_width+bordersWidth, Size().h );


	if( m_pHook )
	{
		WgWidget * pRoot = m_pHook->GetRoot();
		int yPos;

		if( pRoot->Height() < r.y + r.h + pItem->m_pMenu->Height() )
			yPos = y - pItem->m_pMenu->Height();
		else
			yPos = y + Size().h;

		pItem->m_pMenu->Open((Wdg_Root*)m_pHook->GetRoot(), x, yPos, 0, &r );
	}
	return true;
}

//____ CloseMenu() ____________________________________________________________

bool WgGizmoMenubar::CloseMenu( Uint32 nb )
{
	if( nb == 0 )
		return false;

	WgMenuBarItem * pItem = m_items.get(m_selectedItem-1);
	if( !pItem || !pItem->m_pMenu )
		return false;

	pItem->m_pMenu->Close();
	return true;
}

//____ GetItemAtPos() _________________________________________________________

Uint32 WgGizmoMenubar::GetItemAtAbsPos( int x, int y )
{
	WgCord pos = Abs2local( WgCord(x, y) );
	x = pos.x;
	y = pos.y;

	if( m_pBgGfx )
	{
		pos.x -= m_pBgGfx->GetContentBorders().left;
		pos.y -= m_pBgGfx->GetContentBorders().top;
	}

	if( y > 0 && x > 0 && y < (int) Size().h )
	{
		int bordersWidth = GetEntryBorders().GetWidth();	

		WgMenuBarItem * pItem = m_items.getFirst();
		int		item = 1;
		while( pItem )
		{
			if(pItem->IsVisible())
			{
				x -= pItem->m_width+bordersWidth;

				if( x < 0 )
					return item;
			}

			pItem = pItem->getNext();
			item++;
		}
	}

	return 0;
}

//____ MoveOutsideModal() _____________________________________________________

void WgGizmoMenubar::MoveOutsideModal( int x, int y )
{
	Uint32 item = GetItemAtAbsPos( x, y );

	if( item && item != m_selectedItem && m_items.get(item-1)->m_bEnabled )
	{
		CloseMenu( m_selectedItem );
		OpenMenu(item);
	}

}

//____ MenuOpened() ___________________________________________________________

void WgGizmoMenubar::MenuOpened( WgMenuBarItem * pItem )
{
	Uint32 item = pItem->getIndex()+1;
	m_selectedItem = item;
	RequestRender();
}

//____ MenuClosed() ___________________________________________________________

void WgGizmoMenubar::MenuClosed( WgMenuBarItem * pItem )
{
	m_selectedItem = 0;
	RequestRender();
}


//____ WgMenuBarItem::WgMenuBarItem() _________________________________________

WgMenuBarItem::WgMenuBarItem()
{
	m_pText		= 0;
	m_navKey	= 0;
	m_pMenu		= 0;
	m_width		= 0;
	m_bEnabled	= true;
	m_bVisible	= true;
}

//____ WgMenuBarItem::~WgMenuBarItem() ________________________________________

WgMenuBarItem::~WgMenuBarItem()
{
	if(m_pText != 0)
		delete [] m_pText;

}

//____ WgMenuBarItem::Enable() ________________________________________________

void WgMenuBarItem::Enable()
{
	if( !m_bEnabled )
	{
		m_bEnabled = true;
		m_pMenuBar->RequestRender();		//TODO: Only render what is needed...
	}
}

//____ WgMenuBarItem::Disable() _______________________________________________

void WgMenuBarItem::Disable()
{
	if( m_bEnabled )
	{
		m_bEnabled = false;

		//TODO: Force unmark/unselect

		m_pMenuBar->RequestRender();		//TODO: Only render what is needed...
	}
}

//____ WgMenuBarItem::Show() ________________________________________________

void WgMenuBarItem::Show()
{
	if( !m_bVisible )
	{
		m_bVisible = true;
		m_pMenuBar->RequestRender();		//TODO: Only render what is needed...
	}
}

//____ WgMenuBarItem::Disable() _______________________________________________

void WgMenuBarItem::Hide()
{
	if( m_bVisible )
	{
		m_bVisible = false;

		//TODO: Force unmark/unselect

		m_pMenuBar->RequestRender();		//TODO: Only render what is needed...
	}
}
