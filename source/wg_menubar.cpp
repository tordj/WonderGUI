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



#include	<wg_menubar.h>
#include	<wg_menu.h>

#include	<wg_types.h>
#include	<wg_char.h>
#include	<wg_pen.h>
#include	<wg_base.h>
#include	<wg_event.h>
#include	<wg_eventhandler.h>
#include	<wg_surface.h>
#include	<wg_gfxdevice.h>
#include	<wg_texttool.h>
#include	<wg_menulayer.h>

const char WgMenubar::CLASSNAME[] = {"Menubar"};


//____ WgMenubar() _________________________________________________________________

WgMenubar::WgMenubar( void )
{
	m_selectedItem	= 0;		// 0 = no item is selected.
	m_markedItem	= 0;		// 0 = no item is marked:

	m_pSkin			= 0;
	m_pEntrySkin	= 0;
}

//____ ~WgMenubar() __________________________________________________________

WgMenubar::~WgMenubar( void )
{
}


//____ IsInstanceOf() _________________________________________________________

bool WgMenubar::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgMenubar::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgMenubarPtr WgMenubar::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMenubarPtr( static_cast<WgMenubar*>(pObject.GetRealPtr()) );

	return 0;
}


//____ SetEntrySkin() _______________________________________________________

bool WgMenubar::SetEntrySkin( const WgSkinPtr& pSkin, const WgTextpropPtr& pTextProperties )
{
	//TODO: Possibly resize if needed.

	m_pEntrySkin			= pSkin;
	m_pTextProp			= pTextProperties;

	_requestRender();
	return true;
}

//____ AddMenu() ______________________________________________________________

bool WgMenubar::AddMenu( const char * pTitle, const WgMenuPtr& pMenu, Uint16 navKey )
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
	WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextprop(), WG_STATE_NORMAL);
	WgTextTool::AddPropAttributes( attr, m_pTextProp, WG_STATE_NORMAL);
	Uint32 lineWidthNormal = WgTextTool::lineWidth( 0, attr, WG_STATE_NORMAL, pItem->m_pText );

	attr.Clear();
	WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextprop(), WG_STATE_HOVERED);
	WgTextTool::AddPropAttributes( attr, m_pTextProp, WG_STATE_HOVERED);
	Uint32 lineWidthMarked = WgTextTool::lineWidth( 0, attr, WG_STATE_HOVERED, pItem->m_pText );

	if( lineWidthNormal > lineWidthMarked )
		pItem->m_width = lineWidthNormal;
	else
		pItem->m_width = lineWidthMarked;

	// Set simple members

	pItem->m_navKey		= navKey;
	pItem->m_pMenu		= pMenu;
	pItem->m_pMenuBar	= this;

	// Register callbacks
/*
	pItem->m_pMenu->AddCallback( WgSignal::PointerOutsideModalPos(), cbMoveOutsideModal, this );
	pItem->m_pMenu->AddCallback( WgSignal::MenuClosed(), cbMenuClosed, pItem );
	pItem->m_pMenu->AddCallback( WgSignal::MenuOpened(), cbMenuOpened, pItem );
*/
	// Finish up

	m_items.PushBack(pItem);
	_requestRender();
	return true;
}

//____ RemoveMenu() ________________________________________________________
bool WgMenubar::RemoveMenu( const WgMenuPtr& pMenu )
{
	for( WgMenuBarItem * pI = m_items.First(); pI; pI = pI->Next() )
	{
		if(pI->m_pMenu == pMenu)
		{
			pI->m_pMenu = 0;
			delete pI;
			_requestRender();
			return true;
		}
	}
	return false;
}

//____ MenuTitle() ________________________________________________________
WgChar *WgMenubar::MenuTitle( const WgMenuPtr& pMenu) const
{
	for( WgMenuBarItem * pI = m_items.First(); pI; pI = pI->Next() )
	{
		if(pI->m_pMenu == pMenu)
			return pI->m_pText;
	}
	return 0;
}

//____ ShowMenu() ________________________________________________________
bool WgMenubar::ShowMenu( const WgMenuPtr& pMenu)
{
	for( WgMenuBarItem * pI = m_items.First(); pI; pI = pI->Next() )
	{
		if(pI->m_pMenu == pMenu)
		{
			pI->SetVisible(true);
			return true;
		}
	}
	return false;
}

//____ HideMenu() ________________________________________________________
bool WgMenubar::HideMenu(const WgMenuPtr& pMenu)
{
	for( WgMenuBarItem * pI = m_items.First(); pI; pI = pI->Next() )
	{
		if(pI->m_pMenu == pMenu)
		{
			pI->SetVisible(false);
			return true;
		}
	}
	return false;
}

//____ PreferredSize() __________________________________________________________

WgSize WgMenubar::PreferredSize() const
{
	//TODO: Implement!

	return WgSize(200,20);
}


//____ _onRender() ________________________________________________________

void WgMenubar::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	// Take backgrounds content borders into account

	WgRect	window;
	WgRect	clip;

	if( m_pSkin )
	{
		window = m_pSkin->ContentRect( _canvas, m_state );
		clip.Intersection( window, _clip );
	}
	else
	{
		window = _canvas;
		clip = _clip;
	}

	// Go throught the MenuBarItems and print their text and render their rectangles.

	WgMenuBarItem * pI = m_items.First();
	Uint32 posX = window.x;

	WgPen pen;


	WgTextAttr	attr;
	WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextprop(), WG_STATE_NORMAL);
	WgTextTool::AddPropAttributes( attr, m_pTextProp, WG_STATE_NORMAL);

	pen.SetAttributes( attr );
	pen.SetClipRect( clip );
	pen.SetDevice( pDevice );

	Uint32 printPosY = window.y + ( window.h - pen.GetLineHeight() )/2 + pen.GetBaseline();

	Uint32 itemNb = 1;
	while( pI )
	{
		if( pI->IsVisible() )
		{
			WgState	state = WG_STATE_DISABLED;
			if( m_state.IsEnabled() && pI->m_bEnabled )
			{
				state = WG_STATE_NORMAL;

				if( itemNb == m_selectedItem )
					state = WG_STATE_PRESSED;
				else if( itemNb == m_markedItem )
					state = WG_STATE_HOVERED;
			}

			WgBorders b = GetEntryBorders();

//			WgColorsetPtr pTextColors;
			
//			if( m_pSkin )
//				pTextColors = m_pSkin->TextColors();

			if( m_pEntrySkin )
			{
				WgRect	dest( posX, window.y, pI->m_width + b.Width(), window.h );
				m_pEntrySkin->Render( pDevice, dest, state, clip );

//				pTextColors = m_pEntrySkin->TextColors();
			}

			pen.SetPos( WgCoord(posX + b.left, printPosY) );

			WgTextAttr	attr;
			WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextprop(), state );
//			WgTextTool::SetAttrColor( attr, pTextColors, mode );
			WgTextTool::AddPropAttributes( attr, m_pTextProp, state );
			pen.SetAttributes( attr );

			pDevice->PrintLine( pen, attr, pI->m_pText );

			posX += pI->m_width + b.Width();
		}
		itemNb++;
		pI = pI->Next();
	}
}

//____ _onEvent() _____________________________________________________________

void WgMenubar::_onEvent( WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	WgWidget::_onEvent(pEvent,pHandler);

	switch( pEvent->Type() )
	{
		case WG_EVENT_MOUSE_MOVE:
		case WG_EVENT_MOUSEBUTTON_PRESS:
		{
			WgCoord pos = pEvent->PointerPos();

			Uint32 item = GetItemAtAbsPos( pos.x, pos.y );

			if( item && !m_items.Get(item-1)->m_bEnabled )
				item = 0;								// Item is disabled and can't be marked.

			if(m_markedItem != item)
			{
				m_markedItem = item;
				_requestRender();
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

			if( item && pEvent->Type()== WG_EVENT_MOUSEBUTTON_PRESS )
			{
				OpenMenu( item );
			}

		}

		case WG_EVENT_MOUSE_LEAVE:
			m_markedItem = 0;
			_requestRender();
		break;

	}
}

//____ _onAction() _____________________________________________________________
/*
void WgMenubar::_onAction( WgInput::UserAction _action, int _button_key, const WgActionDetails& _info, const WgInput& _inputObj )
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
				_requestRender();
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
			_requestRender();
		break;

        default:
            break;
	}
}
*/

//____ _onRefresh() _______________________________________________________

void WgMenubar::_onRefresh( void )
{
	_requestRender();
}

//____ _onAlphaTest() ___________________________________________________

bool WgMenubar::_onAlphaTest( const WgCoord& ofs )
{
	return true;
}

//____ _onCloneContent() _______________________________________________________

void WgMenubar::_onCloneContent( const WgWidget * _pOrg )
{
	const WgMenubar * pOrg = (const WgMenubar *) _pOrg;

	m_pEntrySkin	= pOrg->m_pEntrySkin;
	m_pTextProp		= pOrg->m_pTextProp;

	//TODO: Clone entries!
}

//____ GetEntryBorders() ____________________________________________________

WgBorders WgMenubar::GetEntryBorders() const
{
	//TODO: This doesn't take ContentShift for different states into account.

	if( m_pEntrySkin )
	{
		WgRect r = m_pEntrySkin->ContentRect( WgRect(0,0,1000,1000), WG_STATE_NORMAL );

		return WgBorders(r.x,r.y,1000-r.w,1000-r.h);
	}
	else
		return WgBorders(10,0,10,0);		// 10 pixels on each side as default margin. Should do something more intelligent here, like taking fonts avgSpacing into account...
}



//____ OpenMenu() _____________________________________________________________

bool WgMenubar::OpenMenu( Uint32 nb )
{
	WgMenuBarItem * pItem = m_items.Get(nb-1);
	if( pItem == 0 || !pItem->m_pMenu )
		return false;

	WgCoord pos = Abs2local( WgCoord(0, 0) );

	if( m_pSkin )
		pos = m_pSkin->ContentRect( pos, WG_STATE_NORMAL ).Pos();

	int bordersWidth = GetEntryBorders().Width();

	WgMenuBarItem * pI = m_items.First();
	while( pI != pItem )
	{
		if( pI->IsVisible() )
			pos.x += pI->m_width + bordersWidth;

		pI = pI->Next();
	}

	WgRect	r(pos, pI->m_width+bordersWidth, Size().h );


	WgMenuLayer * pLayer = 0;
	if( Parent() )
		pLayer = Parent()->_getMenuLayer();
	if( !pLayer )
		return false;

	pLayer->OpenMenu( pItem->m_pMenu, this, r - pLayer->ScreenPos(), WG_SOUTHWEST );
	return true;
}

//____ CloseMenu() ____________________________________________________________

bool WgMenubar::CloseMenu( Uint32 nb )
{
	if( nb == 0 )
		return false;

	WgMenuBarItem * pItem = m_items.Get(m_selectedItem-1);
	if( !pItem || !pItem->m_pMenu )
		return false;


	WgMenuLayer * pLayer = 0;
	WgWidgetPtr pMenu = pItem->m_pMenu;
	if( Parent() )
		pLayer = Parent()->_getMenuLayer();

	if( !pLayer || !pMenu )
		return false;

	pLayer->CloseMenu( pMenu );

	return true;
}

//____ GetItemAtPos() _________________________________________________________

Uint32 WgMenubar::GetItemAtAbsPos( int x, int y )
{
	WgCoord pos = Abs2local( WgCoord(x, y) );

	if( m_pSkin )
		pos = m_pSkin->ContentRect( pos, WG_STATE_NORMAL ).Pos();

	if( y > 0 && x > 0 && y < (int) Size().h )
	{
		int bordersWidth = GetEntryBorders().Width();

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

void WgMenubar::MoveOutsideModal( int x, int y )
{
	Uint32 item = GetItemAtAbsPos( x, y );

	if( item && item != m_selectedItem && m_items.Get(item-1)->m_bEnabled )
	{
		CloseMenu( m_selectedItem );
		OpenMenu(item);
	}

}

//____ MenuOpened() ___________________________________________________________

void WgMenubar::MenuOpened( WgMenuBarItem * pItem )
{
	Uint32 item = pItem->Index()+1;
	m_selectedItem = item;
	_requestRender();
}

//____ MenuClosed() ___________________________________________________________

void WgMenubar::MenuClosed( WgMenuBarItem * pItem )
{
	m_selectedItem = 0;
	_requestRender();
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

//____ WgMenuBarItem::SetEnabled() _______________________________________________

void WgMenuBarItem::SetEnabled( bool bEnabled )
{
	if( m_bEnabled != bEnabled )
	{
		m_bEnabled = bEnabled;

		//TODO: Force unmark/unselect

		m_pMenuBar->_requestRender();		//TODO: Only render what is needed...
	}
}

//____ WgMenuBarItem::SetVisible() ________________________________________________

void WgMenuBarItem::SetVisible( bool bVisible )
{
	if( m_bVisible != bVisible )
	{
		//TODO: Force unmark/unselect on item that gets hidden

		//TODO: I believe we should refresh geometry here...

		m_bVisible = bVisible;
		m_pMenuBar->_requestRender();		//TODO: Only render what is needed...
	}
}
