
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
#include	<wctype.h>

#include	<wg_menu.h>
#include	<wg_popuplayer.h>
#include	<wg_scrollbar.h>
#include	<wg_surface.h>
#include	<wg_gfxdevice.h>
#include	<wg_char.h>
#include	<wg_font.h>
#include	<wg_texttool.h>
#include	<wg_pen.h>
#include	<wg_base.h>
#include	<wg_msgrouter.h>
#include	<wg_patches.h>

namespace wg 
{
	
	const char Menu::CLASSNAME[] = {"Menu"};
	static const char	c_hookType[] = {"MenuScrollbarHook"};
	
	
	//____ Constructor _____________________________________________________________
	
	Menu::Menu()
	{
		m_nItems				= 0;
		m_markedItem			= 0;
		m_pSelectedItem			= 0;
	
		m_pSkin					= 0;
		m_pSeparatorSkin		= 0;
		m_pCheckBoxSkin			= 0;
		m_pRadioButtonSkin		= 0;
	
		m_iconFieldWidth		= 0;
		m_arrowFieldWidth		= 0;
	
		m_pArrowAnim			= 0;
		m_arrowAnimCount		= 0;
	
		m_entryHeight			= 0;
		m_sepHeight				= 2;
	
		m_scrollbarBtnLayout		= Scrollbar::DEFAULT;
	
		m_contentHeight			= 0;
		m_contentOfs			= 0;
	
		m_nSelectorKeys			= 0;
		m_selectorCountdown		= 0;
	
		m_scrollbarHook.m_pParent = this;
		m_pOpenSubMenu			= 0;
	
		_refreshEntryHeight();
	}
	
	//____ ~Menu() __________________________________________________________
	
	Menu::~Menu()
	{
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Menu::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Panel::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Menu::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Menu_p Menu::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Menu_p( static_cast<Menu*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setSkin() __________________________________________________________
	
	bool Menu::setSkin( const Skin_p& pSkin, int iconFieldWidth, int arrowFieldWidth )
	{
		m_pSkin			= pSkin;
	
		m_iconFieldWidth	= iconFieldWidth;
		m_arrowFieldWidth	= arrowFieldWidth;
	
		_adjustSize();
		_requestRender();
		return true;
	
	}
	
	//____ setEntrySkin() _________________________________________________________
	
	void Menu::setEntrySkin( const Skin_p& pSkin )
	{
		if( pSkin != m_pEntrySkin )
		{
			m_pEntrySkin = pSkin;
		_refreshEntryHeight();
		_adjustSize();
		_requestRender();
	
		}
	}
	
	//____ setSeparatorSkin() ___________________________________________________
	
	bool Menu::setSeparatorSkin( const Skin_p& pSkin, const Border& borders )
	{
		m_pSeparatorSkin	= pSkin;
		m_sepBorder		= borders;
	
		if( pSkin )
			m_sepHeight	= m_pSeparatorSkin->preferredSize().h + m_sepBorder.height();
		else
			m_sepHeight = m_sepBorder.height();
	
		_adjustSize();
		_requestRender();
		return true;
	}
	
	//____ setArrowSource() _______________________________________________________
	
	bool Menu::setArrowSource( const GfxAnim_p& pAnim )
	{
		if( !pAnim )
			return false;
	
		m_pArrowAnim = pAnim;
		m_arrowAnimCount = 0;
	
		_requestRender();
		return true;
	}
	
	//____ setTextProperties() _____________________________________________________
	
	bool Menu::setTextProperties( const Textprop_p& pEntryProp, const Textprop_p& pKeyAccelProp )
	{
		m_pEntryProp 	= pEntryProp;
		m_pKeyAccelProp = pKeyAccelProp;
	
		// We need to modify MinWidth now that fonts might have changed
	
		MenuItem * pItem = m_items.first();
		while( pItem )
		{
			if( pItem->getType() != SEPARATOR )
				_calcEntryMinWidth( (MenuEntry*) pItem );
			pItem = pItem->next();
		}
	
		// Refresh everything affected...
	
		_refreshEntryHeight();
		_adjustSize();
		_requestRender();
		return true;
	}
	
	//____ refreshEntryHeight() ____________________________________________________
	
	void Menu::_refreshEntryHeight()
	{
			Pen		pen;
			TextAttr	attr;
			TextTool::addPropAttributes(attr, Base::getDefaultTextprop(), WG_STATE_NORMAL );
			TextTool::addPropAttributes(attr, m_pEntryProp, WG_STATE_NORMAL );
	
			pen.setAttributes( attr );
			int	heightNormal	= pen.getLineSpacing();
	
			attr.clear();
			TextTool::addPropAttributes(attr, Base::getDefaultTextprop(), WG_STATE_HOVERED );
			TextTool::addPropAttributes(attr, m_pEntryProp, WG_STATE_HOVERED );
			pen.setAttributes( attr );
			int heightMarked	= pen.getLineSpacing();
	
			attr.clear();
			TextTool::addPropAttributes(attr, Base::getDefaultTextprop(), WG_STATE_DISABLED );
			TextTool::addPropAttributes(attr, m_pEntryProp, WG_STATE_DISABLED );
			pen.setAttributes( attr );
			int heightDisabled	= pen.getLineSpacing();
	
			if( m_entryHeight < heightNormal )
				m_entryHeight = heightNormal;
	
			if( m_entryHeight < heightMarked )
				m_entryHeight = heightMarked;
	
			if( m_entryHeight < heightDisabled )
				m_entryHeight = heightDisabled;
	
			//
	
			if( m_pEntrySkin )
				m_entryHeight +=  m_pEntrySkin->contentPadding().h;
	}
	
	
	//____ SetScrollbarSkin() ______________________________________________________
	
	bool Menu::setScrollbarSkins(  const Skin_p& pBackgroundSkin, const Skin_p& pHandleSkin, const Skin_p& pBwdButtonSkin, const Skin_p& pFwdButtonSkin )
	{
		m_pScrollbarBgSkin		= pBackgroundSkin;
		m_pScrollbarHandleSkin	= pHandleSkin;
		m_pScrollbarBtnFwdSkin	= pFwdButtonSkin;
		m_pScrollbarBtnBwdSkin	= pBwdButtonSkin;
	
		if( m_scrollbarHook.scrollbar() )
		{
			//TODO: Adapt to changes in scrollbars minimum width.
	
			m_scrollbarHook.scrollbar()->setSkins( 0, pBackgroundSkin, pHandleSkin, pBwdButtonSkin, pFwdButtonSkin );
		}
		return true;
	}
	
	//____ setScrollbarButtonLayout() ________________________________________________
	
	bool Menu::setScrollbarButtonLayout(  Scrollbar::BtnLayout layout )
	{
		m_scrollbarBtnLayout = layout;
	
		if( m_scrollbarHook.scrollbar() )
			m_scrollbarHook.scrollbar()->setButtonLayout( layout );
	
		return true;
	}
	
	
	//____ setCheckBoxSkin() ____________________________________________________
	
	bool Menu::setCheckBoxSkin( const Skin_p& pSkin )
	{
		m_pCheckBoxSkin		= pSkin;
	
		_requestRender();
		return true;
	}
	
	//____ setRadioButtonSkin() _________________________________________________
	
	bool Menu::setRadioButtonSkin( const Skin_p& pSkin )
	{
		m_pRadioButtonSkin			= pSkin;
	
		_requestRender();
		return true;
	}
	
	//____ getEntryHeight() _______________________________________________________
	
	int Menu::getEntryHeight() const
	{
		return m_entryHeight;
	}
	
	//____ addItem() ______________________________________________________________
	
	int Menu::addItem( MenuItem * pItem )
	{
		// Calculate minWidth for all derived from ENTRY
	
		if( pItem->getType() != SEPARATOR )
			_calcEntryMinWidth( (MenuEntry*) pItem );
	
		// Let item do what it needs to
	
		pItem->setMyMenu( this );
	
		// Add the item.
	
		m_items.pushBack(pItem);
		m_nItems++;
	
		// Refresh what needs to be refreshed...
	
		_adjustSize();
		_requestRender();
	
		return m_nItems-1;		// Position of this item.
	}
	
	//____ insertItem() ___________________________________________________________
	
	int Menu::insertItem( MenuItem * pEntry, int pos )
	{
		// Calculate minWidth for all derived from ENTRY
	
		if( pEntry->getType() != SEPARATOR )
			_calcEntryMinWidth( (MenuEntry*) pEntry );
	
		// Let item do what it needs to
	
		pEntry->setMyMenu( this );
	
		// Add the item.
	
		MenuItem * pPos = m_items.get(pos);
		if( pPos )
			pEntry->moveBefore(pPos);
		else
		{
			pos = m_nItems;
			m_items.pushBack(pEntry);
		}
	
		m_nItems++;
	
		// Refresh what needs to be refreshed...
	
		_adjustSize();
		_requestRender();
	
		return pos;		// Position of this item.
	
	}
	
	//____ removeItem() ___________________________________________________________
	
	bool Menu::removeItem( MenuItem * pEntry )
	{
		if( !m_items.isMemberOf(pEntry) )
			return false;
	
		pEntry->disconnect();
		m_nItems--;
		_adjustSize();
		_requestRender();
		return true;
	}
	
	MenuItem* Menu::removeItem( int pos )
	{
		MenuItem * pEntry = getItem(pos);
		removeItem(pEntry);
		return pEntry;
	}
	
	//____ removeAllItems() _______________________________________________________
	
	void Menu::removeAllItems()
	{
		MenuItem* pItem = m_items.first();
		while( pItem )
		{
			pItem->setMyMenu(0);
			pItem->disconnect();
			pItem = m_items.first();
		}
	
		m_nItems = 0;
		_adjustSize();
		_requestRender();
	}
	
	//____ deleteItem() ___________________________________________________________
	
	bool Menu::deleteItem( MenuItem * pEntry )
	{
		if( !m_items.isMemberOf(pEntry) )
			return false;
	
		delete pEntry;
		m_nItems--;
		_adjustSize();
		_requestRender();
		return true;
	}
	
	bool Menu::deleteItem( int pos )
	{
		return deleteItem(getItem(pos));
	}
	
	//____ deleteAllItems() _______________________________________________________
	
	void Menu::deleteAllItems()
	{
		m_items.clear();
		m_nItems = 0;
		_adjustSize();
		_requestRender();
	}
	
	//____ getItemPos() ___________________________________________________________
	
	int Menu::getItemPos( MenuItem* pEntry )
	{
		if( m_items.isMemberOf( pEntry ) )
			return pEntry->index();
		else
			return -1;
	}
	
	//____ getItem() ______________________________________________________________
	
	MenuItem* Menu::getItem( int pos )
	{
		return m_items.get(pos);
	}
	
	//____ FindItem() ______________________________________________________________
	
	MenuItem* Menu::FindItem(int id)
	{
		MenuItem* pItem = m_items.first();
		for(; pItem; pItem = pItem->next())
		{
			if(pItem->getId() == id)
				return pItem;
		}
		return 0;
	}
	
	//____ preferredSize() ___________________________________________________________
	
	Size Menu::preferredSize() const
	{
		return m_defaultSize;
	}
	
	//____ matchingWidth() ________________________________________________________
	
	int  Menu::matchingWidth( int height ) const
	{
		if( height >= m_defaultSize.h )
			return m_defaultSize.w;
	
		int scrollbarWidth = 0;
	
		if( m_pScrollbarBgSkin && m_pScrollbarBgSkin->preferredSize().w > scrollbarWidth )
			scrollbarWidth = m_pScrollbarBgSkin->preferredSize().w;
	
		if( m_pScrollbarHandleSkin && m_pScrollbarHandleSkin->preferredSize().w > scrollbarWidth )
			scrollbarWidth = m_pScrollbarHandleSkin->preferredSize().w;
	
		if( m_pScrollbarBtnFwdSkin && m_pScrollbarBtnFwdSkin->preferredSize().w > scrollbarWidth )
			scrollbarWidth = m_pScrollbarBtnFwdSkin->preferredSize().w;
	
		if( m_pScrollbarBtnBwdSkin && m_pScrollbarBtnBwdSkin->preferredSize().w > scrollbarWidth )
			scrollbarWidth = m_pScrollbarBtnBwdSkin->preferredSize().w;
	
		return m_defaultSize.w + scrollbarWidth;
	}
	
	
	
	//____ _calcEntryMinWidth() _____________________________________________________
	
	void Menu::_calcEntryMinWidth( MenuEntry * pEntry )
	{
		TextAttr	entryAttr;
		TextTool::addPropAttributes(entryAttr, Base::getDefaultTextprop(), WG_STATE_NORMAL );
		TextTool::addPropAttributes(entryAttr, m_pEntryProp, WG_STATE_NORMAL);
		TextAttr	accelAttr;
		TextTool::addPropAttributes(accelAttr, Base::getDefaultTextprop(), WG_STATE_NORMAL );
		TextTool::addPropAttributes(accelAttr, m_pKeyAccelProp, WG_STATE_NORMAL);
	
		int wNormal = TextTool::lineWidth( entryAttr, "  " );
		int wMarked = TextTool::lineWidth( entryAttr, "  " );
	
		wNormal += TextTool::lineWidth( entryAttr, WG_STATE_NORMAL, pEntry->getText().chars() );
		wNormal += TextTool::lineWidth( accelAttr, WG_STATE_NORMAL, pEntry->getAccelText().chars() );
	
		entryAttr.clear();
		TextTool::addPropAttributes(entryAttr, Base::getDefaultTextprop(), WG_STATE_HOVERED );
		TextTool::addPropAttributes(entryAttr, m_pEntryProp, WG_STATE_HOVERED);
		accelAttr.clear();
		TextTool::addPropAttributes(accelAttr, Base::getDefaultTextprop(), WG_STATE_HOVERED );
		TextTool::addPropAttributes(accelAttr, m_pKeyAccelProp, WG_STATE_HOVERED);
	
		wMarked += TextTool::lineWidth( entryAttr, WG_STATE_NORMAL, pEntry->getText().chars() );
		wMarked += TextTool::lineWidth( accelAttr, WG_STATE_NORMAL, pEntry->getAccelText().chars() );
	
	
		if( wNormal > wMarked )
			pEntry->m_minWidth = wNormal;
		else
			pEntry->m_minWidth = wMarked;
	}
	
	//____ _getPadding() ____________________________________________________
	
	Border Menu::_getPadding() const
	{
		//TODO: This is ugly and doesn't take ContentShift of various states into account.
	
		if( m_pSkin )
		{
			Rect r = m_pSkin->contentRect( Rect(0,0,1000,1000), WG_STATE_NORMAL );
			return Border(r.x, r.y, 1000-r.w, 1000-r.h);
		}
		else
			return Border(0);
	}
	
	//____ _scrollItemIntoView() ___________________________________________________
	
	void Menu::_scrollItemIntoView( MenuItem * pItem, bool bForceAtTop )
	{
		MenuItem * p = m_items.first();
		int ofs = 0;
	
		while( p != pItem )
		{
			if( p->getType() == SEPARATOR )
				ofs += m_sepHeight;
			else
				ofs += m_entryHeight;
	
			p = p->next();
		}
	
		if( bForceAtTop || ofs < m_contentOfs )
			_setViewOfs(ofs);
		else if( ofs + m_entryHeight > m_contentOfs + _getViewSize() )
			_setViewOfs( ofs + m_entryHeight - _getViewSize() );
	
		_updateScrollbar( _getHandlePosition(), _getHandleSize() );
	}
	
	//____ _markFirstFilteredEntry() _______________________________________________
	
	void Menu::_markFirstFilteredEntry()
	{
		MenuItem * pItem = m_items.first();
	
		while( pItem )
		{
			if( pItem->getType() != SEPARATOR )
			{
				String str = ((MenuEntry*)pItem)->getText();
	
				if( str.length() >= m_nSelectorKeys )
				{
					const Char * pChars = str.chars();
	
					int i = 0;
					while( i < m_nSelectorKeys && towlower(pChars[i].getGlyph()) == m_selectorKeys[i] )
						i++;
	
					if( i == m_nSelectorKeys )
						break;
				}
			}
	
			pItem = pItem->next();
		}
	
	
		if( pItem )
		{
			m_markedItem = pItem->index()+1;
			_requestRender();
			_scrollItemIntoView( pItem );
		}
	
	}
	
	
	//____ _onRender() _____________________________________________________________
	
	void Menu::_onRender( GfxDevice * pDevice, const Rect& canvas, const Rect& window, const Rect& clip )
	{
		Widget::_onRender(pDevice,canvas,window,clip);
	
		// Render the menu-items
	
		Rect		contentRect = m_pSkin?m_pSkin->contentRect(canvas, m_state):canvas;
	
		Rect		contentClip( contentRect, clip );		// A clip rectangle for content.
	
		MenuItem * pItem = m_items.first();
	
		int		yPos = contentRect.y - m_contentOfs;
		int		xPosText = contentRect.x + m_iconFieldWidth;
		int		xPosIcon = contentRect.x;
		int		textFieldLen = contentRect.w - m_iconFieldWidth - m_arrowFieldWidth;
	
		Pen	entryPen( pDevice, Coord( xPosText, yPos ), contentClip );
		Pen	accelPen( pDevice, Coord( xPosText, yPos ), contentClip );
	
	
		unsigned int	item = 1;
		while( pItem )
		{
			if( pItem->isVisible() )
			{
				if( pItem->getType() == SEPARATOR )
				{
					if( m_pSeparatorSkin )
					{
						Rect	dest( contentRect.x, yPos + m_sepBorder.top,
									  contentRect.w, m_pSeparatorSkin->preferredSize().h );
	
						m_pSeparatorSkin->render( pDevice, dest, m_state, contentClip );
						yPos += m_sepHeight;
					}
				}
				else
				{
					State	state = WG_STATE_DISABLED;
	
					if( ((MenuEntry*)pItem)->isEnabled() )
						state = WG_STATE_NORMAL;
	
					if( item == m_markedItem )
						state = WG_STATE_HOVERED;
	
					// Render the tile for this entry
	
					Rect tileDest(	contentRect.x,
										yPos,
										contentRect.w,
										m_entryHeight );
	
	//				Rect tileClip( contentClip, tileDest);
	
					if( m_pEntrySkin )
						m_pEntrySkin->render( pDevice, tileDest, state, contentClip );
	
	
					// Print the text (if any)
	
					const Char * pText = ((MenuEntry*)pItem)->getText().chars();
					if( pText->getGlyph() != 0 )
					{
	
						TextAttr	attr;
						TextTool::addPropAttributes( attr, Base::getDefaultTextprop(), state );
	//					if( m_pSkin )
	//						TextTool::setAttrColor( attr, m_pSkin->TextColors(), mode );
						TextTool::addPropAttributes( attr, m_pEntryProp, state );
						entryPen.setAttributes( attr );
						int y = yPos + (m_entryHeight - entryPen.getLineHeight())/2 + entryPen.getBaseline();
						entryPen.setPos( Coord( xPosText, y ) );
						pDevice->printLine( entryPen, attr, pText );
					}
	
					// Print the accel text (if any)
	
					const Char * pAccelText = ((MenuEntry*)pItem)->getAccelText().chars();
					if( pAccelText->getGlyph() != 0 )
					{
						TextAttr	attr;
						TextTool::addPropAttributes( attr, Base::getDefaultTextprop(), state );
	//					if( m_pSkin )
	//						TextTool::setAttrColor( attr, m_pSkin->TextColors(), mode );
						TextTool::addPropAttributes( attr, m_pKeyAccelProp, state );
						accelPen.setAttributes( attr );
	
						int y = yPos + (m_entryHeight - accelPen.getLineHeight())/2 + accelPen.getBaseline();
						int width = TextTool::lineWidth( attr, state, pAccelText );
						int x = xPosText + textFieldLen - width;
	
						accelPen.setPos( Coord(x, y) );
						pDevice->printLine( accelPen, attr, pAccelText );
					}
	
					// Show the icon/checkbox/radiobutton (if any)
	
					switch( pItem->getType() )
					{
						case ENTRY:
						{
							Skin_p pIcon = ((MenuEntry*)pItem)->getIcon();
							if( pIcon )
							{
								Size sz = pIcon->preferredSize();
	
								if( sz.w > m_iconFieldWidth )
									sz.w = m_iconFieldWidth;
								if( sz.h > m_entryHeight )
									sz.h = m_entryHeight;
	
								int y = yPos + (m_entryHeight - sz.h)/2;
								int x = xPosIcon + (m_iconFieldWidth - sz.w)/2;
	
								pIcon->render( pDevice, Rect(x,y,sz), state, contentClip );
							}
						}
						break;
	
						case CHECKBOX:
						{
							State checkboxState = state;
	
							if( ((MenuCheckBox*)pItem)->isChecked() )
								checkboxState.setSelected(true);
	
							if( m_pCheckBoxSkin )
							{
								Size sz = m_pCheckBoxSkin->preferredSize();
	
								int y = yPos + (m_entryHeight - sz.h)/2;
								int x = xPosIcon + (m_iconFieldWidth - sz.w)/2;
	
								m_pCheckBoxSkin->render( pDevice, Rect(x,y,sz), checkboxState, contentClip );
							}
						}
						break;
						case RADIOBUTTON:
						{
							State radiobuttonState = state;
	
							if( ((MenuRadioButton*)pItem)->isSelected() )
								radiobuttonState.setSelected(true);
	
							if( m_pRadioButtonSkin )
							{
								Size sz = m_pRadioButtonSkin->preferredSize();
	
								int y = yPos + (m_entryHeight - sz.h)/2;
								int x = xPosIcon + (m_iconFieldWidth - sz.w)/2;
	
								m_pRadioButtonSkin->render( pDevice, Rect(x,y,sz), radiobuttonState, contentClip );
							}
						}
						break;
						case SUBMENU:
						{
							//TODO: Show submenu arrow.
						}
						break;
						default:
						break;
					}
	
	
					yPos += m_entryHeight;
				}
			}
	
			pItem = pItem->next();
			item++;
		}
	}
	
	//____ _onMsg() _____________________________________________________________
	
	void Menu::_onMsg( const Msg_p& pMsg )
	{
		Panel::_onMsg(pMsg);
		
		// TODO: Not handle or swallow key-messages if some modifier keys are pressed.
		
		switch( pMsg->type() )
		{
			case WG_MSG_TICK:
			{
				if( m_selectorCountdown > 0 )
				{
					TickMsg_p pTick = TickMsg::cast(pMsg);
	
					m_selectorCountdown -= pTick->timediff();
					if( m_selectorCountdown < 0 )
					{
						m_selectorCountdown = 0;
						m_nSelectorKeys = 0;
					}
				}
				return;
			}
	
			case WG_MSG_MOUSE_LEAVE:
			{
				// Unmark any selected item unless it is a submenu...
	
				MenuItem * pOldItem = m_items.get(m_markedItem-1);
				if( pOldItem && pOldItem->getType() != SUBMENU )
				{
					m_markedItem = 0;
					_requestRender();
				}
			}
			break;
	
			case WG_MSG_MOUSE_ENTER:
			case WG_MSG_MOUSE_MOVE:
			{
				Coord mousePos = InputMsg::cast(pMsg)->pointerPos() - globalPos();

				MenuItem * pItem = _getItemAtPos( mousePos.x, mousePos.y );
	
				uint32_t markedItem = 0;
				if( pItem )
				{
					if( pItem->getType() != SEPARATOR )
					{
						if( ((MenuEntry*)pItem)->isEnabled() )
							markedItem = pItem->index()+1;
					}
				}
	
				if( m_markedItem != markedItem )
				{
					// Mark and request render
		
					m_markedItem = markedItem;
					_requestRender();
	
					// Open/close submenus depending on what item we have marked.
	
					if( pItem && pItem->getType() == SUBMENU )
					{
						MenuSubMenu * pSubMenu = (MenuSubMenu*) pItem;
						
						if( pSubMenu != m_pOpenSubMenu )
						{
							if( m_pOpenSubMenu )
								_closeSubMenu( m_pOpenSubMenu );
								
							_openSubMenu( pSubMenu );
						}
							
					}
					else if( m_pOpenSubMenu )
					{
						_closeSubMenu( m_pOpenSubMenu );
					}
				}
			}
			break;
	
			case WG_MSG_MOUSE_RELEASE:
			{
				Coord mousePos = InputMsg::cast(pMsg)->pointerPos() - globalPos();

				MenuItem * pItem = _getItemAtPos( mousePos.x, mousePos.y );
				if( pItem )
					selectItem(pItem);
			}
			break;
	
			case WG_MSG_WHEEL_ROLL:
			{
				WheelRollMsg_p pEv = WheelRollMsg::cast(pMsg);
	
				if( pEv->wheel() == 1 )
				{
					int distance = pEv->distance().y;
	
					_setViewOfs( m_contentOfs - m_entryHeight*distance );
					_updateScrollbar( _getHandlePosition(), _getHandleSize() );
				}
			}
	
			case WG_MSG_TEXT_INPUT:
			{
				String text = TextInputMsg::cast(pMsg)->text();
				if( text.length() != 0 )
				{
					m_selectorCountdown = c_selectorCountdownStart;
	
					for( int i = 0 ; i < text.length() ; i++ )
					{
						if( m_nSelectorKeys < c_maxSelectorKeys )
						{
							m_selectorKeys[m_nSelectorKeys++] = towlower( text.chars()[i].getGlyph() );
						}
					}
					_markFirstFilteredEntry();
				}
			}
			break;
	
			case WG_MSG_KEY_PRESS:
			case WG_MSG_KEY_REPEAT:
			{
				MenuItem * pItem = 0;
				if( m_markedItem != 0 )
					pItem = m_items.get( m_markedItem-1 );
	
				int key = KeyMsg::cast(pMsg)->translatedKeyCode();
				switch( key )
				{
					case WG_KEY_ESCAPE:
						if( m_pOpenSubMenu )
						{
							_closeSubMenu( m_pOpenSubMenu );
						}
						break;
						
					case WG_KEY_LEFT:
						if( m_pOpenSubMenu )
						{
							_closeSubMenu( m_pOpenSubMenu );
						}
						break;
					
					case WG_KEY_RIGHT:
						if( pItem )
						{
							if( pItem->getType() == SUBMENU )
							{
								_openSubMenu( (MenuSubMenu*) pItem );
							}
						}
						break;
	
					case WG_KEY_RETURN:
						if( pItem )
						{
							if( pItem->getType() == SUBMENU )
								_openSubMenu( (MenuSubMenu*) pItem );
							else
							{
								selectItem(pItem);
								pItem = 0;				// So we won't mark an item in the closed menu.
							}
						}
						break;
	
					case WG_KEY_UP:
						if( pItem )
						{
							pItem = pItem->prev();
							while( pItem != 0 && (pItem->getType() == SEPARATOR || !pItem->isVisible() ) )
								pItem = pItem->prev();
						}
						break;
	
					case WG_KEY_DOWN:
						if( pItem )
							pItem = pItem->next();
						else
							pItem = m_items.first();
	
						while( pItem != 0 && (pItem->getType() == SEPARATOR || !pItem->isVisible() ) )
								pItem = pItem->next();
						break;
	
					case WG_KEY_HOME:
						pItem = m_items.first();
						while( pItem != 0 && (pItem->getType() == SEPARATOR || !pItem->isVisible() ) )
							pItem = pItem->next();
						break;
	
					case WG_KEY_END:
						pItem = m_items.last();
						while( pItem != 0 && (pItem->getType() == SEPARATOR || !pItem->isVisible() ))
							pItem = pItem->prev();
						break;
	
					case WG_KEY_PAGE_UP:
					{
						int viewHeight = _getViewSize();
	
						int distance = m_entryHeight;
						while( pItem != 0 && distance < viewHeight )
						{
							if( pItem->isVisible() )
							{
								if( pItem->getType() == SEPARATOR )
									distance += m_sepHeight;
								else
									distance += m_entryHeight;
							}
	
							pItem = pItem->prev();
						}
	
						if( !pItem )
						{
							pItem = m_items.first();
							while( pItem != 0 && (pItem->getType() == SEPARATOR || !pItem->isVisible() ))
								pItem = pItem->next();
						}
						break;
					}
					case WG_KEY_PAGE_DOWN:
					{
						int viewHeight = _getViewSize();
	
						int distance = m_entryHeight;
						while( pItem != 0 && distance < viewHeight )
						{
							if( pItem->isVisible() )
							{
								if( pItem->getType() == SEPARATOR )
									distance += m_sepHeight;
								else
									distance += m_entryHeight;
							}
	
							pItem = pItem->next();
						}
	
						if( !pItem )
						{
							pItem = m_items.last();
							while( pItem != 0 && (pItem->getType() == SEPARATOR || !pItem->isVisible() ))
								pItem = pItem->next();
						}
						break;
					}
				}
	
	
				if( pItem )
				{
					uint32_t markedItem = pItem->index()+1;
					if( markedItem != m_markedItem )
					{
						m_markedItem = markedItem;
						_scrollItemIntoView(pItem);
						_requestRender();
					}
				}
	
			}
			break;
	
	        default:
	            break;
	
		}
	
		// Swallow message depending on rules.
	
		if( pMsg->isMouseButtonMsg() && MouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
			pMsg->swallow();
		else if( pMsg->isKeyMsg() )
		{
			int key = KeyMsg::cast(pMsg)->translatedKeyCode();
			if( key == WG_KEY_RIGHT || key == WG_KEY_RETURN || key == WG_KEY_UP || key == WG_KEY_DOWN &&
				key == WG_KEY_HOME || key == WG_KEY_END || key == WG_KEY_PAGE_UP || key == WG_KEY_PAGE_DOWN &&
				key == WG_KEY_ESCAPE || key == WG_KEY_LEFT )
				pMsg->swallow();
		}
		else if( pMsg->type() == WG_MSG_TEXT_INPUT || pMsg->type() == WG_MSG_WHEEL_ROLL )
			pMsg->swallow();
	}
	
	//____ _onStateChanged() ______________________________________________________
	
	void Menu::_onStateChanged( State oldState )
	{
		Widget::_onStateChanged(oldState);
	
		if( m_state.isEnabled() != oldState.isEnabled() && m_scrollbarHook._widget() )
			m_scrollbarHook._widget()->setEnabled(m_state.isEnabled());
	
	}
	
	//____ selectItem() _________________________________________________
	void Menu::selectItem(MenuItem* pItem)
	{
		m_pSelectedItem = 0;
	
		if(pItem == 0 || !pItem->isVisible() )
			return;
	
		switch( pItem->getType() )
		{
			case ENTRY:
			{
				m_pSelectedItem = pItem;
	
				ItemInfo * pInfo = new ItemInfo[1];
				pInfo->id = pItem->getId();
												//TODO: Add index (and in the future pObject).
	
				Base::msgRouter()->post( new ItemsSelectMsg(this,1,pInfo));
				_itemSelected();
			}
			break;
			case CHECKBOX:
			{
				MenuCheckBox * pCheckBox = (MenuCheckBox*) pItem;
	
				if( pCheckBox->isChecked() )
				{
					pCheckBox->uncheck();
					Base::msgRouter()->post( new ItemToggleMsg(this,pItem->getId(),Object_p(),true));
				}
				else
				{
					pCheckBox->check();
					Base::msgRouter()->post( new ItemToggleMsg(this,pItem->getId(),Object_p(),false));
				}
	
				_itemSelected();
			}
			break;
			case RADIOBUTTON:
				if( m_pSelectedItem )
					Base::msgRouter()->post( new ItemToggleMsg(this,m_pSelectedItem->getId(),Object_p(),false));
				m_pSelectedItem = pItem;
				((MenuRadioButton*)pItem)->select();
				Base::msgRouter()->post( new ItemToggleMsg(this,pItem->getId(),Object_p(),true));
				_itemSelected();
			break;
	
			default:
	            break;
		}
	}
	
	//____ _findWidget() _____________________________________________________________
	
	Widget * Menu::_findWidget( const Coord& ofs, WgSearchMode mode )
	{
		Widget * pWidget = Panel::_findWidget(ofs, mode);
		if( !pWidget && markTest( ofs ) )
			return this;
	
		return pWidget;
	}
	
	//____ _openSubMenu() __________________________________________________________
	
	void Menu::_openSubMenu( MenuSubMenu * pItem )
	{
		Widget_p pMenu = pItem->getSubMenu();
	
		if( !pMenu )
			return;
	
		// Figure out Y-offset for pItem
	
		uint32_t yOfs = 0;
	
		MenuItem * p = m_items.first();
		while( p != pItem )
		{
			if( p->isVisible() )
			{
				if( p->getType() == SEPARATOR )
					yOfs += m_sepHeight;
				else
					yOfs += m_entryHeight;
			}
			p = p->next();
		}
	
		// Calculate itemArea
	
		Rect	geo = m_pSkin?m_pSkin->contentRect(globalGeo(), WG_STATE_NORMAL):globalGeo();
		Rect itemArea( geo.x, geo.y + yOfs, geo.w, m_entryHeight );
	
		// 
	
		PopupLayer * pLayer = 0;
	
		if( parent() )
			pLayer = parent()->_getPopupLayer();
	
		if( pLayer )
		{
			pLayer->openPopup( pMenu, this, itemArea - pLayer->globalPos(), WG_NORTHEAST );
			m_pOpenSubMenu = pItem;
		}
	}
	
	
	//____ _closeSubMenu() _________________________________________________________
	
	void Menu::_closeSubMenu( MenuSubMenu * pItem )
	{
		PopupLayer * pLayer = 0;
		Widget_p pMenu = pItem->getSubMenu();
	
		if( parent() )
			pLayer = parent()->_getPopupLayer();
	
		if( pLayer && pMenu )
		{	
			pLayer->closePopup( pMenu );
			m_pOpenSubMenu = 0;
		}
	}
	
	
	//____ _itemSelected() ______________________________________________________
	
	void Menu::_itemSelected()
	{
		PopupLayer * pLayer = 0;
	
		if( parent() )
			pLayer = parent()->_getPopupLayer();
	
		if( pLayer )
			pLayer->closeAllPopups();
	}
	
	//____ _renderPatches() ________________________________________________________
	
	void Menu::_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches )
	{
		Rect scrollbarGeo = _scrollbarGeo( _canvas );
	
		for( const Rect * pRect = _pPatches->begin() ; pRect != _pPatches->end() ; pRect++ )
		{
			// Render menu itself
	
			Rect clip( _canvas, *pRect );
			if( clip.w > 0 || clip.h > 0 )
				_onRender( pDevice, _canvas, _window, clip );
	
			// Render scrollbar if present.
	
			if( m_scrollbarHook._widget() )
			{
				Rect clip( scrollbarGeo, *pRect );
				if( clip.w > 0 || clip.h > 0 )
					((Scrollbar*)m_scrollbarHook._widget())->_onRender( pDevice, scrollbarGeo, scrollbarGeo, clip );
			}
		}
	}
	
	//____ _onCollectPatches() _____________________________________________________
	
	void Menu::_onCollectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
		container.add( Rect( geo, clip ) );
	}
	
	//____ _onMaskPatches() ________________________________________________________
	
	void Menu::_onMaskPatches( Patches& patches, const Rect& geo, const Rect& clip, WgBlendMode blendMode )
	{
		if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE )
		{
			patches.sub( Rect( geo, clip ) );
		}
		else if( blendMode == WG_BLENDMODE_BLEND && m_pSkin )
		{
			if( m_pSkin->isOpaque() )
				patches.sub( Rect( geo, clip ) );
			else if( m_pSkin->isOpaque() )
				patches.sub( Rect( geo, clip ) );
		}
	}
	
	//____ _onCloneContent() _______________________________________________________
	
	void Menu::_onCloneContent( const Widget * _pOrg )
	{
		const Menu * pOrg = static_cast<const Menu*>(_pOrg);
	
		m_iconFieldWidth 		= pOrg->m_iconFieldWidth;
		m_arrowFieldWidth 		= pOrg->m_arrowFieldWidth;
		m_pSeparatorSkin		= pOrg->m_pSeparatorSkin;
		m_sepBorder			= pOrg->m_sepBorder;
		m_pArrowAnim			= pOrg->m_pArrowAnim;
		m_entryHeight			= pOrg->m_entryHeight;
		m_sepHeight				= pOrg->m_sepHeight;
		m_pEntryProp			= pOrg->m_pEntryProp;
		m_pKeyAccelProp			= pOrg->m_pKeyAccelProp;
		m_pCheckBoxSkin			= pOrg->m_pCheckBoxSkin;
		m_pRadioButtonSkin		= pOrg->m_pRadioButtonSkin;
		m_pEntrySkin			= pOrg->m_pEntrySkin;
		m_pScrollbarBgSkin		= pOrg->m_pScrollbarBgSkin;
		m_pScrollbarHandleSkin	= pOrg->m_pScrollbarHandleSkin;
		m_pScrollbarBtnFwdSkin	= pOrg->m_pScrollbarBtnFwdSkin;
		m_pScrollbarBtnBwdSkin	= pOrg->m_pScrollbarBtnBwdSkin;
		m_scrollbarBtnLayout	= pOrg->m_scrollbarBtnLayout;
	
	
		//TODO: Implement cloning of menu items!
	}
	
	//____ _onRefresh() ____________________________________________________________
	void Menu::_onRefresh()
	{
		Widget::_onRefresh();
	
		//TODO: Implement!
	}
	
	
	//____ _onNewSize() ____________________________________________________________
	
	void Menu::_onNewSize( const Size& size )
	{
			_adjustSize();
	}
	
	//____ _onAlphaTest() ___________________________________________________
	
	bool Menu::_onAlphaTest( const Coord& ofs , const Size& sz)
	{
		return Widget::_onAlphaTest(ofs, sz);
	}
	
	//____ _firstHook() ____________________________________________________________
	
	Hook * Menu::_firstHook() const
	{
		if( m_scrollbarHook._widget() )
			return const_cast<ScrollbarHook*>(&m_scrollbarHook);
		else
			return 0;
	}
	
	//____ _scrollbarGeo() ___________________________________________________________
	
	Rect Menu::_scrollbarGeo( const Rect& menuGeo ) const
	{
		if( m_scrollbarHook._widget() )
		{
			Rect contentGeo = menuGeo - _getPadding();
			Rect scrollbarGeo( contentGeo.x + contentGeo.w - m_scrollbarHook.m_size.w, contentGeo.y, m_scrollbarHook.m_size.w, contentGeo.h );	//TODO: Scrollbar is now hardcoded to right side.
			return scrollbarGeo;
		}
		else
			return Rect();
	}
	
	
	//____ _firstHookWithGeo() _____________________________________________________
	
	Hook * Menu::_firstHookWithGeo( Rect& writeGeo ) const
	{
		if( m_scrollbarHook._widget() )
		{
			writeGeo = _scrollbarGeo( size() );
			return const_cast<ScrollbarHook*>(&m_scrollbarHook);
		}
		else
			return 0;
	}
	
	//____ _getItemAtPos() ______________________________________________________
	
	MenuItem * Menu::_getItemAtPos( int x, int y )
	{
		Border	contentBorder = _getPadding();
	
		x -= contentBorder.left;
		y -= contentBorder.top;
	
		y += m_contentOfs;
	
		if( y > 0 && x > 0 && x < (int) ( geo().w - contentBorder.right ) )
		{
			MenuItem * pItem = m_items.first();
			while( pItem )
			{
				if( pItem->isVisible() )
				{
					if( pItem->getType() == SEPARATOR )
						y -= m_sepHeight;
					else
						y -= m_entryHeight;
	
					if( y < 0 )
					{
						return pItem;
					}
				}
				pItem = pItem->next();
			}
		}
	
		return 0;
	}
	
	//____ _itemModified() _________________________________________________________
	
	void Menu::_itemModified()
	{
		_adjustSize();
		_requestRender();
	}
	
	//____ _adjustSize() ___________________________________________________________
	
	void Menu::_adjustSize()
	{
		Border contentBorder = _getPadding();
	
		int  w = contentBorder.width();
		int	 h = contentBorder.height();
	
		int minSep = m_sepBorder.width();
		if( m_pSeparatorSkin )
			minSep += m_pSeparatorSkin->minSize().w;
	
		MenuItem * pItem = m_items.first();
		while( pItem )
		{
			if( pItem->isVisible() )
			{
				if( pItem->getType() == SEPARATOR )
				{
					h += m_sepHeight;
					if( w < minSep )
						w = minSep;
				}
				else
				{
					h += m_entryHeight;
	
					int minW = ((MenuEntry*)pItem)->m_minWidth + contentBorder.width() + m_iconFieldWidth + m_arrowFieldWidth;
	
					if( w < minW )
						w = minW;
				}
			}
	
	
			pItem = pItem->next();
		}
	
		m_contentHeight = h - contentBorder.height();
	
		if( h < m_entryHeight + contentBorder.height() )
			h = m_entryHeight + contentBorder.height();
	
	
		if( w != m_defaultSize.w || h != m_defaultSize.h )
		{
			m_defaultSize.w = w;
			m_defaultSize.h = h;
			_requestResize();
		}
	
		if( h > size().h )
		{
			Scrollbar * pScrollbar = m_scrollbarHook.scrollbar();
			if( !pScrollbar )
			{
				pScrollbar = new Scrollbar();
				pScrollbar->setSkins( 0, m_pScrollbarBgSkin, m_pScrollbarHandleSkin, m_pScrollbarBtnBwdSkin, m_pScrollbarBtnFwdSkin );
				pScrollbar->setButtonLayout( m_scrollbarBtnLayout );
				pScrollbar->setScrollbarTarget(this);
			}
			Size scrollbarSize = pScrollbar->preferredSize();
	
			m_scrollbarHook.m_size.w = scrollbarSize.w;
			m_scrollbarHook.m_size.h = size().h - contentBorder.height();
	
			m_scrollbarHook._setWidget(pScrollbar);
	
			_updateScrollbar( _getHandlePosition(), _getHandleSize() );
		}
		else
		{
			if( m_scrollbarHook._widget() )
			{
				m_scrollbarHook._setWidget(0);
			}
		}
	
		_setViewOfs(m_contentOfs);		// Refresh offset.
	}
	
	
	//____ _stepFwd() ______________________________________________________________
	
	float Menu::_stepFwd()
	{
		_setViewOfs( m_contentOfs + m_entryHeight );
		return _getHandlePosition();
	}
	
	//____ _stepBwd() ______________________________________________________________
	
	float Menu::_stepBwd()
	{
		_setViewOfs( m_contentOfs - m_entryHeight );
		return _getHandlePosition();
	}
	
	//____ _jumpFwd() ______________________________________________________________
	
	float Menu::_jumpFwd()
	{
		int viewHeight = size().h - _getPadding().height();
		_setViewOfs( m_contentOfs + (viewHeight - m_entryHeight) );
		return _getHandlePosition();
	}
	
	//____ _jumpBwd() ______________________________________________________________
	
	float Menu::_jumpBwd()
	{
		int viewHeight = size().h - _getPadding().height();
		_setViewOfs( m_contentOfs - (viewHeight - m_entryHeight) );
		return _getHandlePosition();
	}
	
	//____ _wheelRolled() ______________________________________________________________
	
	float Menu::_wheelRolled( int distance )
	{
		int viewHeight = size().h - _getPadding().height();
		_setViewOfs( m_contentOfs + m_entryHeight*distance );
		return _getHandlePosition();
	}
	
	
	//____ _setPosition() __________________________________________________________
	
	float Menu::_setPosition( float fraction )
	{
		if( fraction < 0.f )
			fraction = 0.f;
	
		if( fraction > 1.f )
			fraction = 1.f;
	
		int viewHeight = size().h - _getPadding().height();
	
		int ofs = (int) (fraction * (m_contentHeight-viewHeight));
	
		_setViewOfs(ofs);
	
		return fraction;
	}
	
	
	//____ _getWidget() _____________________________________________________________
	
	Widget* Menu::_getWidget()
	{
		return this;
	}
	
	//____ _getHandlePosition() ____________________________________________________
	
	float Menu::_getHandlePosition()
	{
		return ((float)m_contentOfs) / (m_contentHeight-(size().h-_getPadding().height()));
	}
	
	//____ _getHandleSize() ________________________________________________________
	
	float Menu::_getHandleSize()
	{
		return ((float)(_getViewSize())) / m_contentHeight;
	}
	
	//____ _getViewSize() ____________________________________________________
	
	int Menu::_getViewSize()
	{
		return size().h-_getPadding().height();
	}
	
	//____ _setViewOfs() ______________________________________________________________
	
	void Menu::_setViewOfs(int pos)
	{
		int viewHeight = size().h - _getPadding().height();
	
		if( pos + viewHeight > m_contentHeight )
			pos = m_contentHeight - viewHeight;
	
		if( pos < 0 )
			pos = 0;
	
		if( pos != (int) m_contentOfs )
		{
			m_contentOfs = pos;
			_requestRender();
		}
	}
	
	
	const char * Menu::ScrollbarHook::type( void ) const
	{
		return classType();
	}
	
	const char * Menu::ScrollbarHook::classType()
	{
		return c_hookType;
	}
	
	Coord Menu::ScrollbarHook::pos() const
	{
		Size parentSize = m_pParent->size();
		Border borders = m_pParent->_getPadding();
		return Coord(parentSize.w-m_size.w-borders.right,borders.top);
	}
	
	Size Menu::ScrollbarHook::size() const
	{
		return m_size;
	}
	
	Rect Menu::ScrollbarHook::geo() const
	{
		Size parentSize = m_pParent->size();
		Border borders = m_pParent->_getPadding();
		return Rect(parentSize.w-m_size.w-borders.right,borders.top,m_size);
	}
	
	Coord Menu::ScrollbarHook::globalPos() const
	{
		Rect content = m_pParent->globalGeo() - m_pParent->_getPadding();
		return Coord( content.x + content.w - m_size.w, content.y);
	}
	
	Rect Menu::ScrollbarHook::globalGeo() const
	{
		Rect content = m_pParent->globalGeo() - m_pParent->_getPadding();
		return Rect( content.x + content.w - m_size.w, content.y, m_size );
	}
	
	void Menu::ScrollbarHook::_requestRender()
	{
		m_pParent->_requestRender( geo() );
	}
	
	void Menu::ScrollbarHook::_requestRender( const Rect& rect )
	{
		m_pParent->_requestRender( rect + pos() );
	}
	
	void Menu::ScrollbarHook::_requestResize()
	{
		// Do nothing.
	}
	

} // namespace wg
