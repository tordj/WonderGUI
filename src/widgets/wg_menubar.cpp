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
	
	const char Menubar::CLASSNAME[] = {"Menubar"};
	
	
	//____ Menubar() _________________________________________________________________
	
	Menubar::Menubar( void )
	{
		m_selectedItem	= 0;		// 0 = no item is selected.
		m_markedItem	= 0;		// 0 = no item is marked:
	
		m_pSkin			= 0;
		m_pEntrySkin	= 0;
	}
	
	//____ ~Menubar() __________________________________________________________
	
	Menubar::~Menubar( void )
	{
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Menubar::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Widget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Menubar::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Menubar_p Menubar::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Menubar_p( static_cast<Menubar*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setEntrySkin() _______________________________________________________
	
	bool Menubar::setEntrySkin( const Skin_p& pSkin, const Textprop_p& pTextProperties )
	{
		//TODO: Possibly resize if needed.
	
		m_pEntrySkin			= pSkin;
		m_pTextProp			= pTextProperties;
	
		_requestRender();
		return true;
	}
	
	//____ addMenu() ______________________________________________________________
	
	bool Menubar::addMenu( const char * pTitle, const Menu_p& pMenu, uint16_t navKey )
	{
		// Sanity check
	
		if( !pTitle || !pMenu )
			return false;
	
		// Convert/copy title
	
		MenuBarItem * pItem = new MenuBarItem();
	
		uint32_t strlen = TextTool::countChars( pTitle );
		pItem->m_pText = new Char[strlen+1];
	
		TextTool::readString(pTitle, pItem->m_pText, strlen+1);
	
		// Calculate linewidth
	
		TextAttr	attr;
		TextTool::addPropAttributes( attr, Base::getDefaultTextprop(), WG_STATE_NORMAL);
		TextTool::addPropAttributes( attr, m_pTextProp, WG_STATE_NORMAL);
		uint32_t lineWidthNormal = TextTool::lineWidth( attr, WG_STATE_NORMAL, pItem->m_pText );
	
		attr.clear();
		TextTool::addPropAttributes( attr, Base::getDefaultTextprop(), WG_STATE_HOVERED);
		TextTool::addPropAttributes( attr, m_pTextProp, WG_STATE_HOVERED);
		uint32_t lineWidthMarked = TextTool::lineWidth( attr, WG_STATE_HOVERED, pItem->m_pText );
	
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
	bool Menubar::removeMenu( const Menu_p& pMenu )
	{
		for( MenuBarItem * pI = m_items.first(); pI; pI = pI->next() )
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
	Char *Menubar::menuTitle( const Menu_p& pMenu) const
	{
		for( MenuBarItem * pI = m_items.first(); pI; pI = pI->next() )
		{
			if(pI->m_pMenu == pMenu)
				return pI->m_pText;
		}
		return 0;
	}
	
	//____ showMenu() ________________________________________________________
	bool Menubar::showMenu( const Menu_p& pMenu)
	{
		for( MenuBarItem * pI = m_items.first(); pI; pI = pI->next() )
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
	bool Menubar::hideMenu(const Menu_p& pMenu)
	{
		for( MenuBarItem * pI = m_items.first(); pI; pI = pI->next() )
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
	
	Size Menubar::preferredSize() const
	{
		//TODO: Implement!
	
		return Size(200,20);
	}
	
	
	//____ _onRender() ________________________________________________________
	
	void Menubar::_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip )
	{
		Widget::_onRender(pDevice,_canvas,_window,_clip);
	
		// Take backgrounds content borders into account
	
		Rect	window;
		Rect	clip;
	
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
	
		MenuBarItem * pI = m_items.first();
		uint32_t posX = window.x;
	
		Pen pen;
	
	
		TextAttr	attr;
		TextTool::addPropAttributes( attr, Base::getDefaultTextprop(), WG_STATE_NORMAL);
		TextTool::addPropAttributes( attr, m_pTextProp, WG_STATE_NORMAL);
	
		pen.setAttributes( attr );
		pen.setClipRect( clip );
		pen.setDevice( pDevice );
	
		uint32_t printPosY = window.y + ( window.h - pen.getLineHeight() )/2 + pen.getBaseline();
	
		uint32_t itemNb = 1;
		while( pI )
		{
			if( pI->isVisible() )
			{
				State	state = WG_STATE_DISABLED;
				if( m_state.isEnabled() && pI->m_bEnabled )
				{
					state = WG_STATE_NORMAL;
	
					if( itemNb == m_selectedItem )
						state = WG_STATE_PRESSED;
					else if( itemNb == m_markedItem )
						state = WG_STATE_HOVERED;
				}
	
				Border b = _getEntryBorder();
	
	//			ColorsetPtr pTextColors;
				
	//			if( m_pSkin )
	//				pTextColors = m_pSkin->TextColors();
	
				if( m_pEntrySkin )
				{
					Rect	dest( posX, window.y, pI->m_width + b.width(), window.h );
					m_pEntrySkin->render( pDevice, dest, state, clip );
	
	//				pTextColors = m_pEntrySkin->TextColors();
				}
	
				pen.setPos( Coord(posX + b.left, printPosY) );
	
				TextAttr	attr;
				TextTool::addPropAttributes( attr, Base::getDefaultTextprop(), state );
	//			TextTool::setAttrColor( attr, pTextColors, mode );
				TextTool::addPropAttributes( attr, m_pTextProp, state );
				pen.setAttributes( attr );
	
				pDevice->printLine( pen, attr, pI->m_pText );
	
				posX += pI->m_width + b.width();
			}
			itemNb++;
			pI = pI->next();
		}
	}
	
	//____ _onMsg() _____________________________________________________________
	
	void Menubar::_onMsg( const Msg_p& pMsg )
	{
		Widget::_onMsg(pMsg);
	
		switch( pMsg->type() )
		{
			case WG_MSG_MOUSE_MOVE:
			case WG_MSG_MOUSE_PRESS:
			{
				Coord pos = InputMsg::cast(pMsg)->pointerPos() - globalPos();
	
				uint32_t item = _getItemAtAbsPos( pos.x, pos.y );
	
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
	
	void Menubar::_onRefresh( void )
	{
		Widget::_onRefresh();
	}
	
	//____ _onAlphaTest() ___________________________________________________
	
	bool Menubar::_onAlphaTest( const Coord& ofs, const Size& sz )
	{
		return true;
	}
	
	//____ _onCloneContent() _______________________________________________________
	
	void Menubar::_onCloneContent( const Widget * _pOrg )
	{
		const Menubar * pOrg = (const Menubar *) _pOrg;
	
		m_pEntrySkin	= pOrg->m_pEntrySkin;
		m_pTextProp		= pOrg->m_pTextProp;
	
		//TODO: Clone entries!
	}
	
	//____ _getEntryBorder() ____________________________________________________
	
	Border Menubar::_getEntryBorder() const
	{
		//TODO: This doesn't take ContentShift for different states into account.
	
		if( m_pEntrySkin )
		{
			Rect r = m_pEntrySkin->contentRect( Rect(0,0,1000,1000), WG_STATE_NORMAL );
	
			return Border(r.x,r.y,1000-r.w,1000-r.h);
		}
		else
			return Border(10,0,10,0);		// 10 pixels on each side as default margin. Should do something more intelligent here, like taking fonts avgSpacing into account...
	}
	
	
	
	//____ _openMenu() _____________________________________________________________
	
	bool Menubar::_openMenu( int nb )
	{
		MenuBarItem * pItem = m_items.get(nb-1);
		if( pItem == 0 || !pItem->m_pMenu )
			return false;
	
		Coord pos = toLocal( Coord(0, 0) );
	
		if( m_pSkin )
			pos = m_pSkin->contentRect( pos, WG_STATE_NORMAL ).pos();
	
		int bordersWidth = _getEntryBorder().width();
	
		MenuBarItem * pI = m_items.first();
		while( pI != pItem )
		{
			if( pI->isVisible() )
				pos.x += pI->m_width + bordersWidth;
	
			pI = pI->next();
		}
	
		Rect	r(pos, pI->m_width+bordersWidth, size().h );
	
	
		PopupLayer * pLayer = 0;
		if( parent() )
			pLayer = parent()->_getPopupLayer();
		if( !pLayer )
			return false;
	
		pLayer->openPopup( pItem->m_pMenu, this, r - pLayer->globalPos(), WG_SOUTHWEST );
		return true;
	}
	
	//____ _closeMenu() ____________________________________________________________
	
	bool Menubar::_closeMenu( int nb )
	{
		if( nb == 0 )
			return false;
	
		MenuBarItem * pItem = m_items.get(m_selectedItem-1);
		if( !pItem || !pItem->m_pMenu )
			return false;
	
	
		PopupLayer * pLayer = 0;
		Widget_p pMenu = pItem->m_pMenu;
		if( parent() )
			pLayer = parent()->_getPopupLayer();
	
		if( !pLayer || !pMenu )
			return false;
	
		pLayer->closePopup( pMenu );
	
		return true;
	}
	
	//____ _getItemAtPos() _________________________________________________________
	
	uint32_t Menubar::_getItemAtAbsPos( int x, int y )
	{
		Coord pos = toLocal( Coord(x, y) );
	
		if( m_pSkin )
			pos = m_pSkin->contentRect( pos, WG_STATE_NORMAL ).pos();
	
		if( y > 0 && x > 0 && y < (int) size().h )
		{
			int bordersWidth = _getEntryBorder().width();
	
			MenuBarItem * pItem = m_items.first();
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
	
	void Menubar::_moveOutsideModal( int x, int y )
	{
		uint32_t item = _getItemAtAbsPos( x, y );
	
		if( item && item != m_selectedItem && m_items.get(item-1)->m_bEnabled )
		{
			_closeMenu( m_selectedItem );
			_openMenu(item);
		}
	
	}
	
	//____ _menuOpened() ___________________________________________________________
	
	void Menubar::_menuOpened( MenuBarItem * pItem )
	{
		uint32_t item = pItem->index()+1;
		m_selectedItem = item;
		_requestRender();
	}
	
	//____ _menuClosed() ___________________________________________________________
	
	void Menubar::_menuClosed( MenuBarItem * pItem )
	{
		m_selectedItem = 0;
		_requestRender();
	}
	
	
	//____ MenuBarItem::MenuBarItem() _________________________________________
	
	MenuBarItem::MenuBarItem()
	{
		m_pText		= 0;
		m_navKey	= 0;
		m_pMenu		= 0;
		m_width		= 0;
		m_bEnabled	= true;
		m_bVisible	= true;
	}
	
	//____ MenuBarItem::~MenuBarItem() ________________________________________
	
	MenuBarItem::~MenuBarItem()
	{
		if(m_pText != 0)
			delete [] m_pText;
	}
	
	//____ MenuBarItem::setEnabled() _______________________________________________
	
	void MenuBarItem::setEnabled( bool bEnabled )
	{
		if( m_bEnabled != bEnabled )
		{
			m_bEnabled = bEnabled;
	
			//TODO: Force unmark/unselect
	
			m_pMenuBar->_requestRender();		//TODO: Only render what is needed...
		}
	}
	
	//____ MenuBarItem::setVisible() ________________________________________________
	
	void MenuBarItem::setVisible( bool bVisible )
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
