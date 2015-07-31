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
#include	<wg_msg.h>
#include	<wg_msgrouter.h>
#include	<wg_surface.h>
#include	<wg_gfxdevice.h>
#include	<wg_texttool.h>
#include	<wg_popuplayer.h>

namespace wg 
{
	
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
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgMenubar::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgWidget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgMenubar::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgMenubar_p WgMenubar::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgMenubar_p( static_cast<WgMenubar*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setEntrySkin() _______________________________________________________
	
	bool WgMenubar::setEntrySkin( const WgSkin_p& pSkin, const WgTextprop_p& pTextProperties )
	{
		//TODO: Possibly resize if needed.
	
		m_pEntrySkin			= pSkin;
		m_pTextProp			= pTextProperties;
	
		_requestRender();
		return true;
	}
	
	//____ addMenu() ______________________________________________________________
	
	bool WgMenubar::addMenu( const char * pTitle, const WgMenu_p& pMenu, Uint16 navKey )
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
		WgTextTool::addPropAttributes( attr, WgBase::getDefaultTextprop(), WG_STATE_NORMAL);
		WgTextTool::addPropAttributes( attr, m_pTextProp, WG_STATE_NORMAL);
		Uint32 lineWidthNormal = WgTextTool::lineWidth( attr, WG_STATE_NORMAL, pItem->m_pText );
	
		attr.clear();
		WgTextTool::addPropAttributes( attr, WgBase::getDefaultTextprop(), WG_STATE_HOVERED);
		WgTextTool::addPropAttributes( attr, m_pTextProp, WG_STATE_HOVERED);
		Uint32 lineWidthMarked = WgTextTool::lineWidth( attr, WG_STATE_HOVERED, pItem->m_pText );
	
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
	
		m_items.pushBack(pItem);
		_requestRender();
		return true;
	}
	
	//____ removeMenu() ________________________________________________________
	bool WgMenubar::removeMenu( const WgMenu_p& pMenu )
	{
		for( WgMenuBarItem * pI = m_items.first(); pI; pI = pI->next() )
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
	
	//____ menuTitle() ________________________________________________________
	WgChar *WgMenubar::menuTitle( const WgMenu_p& pMenu) const
	{
		for( WgMenuBarItem * pI = m_items.first(); pI; pI = pI->next() )
		{
			if(pI->m_pMenu == pMenu)
				return pI->m_pText;
		}
		return 0;
	}
	
	//____ showMenu() ________________________________________________________
	bool WgMenubar::showMenu( const WgMenu_p& pMenu)
	{
		for( WgMenuBarItem * pI = m_items.first(); pI; pI = pI->next() )
		{
			if(pI->m_pMenu == pMenu)
			{
				pI->setVisible(true);
				return true;
			}
		}
		return false;
	}
	
	//____ hideMenu() ________________________________________________________
	bool WgMenubar::hideMenu(const WgMenu_p& pMenu)
	{
		for( WgMenuBarItem * pI = m_items.first(); pI; pI = pI->next() )
		{
			if(pI->m_pMenu == pMenu)
			{
				pI->setVisible(false);
				return true;
			}
		}
		return false;
	}
	
	//____ preferredSize() __________________________________________________________
	
	WgSize WgMenubar::preferredSize() const
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
			window = m_pSkin->contentRect( _canvas, m_state );
			clip.intersection( window, _clip );
		}
		else
		{
			window = _canvas;
			clip = _clip;
		}
	
		// Go throught the MenuBarItems and print their text and render their rectangles.
	
		WgMenuBarItem * pI = m_items.first();
		Uint32 posX = window.x;
	
		WgPen pen;
	
	
		WgTextAttr	attr;
		WgTextTool::addPropAttributes( attr, WgBase::getDefaultTextprop(), WG_STATE_NORMAL);
		WgTextTool::addPropAttributes( attr, m_pTextProp, WG_STATE_NORMAL);
	
		pen.setAttributes( attr );
		pen.setClipRect( clip );
		pen.setDevice( pDevice );
	
		Uint32 printPosY = window.y + ( window.h - pen.getLineHeight() )/2 + pen.getBaseline();
	
		Uint32 itemNb = 1;
		while( pI )
		{
			if( pI->isVisible() )
			{
				WgState	state = WG_STATE_DISABLED;
				if( m_state.isEnabled() && pI->m_bEnabled )
				{
					state = WG_STATE_NORMAL;
	
					if( itemNb == m_selectedItem )
						state = WG_STATE_PRESSED;
					else if( itemNb == m_markedItem )
						state = WG_STATE_HOVERED;
				}
	
				WgBorder b = _getEntryBorder();
	
	//			WgColorsetPtr pTextColors;
				
	//			if( m_pSkin )
	//				pTextColors = m_pSkin->TextColors();
	
				if( m_pEntrySkin )
				{
					WgRect	dest( posX, window.y, pI->m_width + b.width(), window.h );
					m_pEntrySkin->render( pDevice, dest, state, clip );
	
	//				pTextColors = m_pEntrySkin->TextColors();
				}
	
				pen.setPos( WgCoord(posX + b.left, printPosY) );
	
				WgTextAttr	attr;
				WgTextTool::addPropAttributes( attr, WgBase::getDefaultTextprop(), state );
	//			WgTextTool::setAttrColor( attr, pTextColors, mode );
				WgTextTool::addPropAttributes( attr, m_pTextProp, state );
				pen.setAttributes( attr );
	
				pDevice->printLine( pen, attr, pI->m_pText );
	
				posX += pI->m_width + b.width();
			}
			itemNb++;
			pI = pI->next();
		}
	}
	
	//____ _onMsg() _____________________________________________________________
	
	void WgMenubar::_onMsg( const WgMsg_p& pMsg )
	{
		WgWidget::_onMsg(pMsg);
	
		switch( pMsg->type() )
		{
			case WG_MSG_MOUSE_MOVE:
			case WG_MSG_MOUSE_PRESS:
			{
				WgCoord pos = pMsg->pointerPos() - globalPos();
	
				Uint32 item = _getItemAtAbsPos( pos.x, pos.y );
	
				if( item && !m_items.get(item-1)->m_bEnabled )
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
						_closeMenu( m_selectedItem );
						_openMenu( m_markedItem );
					}
				}
	
				// If this was a press, we need to select the item and open the menu.
	
				//TODO: A click on an already open entry should close the menu.
	
				if( item && pMsg->type()== WG_MSG_MOUSE_PRESS )
				{
					_openMenu( item );
				}
	
			}
	
			case WG_MSG_MOUSE_LEAVE:
				m_markedItem = 0;
				_requestRender();
			break;
	
		}
	}
	
	//____ _onRefresh() _______________________________________________________
	
	void WgMenubar::_onRefresh( void )
	{
		WgWidget::_onRefresh();
	}
	
	//____ _onAlphaTest() ___________________________________________________
	
	bool WgMenubar::_onAlphaTest( const WgCoord& ofs, const WgSize& sz )
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
	
	//____ _getEntryBorder() ____________________________________________________
	
	WgBorder WgMenubar::_getEntryBorder() const
	{
		//TODO: This doesn't take ContentShift for different states into account.
	
		if( m_pEntrySkin )
		{
			WgRect r = m_pEntrySkin->contentRect( WgRect(0,0,1000,1000), WG_STATE_NORMAL );
	
			return WgBorder(r.x,r.y,1000-r.w,1000-r.h);
		}
		else
			return WgBorder(10,0,10,0);		// 10 pixels on each side as default margin. Should do something more intelligent here, like taking fonts avgSpacing into account...
	}
	
	
	
	//____ _openMenu() _____________________________________________________________
	
	bool WgMenubar::_openMenu( int nb )
	{
		WgMenuBarItem * pItem = m_items.get(nb-1);
		if( pItem == 0 || !pItem->m_pMenu )
			return false;
	
		WgCoord pos = toLocal( WgCoord(0, 0) );
	
		if( m_pSkin )
			pos = m_pSkin->contentRect( pos, WG_STATE_NORMAL ).pos();
	
		int bordersWidth = _getEntryBorder().width();
	
		WgMenuBarItem * pI = m_items.first();
		while( pI != pItem )
		{
			if( pI->isVisible() )
				pos.x += pI->m_width + bordersWidth;
	
			pI = pI->next();
		}
	
		WgRect	r(pos, pI->m_width+bordersWidth, size().h );
	
	
		WgPopupLayer * pLayer = 0;
		if( parent() )
			pLayer = parent()->_getPopupLayer();
		if( !pLayer )
			return false;
	
		pLayer->openPopup( pItem->m_pMenu, this, r - pLayer->globalPos(), WG_SOUTHWEST );
		return true;
	}
	
	//____ _closeMenu() ____________________________________________________________
	
	bool WgMenubar::_closeMenu( int nb )
	{
		if( nb == 0 )
			return false;
	
		WgMenuBarItem * pItem = m_items.get(m_selectedItem-1);
		if( !pItem || !pItem->m_pMenu )
			return false;
	
	
		WgPopupLayer * pLayer = 0;
		WgWidget_p pMenu = pItem->m_pMenu;
		if( parent() )
			pLayer = parent()->_getPopupLayer();
	
		if( !pLayer || !pMenu )
			return false;
	
		pLayer->closePopup( pMenu );
	
		return true;
	}
	
	//____ _getItemAtPos() _________________________________________________________
	
	Uint32 WgMenubar::_getItemAtAbsPos( int x, int y )
	{
		WgCoord pos = toLocal( WgCoord(x, y) );
	
		if( m_pSkin )
			pos = m_pSkin->contentRect( pos, WG_STATE_NORMAL ).pos();
	
		if( y > 0 && x > 0 && y < (int) size().h )
		{
			int bordersWidth = _getEntryBorder().width();
	
			WgMenuBarItem * pItem = m_items.first();
			int		item = 1;
			while( pItem )
			{
				if(pItem->isVisible())
				{
					x -= pItem->m_width+bordersWidth;
	
					if( x < 0 )
						return item;
				}
	
				pItem = pItem->next();
				item++;
			}
		}
	
		return 0;
	}
	
	//____ _moveOutsideModal() _____________________________________________________
	
	void WgMenubar::_moveOutsideModal( int x, int y )
	{
		Uint32 item = _getItemAtAbsPos( x, y );
	
		if( item && item != m_selectedItem && m_items.get(item-1)->m_bEnabled )
		{
			_closeMenu( m_selectedItem );
			_openMenu(item);
		}
	
	}
	
	//____ _menuOpened() ___________________________________________________________
	
	void WgMenubar::_menuOpened( WgMenuBarItem * pItem )
	{
		Uint32 item = pItem->index()+1;
		m_selectedItem = item;
		_requestRender();
	}
	
	//____ _menuClosed() ___________________________________________________________
	
	void WgMenubar::_menuClosed( WgMenuBarItem * pItem )
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
	
	//____ WgMenuBarItem::setEnabled() _______________________________________________
	
	void WgMenuBarItem::setEnabled( bool bEnabled )
	{
		if( m_bEnabled != bEnabled )
		{
			m_bEnabled = bEnabled;
	
			//TODO: Force unmark/unselect
	
			m_pMenuBar->_requestRender();		//TODO: Only render what is needed...
		}
	}
	
	//____ WgMenuBarItem::setVisible() ________________________________________________
	
	void WgMenuBarItem::setVisible( bool bVisible )
	{
		if( m_bVisible != bVisible )
		{
			//TODO: Force unmark/unselect on item that gets hidden
	
			//TODO: I believe we should refresh geometry here...
	
			m_bVisible = bVisible;
			m_pMenuBar->_requestRender();		//TODO: Only render what is needed...
		}
	}

} // namespace wg
