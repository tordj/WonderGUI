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
#include	<wg_pen.h>
#include	<wg_base.h>

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

static const char	c_gizmoType[] = {"MenuBar"};


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
	return c_gizmoType;
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

	WgTextAttr	attr;
	WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextProp(), WG_MODE_NORMAL);
	WgTextTool::AddPropAttributes( attr, m_pTextProp, WG_MODE_NORMAL);
	Uint32 lineWidthNormal = WgTextTool::lineWidth( 0, attr, WG_MODE_NORMAL, pItem->m_pText );

	attr.Clear();
	WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextProp(), WG_MODE_MARKED);
	WgTextTool::AddPropAttributes( attr, m_pTextProp, WG_MODE_MARKED);
	Uint32 lineWidthMarked = WgTextTool::lineWidth( 0, attr, WG_MODE_NORMAL, pItem->m_pText );

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

	m_items.PushBack(pItem);
	RequestRender();
	return true;
}

//____ RemoveMenu() ________________________________________________________
bool WgGizmoMenubar::RemoveMenu( Wdg_Menu * pMenu )
{
	for( WgMenuBarItem * pI = m_items.First(); pI; pI = pI->Next() )
	{
		if(pI->m_pMenu == pMenu)
		{
			pI->m_pMenu = 0;
			delete pI;
			RequestRender();
			return true;
		}
	}
	return false;
}

//____ GetMenuTitle() ________________________________________________________
WgChar *WgGizmoMenubar::GetMenuTitle(Wdg_Menu * pMenu) const
{
	for( WgMenuBarItem * pI = m_items.First(); pI; pI = pI->Next() )
	{
		if(pI->m_pMenu == pMenu)
			return pI->m_pText;
	}
	return 0;
}

//____ ShowMenu() ________________________________________________________
bool WgGizmoMenubar::ShowMenu(Wdg_Menu * pMenu)
{
	for( WgMenuBarItem * pI = m_items.First(); pI; pI = pI->Next() )
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
	for( WgMenuBarItem * pI = m_items.First(); pI; pI = pI->Next() )
	{
		if(pI->m_pMenu == pMenu)
		{
			pI->Hide();
			return true;
		}
	}
	return false;
}

//____ _onRender() ________________________________________________________

void WgGizmoMenubar::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Render background

	if( m_pBgGfx )
	{
		WgBlock block;

		if( m_bEnabled )
			block = m_pBgGfx->GetBlock(WG_MODE_NORMAL);
		else
			block = m_pBgGfx->GetBlock(WG_MODE_DISABLED);

		pDevice->ClipBlitBlock( _clip, block, _canvas );
	}

	// Take backgrounds content borders into account

	WgRect	window	= _canvas;
	WgRect	clip;

	if( m_pBgGfx )
	{
		window.shrink( m_pBgGfx->GetContentBorders() );
		clip.intersection( window, _clip );
	}
	else
		clip = _clip;

	// Go throught the MenuBarItems and print their text and render their rectangles.

	WgMenuBarItem * pI = m_items.First();
	Uint32 posX = window.x;

	WgPen pen;


	WgTextAttr	attr;
	WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextProp(), WG_MODE_NORMAL);
	WgTextTool::AddPropAttributes( attr, m_pTextProp, WG_MODE_NORMAL);

	pen.SetAttributes( attr );
	pen.SetClipRect( clip );
	pen.SetDevice( pDevice );

	Uint32 printPosY = window.y + ( window.h - pen.GetLineHeight() )/2 + pen.GetBaseline();

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
				WgRect	dest( posX, window.y, pI->m_width + b.width(), window.h );
				pDevice->ClipBlitBlock( clip, m_pEntryGfx->GetBlock(mode), dest );
			}

			pen.SetPos( WgCord(posX + b.left, printPosY) );

			WgTextAttr	attr;
			WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextProp(), mode );
			WgTextTool::AddPropAttributes( attr, m_pTextProp, mode );
			pen.SetAttributes( attr );

			pDevice->PrintLine( pen, attr, pI->m_pText );

			posX += pI->m_width + b.width();
		}
		itemNb++;
		pI = pI->Next();
	}
}


//____ _onAction() _____________________________________________________________

void WgGizmoMenubar::_onAction( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
{
	switch( _action )
	{
		case WgInput::POINTER_OVER:
		case WgInput::BUTTON_PRESS:
		{
			Uint32 item = GetItemAtAbsPos( _info.x, _info.y );

			if( item && !m_items.Get(item-1)->m_bEnabled )
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

//____ _onRefresh() _______________________________________________________

void WgGizmoMenubar::_onRefresh( void )
{
	RequestRender();
}

//____ _onAlphaTest() ___________________________________________________

bool WgGizmoMenubar::_onAlphaTest( const WgCord& ofs )
{
	return true;
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoMenubar::_onCloneContent( const WgGizmo * _pOrg )
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
	WgMenuBarItem * pItem = m_items.Get(nb-1);
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

	int bordersWidth = GetEntryBorders().width();

	WgMenuBarItem * pI = m_items.First();
	while( pI != pItem )
	{
		if( pI->IsVisible() )
			x += pI->m_width + bordersWidth;

		pI = pI->Next();
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

		pItem->m_pMenu->Open((Wdg_Root*)m_pHook->GetRoot(), x, yPos, 0, 0, &r );
	}
	return true;
}

//____ CloseMenu() ____________________________________________________________

bool WgGizmoMenubar::CloseMenu( Uint32 nb )
{
	if( nb == 0 )
		return false;

	WgMenuBarItem * pItem = m_items.Get(m_selectedItem-1);
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
		int bordersWidth = GetEntryBorders().width();

		WgMenuBarItem * pItem = m_items.First();
		int		item = 1;
		while( pItem )
		{
			if(pItem->IsVisible())
			{
				x -= pItem->m_width+bordersWidth;

				if( x < 0 )
					return item;
			}

			pItem = pItem->Next();
			item++;
		}
	}

	return 0;
}

//____ MoveOutsideModal() _____________________________________________________

void WgGizmoMenubar::MoveOutsideModal( int x, int y )
{
	Uint32 item = GetItemAtAbsPos( x, y );

	if( item && item != m_selectedItem && m_items.Get(item-1)->m_bEnabled )
	{
		CloseMenu( m_selectedItem );
		OpenMenu(item);
	}

}

//____ MenuOpened() ___________________________________________________________

void WgGizmoMenubar::MenuOpened( WgMenuBarItem * pItem )
{
	Uint32 item = pItem->Index()+1;
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
	delete m_pMenu;

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
