
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

const char WgMenu::CLASSNAME[] = {"Menu"};
static const char	c_hookType[] = {"MenuScrollbarHook"};


//____ Constructor _____________________________________________________________

WgMenu::WgMenu()
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

	m_scrollbarBtnLayout		= WgScrollbar::DEFAULT;

	m_contentHeight			= 0;
	m_contentOfs			= 0;

	m_nSelectorKeys			= 0;
	m_selectorCountdown		= 0;

	m_scrollbarHook.m_pParent = this;
	m_pOpenSubMenu			= 0;

	_refreshEntryHeight();
}

//____ ~WgMenu() __________________________________________________________

WgMenu::~WgMenu()
{
}


//____ isInstanceOf() _________________________________________________________

bool WgMenu::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgPanel::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgMenu::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgMenu_p WgMenu::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgMenu_p( static_cast<WgMenu*>(pObject.rawPtr()) );

	return 0;
}

//____ setSkin() __________________________________________________________

bool WgMenu::setSkin( const WgSkin_p& pSkin, int iconFieldWidth, int arrowFieldWidth )
{
	m_pSkin			= pSkin;

	m_iconFieldWidth	= iconFieldWidth;
	m_arrowFieldWidth	= arrowFieldWidth;

	_adjustSize();
	_requestRender();
	return true;

}

//____ setEntrySkin() _________________________________________________________

void WgMenu::setEntrySkin( const WgSkin_p& pSkin )
{
	if( pSkin != m_pEntrySkin )
	{
		m_pEntrySkin = pSkin;
	_refreshEntryHeight();
	_adjustSize();
	_requestRender();

	}
}

//____ SetSeparatorSkin() ___________________________________________________

bool WgMenu::SetSeparatorSkin( const WgSkin_p& pSkin, const WgBorder& borders )
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

//____ SetArrowSource() _______________________________________________________

bool WgMenu::SetArrowSource( const WgGfxAnim_p& pAnim )
{
	if( !pAnim )
		return false;

	m_pArrowAnim = pAnim;
	m_arrowAnimCount = 0;

	_requestRender();
	return true;
}

//____ SetTextProperties() _____________________________________________________

bool WgMenu::SetTextProperties( const WgTextprop_p& pEntryProp, const WgTextprop_p& pKeyAccelProp )
{
	m_pEntryProp 	= pEntryProp;
	m_pKeyAccelProp = pKeyAccelProp;

	// We need to modify MinWidth now that fonts might have changed

	WgMenuItem * pItem = m_items.first();
	while( pItem )
	{
		if( pItem->getType() != SEPARATOR )
			_calcEntryMinWidth( (WgMenuEntry*) pItem );
		pItem = pItem->next();
	}

	// Refresh everything affected...

	_refreshEntryHeight();
	_adjustSize();
	_requestRender();
	return true;
}

//____ refreshEntryHeight() ____________________________________________________

void WgMenu::_refreshEntryHeight()
{
		WgPen		pen;
		WgTextAttr	attr;
		WgTextTool::addPropAttributes(attr, WgBase::getDefaultTextprop(), WG_STATE_NORMAL );
		WgTextTool::addPropAttributes(attr, m_pEntryProp, WG_STATE_NORMAL );

		pen.setAttributes( attr );
		int	heightNormal	= pen.getLineSpacing();

		attr.clear();
		WgTextTool::addPropAttributes(attr, WgBase::getDefaultTextprop(), WG_STATE_HOVERED );
		WgTextTool::addPropAttributes(attr, m_pEntryProp, WG_STATE_HOVERED );
		pen.setAttributes( attr );
		int heightMarked	= pen.getLineSpacing();

		attr.clear();
		WgTextTool::addPropAttributes(attr, WgBase::getDefaultTextprop(), WG_STATE_DISABLED );
		WgTextTool::addPropAttributes(attr, m_pEntryProp, WG_STATE_DISABLED );
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

bool WgMenu::SetScrollbarSkins(  const WgSkin_p& pBackgroundSkin, const WgSkin_p& pHandleSkin, const WgSkin_p& pBwdButtonSkin, const WgSkin_p& pFwdButtonSkin )
{
	m_pScrollbarBgSkin		= pBackgroundSkin;
	m_pScrollbarHandleSkin	= pHandleSkin;
	m_pScrollbarBtnFwdSkin	= pFwdButtonSkin;
	m_pScrollbarBtnBwdSkin	= pBwdButtonSkin;

	if( m_scrollbarHook.Scrollbar() )
	{
		//TODO: Adapt to changes in scrollbars minimum width.

		m_scrollbarHook.Scrollbar()->SetSkins( 0, pBackgroundSkin, pHandleSkin, pBwdButtonSkin, pFwdButtonSkin );
	}
	return true;
}

//____ SetScrollbarButtonLayout() ________________________________________________

bool WgMenu::SetScrollbarButtonLayout(  WgScrollbar::BtnLayout layout )
{
	m_scrollbarBtnLayout = layout;

	if( m_scrollbarHook.Scrollbar() )
		m_scrollbarHook.Scrollbar()->SetButtonLayout( layout );

	return true;
}


//____ SetCheckBoxSkin() ____________________________________________________

bool WgMenu::SetCheckBoxSkin( const WgSkin_p& pSkin )
{
	m_pCheckBoxSkin		= pSkin;

	_requestRender();
	return true;
}

//____ SetRadioButtonSkin() _________________________________________________

bool WgMenu::SetRadioButtonSkin( const WgSkin_p& pSkin )
{
	m_pRadioButtonSkin			= pSkin;

	_requestRender();
	return true;
}

//____ GetEntryHeight() _______________________________________________________

int WgMenu::GetEntryHeight() const
{
	return m_entryHeight;
}

//____ addItem() ______________________________________________________________

int WgMenu::addItem( WgMenuItem * pItem )
{
	// Calculate minWidth for all derived from ENTRY

	if( pItem->getType() != SEPARATOR )
		_calcEntryMinWidth( (WgMenuEntry*) pItem );

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

//____ InsertItem() ___________________________________________________________

int WgMenu::InsertItem( WgMenuItem * pEntry, int pos )
{
	// Calculate minWidth for all derived from ENTRY

	if( pEntry->getType() != SEPARATOR )
		_calcEntryMinWidth( (WgMenuEntry*) pEntry );

	// Let item do what it needs to

	pEntry->setMyMenu( this );

	// Add the item.

	WgMenuItem * pPos = m_items.get(pos);
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

bool WgMenu::removeItem( WgMenuItem * pEntry )
{
	if( !m_items.isMemberOf(pEntry) )
		return false;

	pEntry->disconnect();
	m_nItems--;
	_adjustSize();
	_requestRender();
	return true;
}

WgMenuItem* WgMenu::removeItem( int pos )
{
	WgMenuItem * pEntry = GetItem(pos);
	removeItem(pEntry);
	return pEntry;
}

//____ removeAllItems() _______________________________________________________

void WgMenu::removeAllItems()
{
	WgMenuItem* pItem = m_items.first();
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

//____ DeleteItem() ___________________________________________________________

bool WgMenu::DeleteItem( WgMenuItem * pEntry )
{
	if( !m_items.isMemberOf(pEntry) )
		return false;

	delete pEntry;
	m_nItems--;
	_adjustSize();
	_requestRender();
	return true;
}

bool WgMenu::DeleteItem( int pos )
{
	return DeleteItem(GetItem(pos));
}

//____ DeleteAllItems() _______________________________________________________

void WgMenu::DeleteAllItems()
{
	m_items.clear();
	m_nItems = 0;
	_adjustSize();
	_requestRender();
}

//____ GetItemPos() ___________________________________________________________

int WgMenu::GetItemPos( WgMenuItem* pEntry )
{
	if( m_items.isMemberOf( pEntry ) )
		return pEntry->index();
	else
		return -1;
}

//____ GetItem() ______________________________________________________________

WgMenuItem* WgMenu::GetItem( int pos )
{
	return m_items.get(pos);
}

//____ FindItem() ______________________________________________________________

WgMenuItem* WgMenu::FindItem(int id)
{
	WgMenuItem* pItem = m_items.first();
	for(; pItem; pItem = pItem->next())
	{
		if(pItem->getId() == id)
			return pItem;
	}
	return 0;
}

//____ preferredSize() ___________________________________________________________

WgSize WgMenu::preferredSize() const
{
	return m_defaultSize;
}

//____ matchingWidth() ________________________________________________________

int  WgMenu::matchingWidth( int height ) const
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

void WgMenu::_calcEntryMinWidth( WgMenuEntry * pEntry )
{
	WgTextAttr	entryAttr;
	WgTextTool::addPropAttributes(entryAttr, WgBase::getDefaultTextprop(), WG_STATE_NORMAL );
	WgTextTool::addPropAttributes(entryAttr, m_pEntryProp, WG_STATE_NORMAL);
	WgTextAttr	accelAttr;
	WgTextTool::addPropAttributes(accelAttr, WgBase::getDefaultTextprop(), WG_STATE_NORMAL );
	WgTextTool::addPropAttributes(accelAttr, m_pKeyAccelProp, WG_STATE_NORMAL);

	int wNormal = WgTextTool::lineWidth( entryAttr, "  " );
	int wMarked = WgTextTool::lineWidth( entryAttr, "  " );

	wNormal += WgTextTool::lineWidth( entryAttr, WG_STATE_NORMAL, pEntry->getText().chars() );
	wNormal += WgTextTool::lineWidth( accelAttr, WG_STATE_NORMAL, pEntry->getAccelText().chars() );

	entryAttr.clear();
	WgTextTool::addPropAttributes(entryAttr, WgBase::getDefaultTextprop(), WG_STATE_HOVERED );
	WgTextTool::addPropAttributes(entryAttr, m_pEntryProp, WG_STATE_HOVERED);
	accelAttr.clear();
	WgTextTool::addPropAttributes(accelAttr, WgBase::getDefaultTextprop(), WG_STATE_HOVERED );
	WgTextTool::addPropAttributes(accelAttr, m_pKeyAccelProp, WG_STATE_HOVERED);

	wMarked += WgTextTool::lineWidth( entryAttr, WG_STATE_NORMAL, pEntry->getText().chars() );
	wMarked += WgTextTool::lineWidth( accelAttr, WG_STATE_NORMAL, pEntry->getAccelText().chars() );


	if( wNormal > wMarked )
		pEntry->m_minWidth = wNormal;
	else
		pEntry->m_minWidth = wMarked;
}

//____ _getPadding() ____________________________________________________

WgBorder WgMenu::_getPadding() const
{
	//TODO: This is ugly and doesn't take ContentShift of various states into account.

	if( m_pSkin )
	{
		WgRect r = m_pSkin->contentRect( WgRect(0,0,1000,1000), WG_STATE_NORMAL );
		return WgBorder(r.x, r.y, 1000-r.w, 1000-r.h);
	}
	else
		return WgBorder(0);
}

//____ _scrollItemIntoView() ___________________________________________________

void WgMenu::_scrollItemIntoView( WgMenuItem * pItem, bool bForceAtTop )
{
	WgMenuItem * p = m_items.first();
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

void WgMenu::_markFirstFilteredEntry()
{
	WgMenuItem * pItem = m_items.first();

	while( pItem )
	{
		if( pItem->getType() != SEPARATOR )
		{
			WgString str = ((WgMenuEntry*)pItem)->getText();

			if( str.length() >= m_nSelectorKeys )
			{
				const WgChar * pChars = str.chars();

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

void WgMenu::_onRender( WgGfxDevice * pDevice, const WgRect& canvas, const WgRect& window, const WgRect& clip )
{
	WgWidget::_onRender(pDevice,canvas,window,clip);

	// Render the menu-items

	WgRect		contentRect = m_pSkin?m_pSkin->contentRect(canvas, m_state):canvas;

	WgRect		contentClip( contentRect, clip );		// A clip rectangle for content.

	WgMenuItem * pItem = m_items.first();

	int		yPos = contentRect.y - m_contentOfs;
	int		xPosText = contentRect.x + m_iconFieldWidth;
	int		xPosIcon = contentRect.x;
	int		textFieldLen = contentRect.w - m_iconFieldWidth - m_arrowFieldWidth;

	WgPen	entryPen( pDevice, WgCoord( xPosText, yPos ), contentClip );
	WgPen	accelPen( pDevice, WgCoord( xPosText, yPos ), contentClip );


	unsigned int	item = 1;
	while( pItem )
	{
		if( pItem->isVisible() )
		{
			if( pItem->getType() == SEPARATOR )
			{
				if( m_pSeparatorSkin )
				{
					WgRect	dest( contentRect.x, yPos + m_sepBorder.top,
								  contentRect.w, m_pSeparatorSkin->preferredSize().h );

					m_pSeparatorSkin->render( pDevice, dest, m_state, contentClip );
					yPos += m_sepHeight;
				}
			}
			else
			{
				WgState	state = WG_STATE_DISABLED;

				if( ((WgMenuEntry*)pItem)->isEnabled() )
					state = WG_STATE_NORMAL;

				if( item == m_markedItem )
					state = WG_STATE_HOVERED;

				// Render the tile for this entry

				WgRect tileDest(	contentRect.x,
									yPos,
									contentRect.w,
									m_entryHeight );

//				WgRect tileClip( contentClip, tileDest);

				if( m_pEntrySkin )
					m_pEntrySkin->render( pDevice, tileDest, state, contentClip );


				// Print the text (if any)

				const WgChar * pText = ((WgMenuEntry*)pItem)->getText().chars();
				if( pText->getGlyph() != 0 )
				{

					WgTextAttr	attr;
					WgTextTool::addPropAttributes( attr, WgBase::getDefaultTextprop(), state );
//					if( m_pSkin )
//						WgTextTool::setAttrColor( attr, m_pSkin->TextColors(), mode );
					WgTextTool::addPropAttributes( attr, m_pEntryProp, state );
					entryPen.setAttributes( attr );
					int y = yPos + (m_entryHeight - entryPen.getLineHeight())/2 + entryPen.getBaseline();
					entryPen.setPos( WgCoord( xPosText, y ) );
					pDevice->printLine( entryPen, attr, pText );
				}

				// Print the accel text (if any)

				const WgChar * pAccelText = ((WgMenuEntry*)pItem)->getAccelText().chars();
				if( pAccelText->getGlyph() != 0 )
				{
					WgTextAttr	attr;
					WgTextTool::addPropAttributes( attr, WgBase::getDefaultTextprop(), state );
//					if( m_pSkin )
//						WgTextTool::setAttrColor( attr, m_pSkin->TextColors(), mode );
					WgTextTool::addPropAttributes( attr, m_pKeyAccelProp, state );
					accelPen.setAttributes( attr );

					int y = yPos + (m_entryHeight - accelPen.getLineHeight())/2 + accelPen.getBaseline();
					int width = WgTextTool::lineWidth( attr, state, pAccelText );
					int x = xPosText + textFieldLen - width;

					accelPen.setPos( WgCoord(x, y) );
					pDevice->printLine( accelPen, attr, pAccelText );
				}

				// Show the icon/checkbox/radiobutton (if any)

				switch( pItem->getType() )
				{
					case ENTRY:
					{
						WgSkin_p pIcon = ((WgMenuEntry*)pItem)->getIcon();
						if( pIcon )
						{
							WgSize sz = pIcon->preferredSize();

							if( sz.w > m_iconFieldWidth )
								sz.w = m_iconFieldWidth;
							if( sz.h > m_entryHeight )
								sz.h = m_entryHeight;

							int y = yPos + (m_entryHeight - sz.h)/2;
							int x = xPosIcon + (m_iconFieldWidth - sz.w)/2;

							pIcon->render( pDevice, WgRect(x,y,sz), state, contentClip );
						}
					}
					break;

					case CHECKBOX:
					{
						WgState checkboxState = state;

						if( ((WgMenuCheckBox*)pItem)->isChecked() )
							checkboxState.setSelected(true);

						if( m_pCheckBoxSkin )
						{
							WgSize sz = m_pCheckBoxSkin->preferredSize();

							int y = yPos + (m_entryHeight - sz.h)/2;
							int x = xPosIcon + (m_iconFieldWidth - sz.w)/2;

							m_pCheckBoxSkin->render( pDevice, WgRect(x,y,sz), checkboxState, contentClip );
						}
					}
					break;
					case RADIOBUTTON:
					{
						WgState radiobuttonState = state;

						if( ((WgMenuRadioButton*)pItem)->isSelected() )
							radiobuttonState.setSelected(true);

						if( m_pRadioButtonSkin )
						{
							WgSize sz = m_pRadioButtonSkin->preferredSize();

							int y = yPos + (m_entryHeight - sz.h)/2;
							int x = xPosIcon + (m_iconFieldWidth - sz.w)/2;

							m_pRadioButtonSkin->render( pDevice, WgRect(x,y,sz), radiobuttonState, contentClip );
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

void WgMenu::_onMsg( const WgMsg_p& pMsg )
{
	WgPanel::_onMsg(pMsg);
	
	// TODO: Not handle or swallow key-messages if some modifier keys are pressed.

	WgCoord mousePos = pMsg->pointerPos() - globalPos();

	switch( pMsg->type() )
	{
		case WG_MSG_TICK:
		{
			if( m_selectorCountdown > 0 )
			{
				WgTickMsg_p pTick = WgTickMsg::cast(pMsg);

				m_selectorCountdown -= pTick->millisec();
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

			WgMenuItem * pOldItem = m_items.get(m_markedItem-1);
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
			WgMenuItem * pItem = _getItemAtPos( mousePos.x, mousePos.y );

			Uint32 markedItem = 0;
			if( pItem )
			{
				if( pItem->getType() != SEPARATOR )
				{
					if( ((WgMenuEntry*)pItem)->isEnabled() )
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
					WgMenuSubMenu * pSubMenu = (WgMenuSubMenu*) pItem;
					
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
			WgMenuItem * pItem = _getItemAtPos( mousePos.x, mousePos.y );
			if( pItem )
				SelectItem(pItem);
		}
		break;

		case WG_MSG_WHEEL_ROLL:
		{
			WgWheelRollMsg_p pEv = WgWheelRollMsg::cast(pMsg);

			if( pEv->wheel() == 1 )
			{
				int distance = pEv->distance();

				_setViewOfs( m_contentOfs - m_entryHeight*distance );
				_updateScrollbar( _getHandlePosition(), _getHandleSize() );
			}
		}

		case WG_MSG_CHARACTER:
		{
			Uint16 chr = WgCharacterMsg::cast(pMsg)->character();
			if( chr != 0 )
			{
				m_selectorCountdown = c_selectorCountdownStart;

				if( m_nSelectorKeys < c_maxSelectorKeys )
				{
					m_selectorKeys[m_nSelectorKeys++] = towlower( chr );
					_markFirstFilteredEntry();
				}
			}
		}
		break;

		case WG_MSG_KEY_PRESS:
		case WG_MSG_KEY_REPEAT:
		{
			WgMenuItem * pItem = 0;
			if( m_markedItem != 0 )
				pItem = m_items.get( m_markedItem-1 );

			int key = WgKeyMsg::cast(pMsg)->translatedKeyCode();
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
							_openSubMenu( (WgMenuSubMenu*) pItem );
						}
					}
					break;

				case WG_KEY_RETURN:
					if( pItem )
					{
						if( pItem->getType() == SUBMENU )
							_openSubMenu( (WgMenuSubMenu*) pItem );
						else
						{
							SelectItem(pItem);
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
				Uint32 markedItem = pItem->index()+1;
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

	if( pMsg->isMouseButtonMsg() && WgMouseButtonMsg::cast(pMsg)->button() == WG_BUTTON_LEFT )
		pMsg->swallow();
	else if( pMsg->isKeyMsg() )
	{
		int key = WgKeyMsg::cast(pMsg)->translatedKeyCode();
		if( key == WG_KEY_RIGHT || key == WG_KEY_RETURN || key == WG_KEY_UP || key == WG_KEY_DOWN &&
			key == WG_KEY_HOME || key == WG_KEY_END || key == WG_KEY_PAGE_UP || key == WG_KEY_PAGE_DOWN &&
			key == WG_KEY_ESCAPE || key == WG_KEY_LEFT )
			pMsg->swallow();
	}
	else if( pMsg->type() == WG_MSG_CHARACTER || pMsg->type() == WG_MSG_WHEEL_ROLL )
		pMsg->swallow();
}

//____ _onStateChanged() ______________________________________________________

void WgMenu::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);

	if( m_state.isEnabled() != oldState.isEnabled() && m_scrollbarHook._widget() )
		m_scrollbarHook._widget()->setEnabled(m_state.isEnabled());

}

//____ SelectItem() _________________________________________________
void WgMenu::SelectItem(WgMenuItem* pItem)
{
	m_pSelectedItem = 0;

	if(pItem == 0 || !pItem->isVisible() )
		return;

	switch( pItem->getType() )
	{
		case ENTRY:
		{
			m_pSelectedItem = pItem;

			WgItemInfo * pInfo = new WgItemInfo[1];
			pInfo->id = pItem->getId();
											//TODO: Add index (and in the future pObject).

			WgBase::msgRouter()->post( new WgItemsSelectMsg(this,1,pInfo));
			_itemSelected();
		}
		break;
		case CHECKBOX:
		{
			WgMenuCheckBox * pCheckBox = (WgMenuCheckBox*) pItem;

			if( pCheckBox->isChecked() )
			{
				pCheckBox->uncheck();
				WgBase::msgRouter()->post( new WgItemToggleMsg(this,pItem->getId(),WgObject_p(),true));
			}
			else
			{
				pCheckBox->check();
				WgBase::msgRouter()->post( new WgItemToggleMsg(this,pItem->getId(),WgObject_p(),false));
			}

			_itemSelected();
		}
		break;
		case RADIOBUTTON:
			if( m_pSelectedItem )
				WgBase::msgRouter()->post( new WgItemToggleMsg(this,m_pSelectedItem->getId(),WgObject_p(),false));
			m_pSelectedItem = pItem;
			((WgMenuRadioButton*)pItem)->select();
			WgBase::msgRouter()->post( new WgItemToggleMsg(this,pItem->getId(),WgObject_p(),true));
			_itemSelected();
		break;

		default:
            break;
	}
}

//____ _findWidget() _____________________________________________________________

WgWidget * WgMenu::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	WgWidget * pWidget = WgPanel::_findWidget(ofs, mode);
	if( !pWidget && markTest( ofs ) )
		return this;

	return pWidget;
}

//____ _openSubMenu() __________________________________________________________

void WgMenu::_openSubMenu( WgMenuSubMenu * pItem )
{
	WgWidget_p pMenu = pItem->getSubMenu();

	if( !pMenu )
		return;

	// Figure out Y-offset for pItem

	Uint32 yOfs = 0;

	WgMenuItem * p = m_items.first();
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

	WgRect	geo = m_pSkin?m_pSkin->contentRect(globalGeo(), WG_STATE_NORMAL):globalGeo();
	WgRect itemArea( geo.x, geo.y + yOfs, geo.w, m_entryHeight );

	// 

	WgPopupLayer * pLayer = 0;

	if( parent() )
		pLayer = parent()->_getPopupLayer();

	if( pLayer )
	{
		pLayer->openPopup( pMenu, this, itemArea - pLayer->globalPos(), WG_NORTHEAST );
		m_pOpenSubMenu = pItem;
	}
}


//____ _closeSubMenu() _________________________________________________________

void WgMenu::_closeSubMenu( WgMenuSubMenu * pItem )
{
	WgPopupLayer * pLayer = 0;
	WgWidget_p pMenu = pItem->getSubMenu();

	if( parent() )
		pLayer = parent()->_getPopupLayer();

	if( pLayer && pMenu )
	{	
		pLayer->closePopup( pMenu );
		m_pOpenSubMenu = 0;
	}
}


//____ _itemSelected() ______________________________________________________

void WgMenu::_itemSelected()
{
	WgPopupLayer * pLayer = 0;

	if( parent() )
		pLayer = parent()->_getPopupLayer();

	if( pLayer )
		pLayer->closeAllPopups();
}

//____ _renderPatches() ________________________________________________________

void WgMenu::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{
	WgRect scrollbarGeo = _scrollbarGeo( _canvas );

	for( const WgRect * pRect = _pPatches->begin() ; pRect != _pPatches->end() ; pRect++ )
	{
		// Render menu itself

		WgRect clip( _canvas, *pRect );
		if( clip.w > 0 || clip.h > 0 )
			_onRender( pDevice, _canvas, _window, clip );

		// Render scrollbar if present.

		if( m_scrollbarHook._widget() )
		{
			WgRect clip( scrollbarGeo, *pRect );
			if( clip.w > 0 || clip.h > 0 )
				((WgScrollbar*)m_scrollbarHook._widget())->_onRender( pDevice, scrollbarGeo, scrollbarGeo, clip );
		}
	}
}

//____ _onCollectPatches() _____________________________________________________

void WgMenu::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
	container.add( WgRect( geo, clip ) );
}

//____ _onMaskPatches() ________________________________________________________

void WgMenu::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE )
	{
		patches.sub( WgRect( geo, clip ) );
	}
	else if( blendMode == WG_BLENDMODE_BLEND && m_pSkin )
	{
		if( m_pSkin->isOpaque() )
			patches.sub( WgRect( geo, clip ) );
		else if( m_pSkin->isOpaque() )
			patches.sub( WgRect( geo, clip ) );
	}
}

//____ _onCloneContent() _______________________________________________________

void WgMenu::_onCloneContent( const WgWidget * _pOrg )
{
	const WgMenu * pOrg = static_cast<const WgMenu*>(_pOrg);

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
void WgMenu::_onRefresh()
{
	WgWidget::_onRefresh();

	//TODO: Implement!
}


//____ _onNewSize() ____________________________________________________________

void WgMenu::_onNewSize( const WgSize& size )
{
		_adjustSize();
}

//____ _onAlphaTest() ___________________________________________________

bool WgMenu::_onAlphaTest( const WgCoord& ofs , const WgSize& sz)
{
	return WgWidget::_onAlphaTest(ofs, sz);
}

//____ _firstHook() ____________________________________________________________

WgHook * WgMenu::_firstHook() const
{
	if( m_scrollbarHook._widget() )
		return const_cast<ScrollbarHook*>(&m_scrollbarHook);
	else
		return 0;
}

//____ _scrollbarGeo() ___________________________________________________________

WgRect WgMenu::_scrollbarGeo( const WgRect& menuGeo ) const
{
	if( m_scrollbarHook._widget() )
	{
		WgRect contentGeo = menuGeo - _getPadding();
		WgRect scrollbarGeo( contentGeo.x + contentGeo.w - m_scrollbarHook.m_size.w, contentGeo.y, m_scrollbarHook.m_size.w, contentGeo.h );	//TODO: Scrollbar is now hardcoded to right side.
		return scrollbarGeo;
	}
	else
		return WgRect();
}


//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgMenu::_firstHookWithGeo( WgRect& writeGeo ) const
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

WgMenuItem * WgMenu::_getItemAtPos( int x, int y )
{
	WgBorder	contentBorder = _getPadding();

	x -= contentBorder.left;
	y -= contentBorder.top;

	y += m_contentOfs;

	if( y > 0 && x > 0 && x < (int) ( geo().w - contentBorder.right ) )
	{
		WgMenuItem * pItem = m_items.first();
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

void WgMenu::_itemModified()
{
	_adjustSize();
	_requestRender();
}

//____ _adjustSize() ___________________________________________________________

void WgMenu::_adjustSize()
{
	WgBorder contentBorder = _getPadding();

	int  w = contentBorder.width();
	int	 h = contentBorder.height();

	int minSep = m_sepBorder.width();
	if( m_pSeparatorSkin )
		minSep += m_pSeparatorSkin->minSize().w;

	WgMenuItem * pItem = m_items.first();
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

				int minW = ((WgMenuEntry*)pItem)->m_minWidth + contentBorder.width() + m_iconFieldWidth + m_arrowFieldWidth;

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
		WgScrollbar * pScrollbar = m_scrollbarHook.Scrollbar();
		if( !pScrollbar )
		{
			pScrollbar = new WgScrollbar();
			pScrollbar->SetSkins( 0, m_pScrollbarBgSkin, m_pScrollbarHandleSkin, m_pScrollbarBtnBwdSkin, m_pScrollbarBtnFwdSkin );
			pScrollbar->SetButtonLayout( m_scrollbarBtnLayout );
			pScrollbar->SetScrollbarTarget(this);
		}
		WgSize scrollbarSize = pScrollbar->preferredSize();

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

float WgMenu::_stepFwd()
{
	_setViewOfs( m_contentOfs + m_entryHeight );
	return _getHandlePosition();
}

//____ _stepBwd() ______________________________________________________________

float WgMenu::_stepBwd()
{
	_setViewOfs( m_contentOfs - m_entryHeight );
	return _getHandlePosition();
}

//____ _jumpFwd() ______________________________________________________________

float WgMenu::_jumpFwd()
{
	int viewHeight = size().h - _getPadding().height();
	_setViewOfs( m_contentOfs + (viewHeight - m_entryHeight) );
	return _getHandlePosition();
}

//____ _jumpBwd() ______________________________________________________________

float WgMenu::_jumpBwd()
{
	int viewHeight = size().h - _getPadding().height();
	_setViewOfs( m_contentOfs - (viewHeight - m_entryHeight) );
	return _getHandlePosition();
}

//____ _wheelRolled() ______________________________________________________________

float WgMenu::_wheelRolled( int distance )
{
	int viewHeight = size().h - _getPadding().height();
	_setViewOfs( m_contentOfs + m_entryHeight*distance );
	return _getHandlePosition();
}


//____ _setPosition() __________________________________________________________

float WgMenu::_setPosition( float fraction )
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

WgWidget* WgMenu::_getWidget()
{
	return this;
}

//____ _getHandlePosition() ____________________________________________________

float WgMenu::_getHandlePosition()
{
	return ((float)m_contentOfs) / (m_contentHeight-(size().h-_getPadding().height()));
}

//____ _getHandleSize() ________________________________________________________

float WgMenu::_getHandleSize()
{
	return ((float)(_getViewSize())) / m_contentHeight;
}

//____ _getViewSize() ____________________________________________________

int WgMenu::_getViewSize()
{
	return size().h-_getPadding().height();
}

//____ _setViewOfs() ______________________________________________________________

void WgMenu::_setViewOfs(int pos)
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


const char * WgMenu::ScrollbarHook::type( void ) const
{
	return classType();
}

const char * WgMenu::ScrollbarHook::classType()
{
	return c_hookType;
}

WgCoord WgMenu::ScrollbarHook::pos() const
{
	WgSize parentSize = m_pParent->size();
	WgBorder borders = m_pParent->_getPadding();
	return WgCoord(parentSize.w-m_size.w-borders.right,borders.top);
}

WgSize WgMenu::ScrollbarHook::size() const
{
	return m_size;
}

WgRect WgMenu::ScrollbarHook::geo() const
{
	WgSize parentSize = m_pParent->size();
	WgBorder borders = m_pParent->_getPadding();
	return WgRect(parentSize.w-m_size.w-borders.right,borders.top,m_size);
}

WgCoord WgMenu::ScrollbarHook::globalPos() const
{
	WgRect content = m_pParent->globalGeo() - m_pParent->_getPadding();
	return WgCoord( content.x + content.w - m_size.w, content.y);
}

WgRect WgMenu::ScrollbarHook::globalGeo() const
{
	WgRect content = m_pParent->globalGeo() - m_pParent->_getPadding();
	return WgRect( content.x + content.w - m_size.w, content.y, m_size );
}

void WgMenu::ScrollbarHook::_requestRender()
{
	m_pParent->_requestRender( geo() );
}

void WgMenu::ScrollbarHook::_requestRender( const WgRect& rect )
{
	m_pParent->_requestRender( rect + pos() );
}

void WgMenu::ScrollbarHook::_requestResize()
{
	// Do nothing.
}

