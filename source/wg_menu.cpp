
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
#include	<wg_eventhandler.h>
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


//____ IsInstanceOf() _________________________________________________________

bool WgMenu::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgPanel::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgMenu::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgMenuPtr WgMenu::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgMenuPtr( static_cast<WgMenu*>(pObject.GetRealPtr()) );

	return 0;
}

//____ SetSkin() __________________________________________________________

bool WgMenu::SetSkin( const WgSkinPtr& pSkin, int iconFieldWidth, int arrowFieldWidth )
{
	m_pSkin			= pSkin;

	m_iconFieldWidth	= iconFieldWidth;
	m_arrowFieldWidth	= arrowFieldWidth;

	_adjustSize();
	_requestRender();
	return true;

}

//____ SetEntrySkin() _________________________________________________________

void WgMenu::SetEntrySkin( const WgSkinPtr& pSkin )
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

bool WgMenu::SetSeparatorSkin( const WgSkinPtr& pSkin, const WgBorders& borders )
{
	m_pSeparatorSkin	= pSkin;
	m_sepBorders		= borders;

	if( pSkin )
		m_sepHeight	= m_pSeparatorSkin->PreferredSize().h + m_sepBorders.Height();
	else
		m_sepHeight = m_sepBorders.Height();

	_adjustSize();
	_requestRender();
	return true;
}

//____ SetArrowSource() _______________________________________________________

bool WgMenu::SetArrowSource( const WgGfxAnimPtr& pAnim )
{
	if( !pAnim )
		return false;

	m_pArrowAnim = pAnim;
	m_arrowAnimCount = 0;

	_requestRender();
	return true;
}

//____ SetTextProperties() _____________________________________________________

bool WgMenu::SetTextProperties( const WgTextpropPtr& pEntryProp, const WgTextpropPtr& pKeyAccelProp )
{
	m_pEntryProp 	= pEntryProp;
	m_pKeyAccelProp = pKeyAccelProp;

	// We need to modify MinWidth now that fonts might have changed

	WgMenuItem * pItem = m_items.First();
	while( pItem )
	{
		if( pItem->GetType() != SEPARATOR )
			_calcEntryMinWidth( (WgMenuEntry*) pItem );
		pItem = pItem->Next();
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
		WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextprop(), WG_STATE_NORMAL );
		WgTextTool::AddPropAttributes(attr, m_pEntryProp, WG_STATE_NORMAL );

		pen.SetAttributes( attr );
		int	heightNormal	= pen.GetLineSpacing();

		attr.Clear();
		WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextprop(), WG_STATE_HOVERED );
		WgTextTool::AddPropAttributes(attr, m_pEntryProp, WG_STATE_HOVERED );
		pen.SetAttributes( attr );
		int heightMarked	= pen.GetLineSpacing();

		attr.Clear();
		WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextprop(), WG_STATE_DISABLED );
		WgTextTool::AddPropAttributes(attr, m_pEntryProp, WG_STATE_DISABLED );
		pen.SetAttributes( attr );
		int heightDisabled	= pen.GetLineSpacing();

		if( m_entryHeight < heightNormal )
			m_entryHeight = heightNormal;

		if( m_entryHeight < heightMarked )
			m_entryHeight = heightMarked;

		if( m_entryHeight < heightDisabled )
			m_entryHeight = heightDisabled;

		//

		if( m_pEntrySkin )
			m_entryHeight +=  m_pEntrySkin->ContentPadding().h;
}


//____ SetScrollbarSkin() ______________________________________________________

bool WgMenu::SetScrollbarSkins(  const WgSkinPtr& pBackgroundSkin, const WgSkinPtr& pHandleSkin, const WgSkinPtr& pBwdButtonSkin, const WgSkinPtr& pFwdButtonSkin )
{
	m_pScrollbarBgSkin		= pBackgroundSkin;
	m_pScrollbarHandleSkin	= pHandleSkin;
	m_pScrollbarBtnFwdSkin	= pFwdButtonSkin;
	m_pScrollbarBtnBwdSkin	= pBwdButtonSkin;

	if( m_scrollbarHook.Scrollbar() )
	{
		//TODO: Adapt to changes in scrollbars minimum width.

		m_scrollbarHook.Scrollbar()->SetSkins( pBackgroundSkin, pHandleSkin, pBwdButtonSkin, pFwdButtonSkin );
	}
	return true;
}

//____ SetScrollbarButtonLayout() ________________________________________________

bool WgMenu::SetScrollbarButtonLayout(  WgScrollbar::ButtonLayout layout )
{
	m_scrollbarBtnLayout = layout;

	if( m_scrollbarHook.Scrollbar() )
		m_scrollbarHook.Scrollbar()->SetButtonLayout( layout );

	return true;
}


//____ SetCheckBoxSkin() ____________________________________________________

bool WgMenu::SetCheckBoxSkin( const WgSkinPtr& pSkin )
{
	m_pCheckBoxSkin		= pSkin;

	_requestRender();
	return true;
}

//____ SetRadioButtonSkin() _________________________________________________

bool WgMenu::SetRadioButtonSkin( const WgSkinPtr& pSkin )
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

//____ AddItem() ______________________________________________________________

int WgMenu::AddItem( WgMenuItem * pItem )
{
	// Calculate minWidth for all derived from ENTRY

	if( pItem->GetType() != SEPARATOR )
		_calcEntryMinWidth( (WgMenuEntry*) pItem );

	// Let item do what it needs to

	pItem->SetMyMenu( this );

	// Add the item.

	m_items.PushBack(pItem);
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

	if( pEntry->GetType() != SEPARATOR )
		_calcEntryMinWidth( (WgMenuEntry*) pEntry );

	// Let item do what it needs to

	pEntry->SetMyMenu( this );

	// Add the item.

	WgMenuItem * pPos = m_items.Get(pos);
	if( pPos )
		pEntry->MoveBefore(pPos);
	else
	{
		pos = m_nItems;
		m_items.PushBack(pEntry);
	}

	m_nItems++;

	// Refresh what needs to be refreshed...

	_adjustSize();
	_requestRender();

	return pos;		// Position of this item.

}

//____ RemoveItem() ___________________________________________________________

bool WgMenu::RemoveItem( WgMenuItem * pEntry )
{
	if( !m_items.IsMemberOf(pEntry) )
		return false;

	pEntry->Disconnect();
	m_nItems--;
	_adjustSize();
	_requestRender();
	return true;
}

WgMenuItem* WgMenu::RemoveItem( int pos )
{
	WgMenuItem * pEntry = GetItem(pos);
	RemoveItem(pEntry);
	return pEntry;
}

//____ RemoveAllItems() _______________________________________________________

void WgMenu::RemoveAllItems()
{
	WgMenuItem* pItem = m_items.First();
	while( pItem )
	{
		pItem->SetMyMenu(0);
		pItem->Disconnect();
		pItem = m_items.First();
	}

	m_nItems = 0;
	_adjustSize();
	_requestRender();
}

//____ DeleteItem() ___________________________________________________________

bool WgMenu::DeleteItem( WgMenuItem * pEntry )
{
	if( !m_items.IsMemberOf(pEntry) )
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
	m_items.Clear();
	m_nItems = 0;
	_adjustSize();
	_requestRender();
}

//____ GetItemPos() ___________________________________________________________

int WgMenu::GetItemPos( WgMenuItem* pEntry )
{
	if( m_items.IsMemberOf( pEntry ) )
		return pEntry->Index();
	else
		return -1;
}

//____ GetItem() ______________________________________________________________

WgMenuItem* WgMenu::GetItem( int pos )
{
	return m_items.Get(pos);
}

//____ FindItem() ______________________________________________________________

WgMenuItem* WgMenu::FindItem(int id)
{
	WgMenuItem* pItem = m_items.First();
	for(; pItem; pItem = pItem->Next())
	{
		if(pItem->GetId() == id)
			return pItem;
	}
	return 0;
}

//____ PreferredSize() ___________________________________________________________

WgSize WgMenu::PreferredSize() const
{
	return m_defaultSize;
}

//____ WidthForHeight() ________________________________________________________

int  WgMenu::WidthForHeight( int height ) const
{
	if( height >= m_defaultSize.h )
		return m_defaultSize.w;

	int scrollbarWidth = 0;

	if( m_pScrollbarBgSkin && m_pScrollbarBgSkin->PreferredSize().w > scrollbarWidth )
		scrollbarWidth = m_pScrollbarBgSkin->PreferredSize().w;

	if( m_pScrollbarHandleSkin && m_pScrollbarHandleSkin->PreferredSize().w > scrollbarWidth )
		scrollbarWidth = m_pScrollbarHandleSkin->PreferredSize().w;

	if( m_pScrollbarBtnFwdSkin && m_pScrollbarBtnFwdSkin->PreferredSize().w > scrollbarWidth )
		scrollbarWidth = m_pScrollbarBtnFwdSkin->PreferredSize().w;

	if( m_pScrollbarBtnBwdSkin && m_pScrollbarBtnBwdSkin->PreferredSize().w > scrollbarWidth )
		scrollbarWidth = m_pScrollbarBtnBwdSkin->PreferredSize().w;

	return m_defaultSize.w + scrollbarWidth;
}



//____ _calcEntryMinWidth() _____________________________________________________

void WgMenu::_calcEntryMinWidth( WgMenuEntry * pEntry )
{
	WgTextAttr	entryAttr;
	WgTextTool::AddPropAttributes(entryAttr, WgBase::GetDefaultTextprop(), WG_STATE_NORMAL );
	WgTextTool::AddPropAttributes(entryAttr, m_pEntryProp, WG_STATE_NORMAL);
	WgTextAttr	accelAttr;
	WgTextTool::AddPropAttributes(accelAttr, WgBase::GetDefaultTextprop(), WG_STATE_NORMAL );
	WgTextTool::AddPropAttributes(accelAttr, m_pKeyAccelProp, WG_STATE_NORMAL);

	int wNormal = WgTextTool::lineWidth( 0, entryAttr, "  " );
	int wMarked = WgTextTool::lineWidth( 0, entryAttr, "  " );

	wNormal += WgTextTool::lineWidth( 0, entryAttr, WG_STATE_NORMAL, pEntry->GetText().Chars() );
	wNormal += WgTextTool::lineWidth( 0, accelAttr, WG_STATE_NORMAL, pEntry->GetAccelText().Chars() );

	entryAttr.Clear();
	WgTextTool::AddPropAttributes(entryAttr, WgBase::GetDefaultTextprop(), WG_STATE_HOVERED );
	WgTextTool::AddPropAttributes(entryAttr, m_pEntryProp, WG_STATE_HOVERED);
	accelAttr.Clear();
	WgTextTool::AddPropAttributes(accelAttr, WgBase::GetDefaultTextprop(), WG_STATE_HOVERED );
	WgTextTool::AddPropAttributes(accelAttr, m_pKeyAccelProp, WG_STATE_HOVERED);

	wMarked += WgTextTool::lineWidth( 0, entryAttr, WG_STATE_NORMAL, pEntry->GetText().Chars() );
	wMarked += WgTextTool::lineWidth( 0, accelAttr, WG_STATE_NORMAL, pEntry->GetAccelText().Chars() );


	if( wNormal > wMarked )
		pEntry->m_minWidth = wNormal;
	else
		pEntry->m_minWidth = wMarked;
}

//____ _getPadding() ____________________________________________________

WgBorders WgMenu::_getPadding() const
{
	//TODO: This is ugly and doesn't take ContentShift of various states into account.

	if( m_pSkin )
	{
		WgRect r = m_pSkin->ContentRect( WgRect(0,0,1000,1000), WG_STATE_NORMAL );
		return WgBorders(r.x, r.y, 1000-r.w, 1000-r.h);
	}
	else
		return WgBorders(0);
}

//____ _scrollItemIntoView() ___________________________________________________

void WgMenu::_scrollItemIntoView( WgMenuItem * pItem, bool bForceAtTop )
{
	WgMenuItem * p = m_items.First();
	int ofs = 0;

	while( p != pItem )
	{
		if( p->GetType() == SEPARATOR )
			ofs += m_sepHeight;
		else
			ofs += m_entryHeight;

		p = p->Next();
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
	WgMenuItem * pItem = m_items.First();

	while( pItem )
	{
		if( pItem->GetType() != SEPARATOR )
		{
			WgString str = ((WgMenuEntry*)pItem)->GetText();

			if( str.Length() >= m_nSelectorKeys )
			{
				const WgChar * pChars = str.Chars();

				int i = 0;
				while( i < m_nSelectorKeys && towlower(pChars[i].Glyph()) == m_selectorKeys[i] )
					i++;

				if( i == m_nSelectorKeys )
					break;
			}
		}

		pItem = pItem->Next();
	}


	if( pItem )
	{
		m_markedItem = pItem->Index()+1;
		_requestRender();
		_scrollItemIntoView( pItem );
	}

}


//____ _onRender() _____________________________________________________________

void WgMenu::_onRender( WgGfxDevice * pDevice, const WgRect& canvas, const WgRect& window, const WgRect& clip )
{
	WgWidget::_onRender(pDevice,canvas,window,clip);

	// Render the menu-items

	WgRect		contentRect = m_pSkin?m_pSkin->ContentRect(canvas, m_state):canvas;

	WgRect		contentClip( contentRect, clip );		// A clip rectangle for content.

	WgMenuItem * pItem = m_items.First();

	int		yPos = contentRect.y - m_contentOfs;
	int		xPosText = contentRect.x + m_iconFieldWidth;
	int		xPosIcon = contentRect.x;
	int		textFieldLen = contentRect.w - m_iconFieldWidth - m_arrowFieldWidth;

	WgPen	entryPen( pDevice, WgCoord( xPosText, yPos ), contentClip );
	WgPen	accelPen( pDevice, WgCoord( xPosText, yPos ), contentClip );


	unsigned int	item = 1;
	while( pItem )
	{
		if( pItem->IsVisible() )
		{
			if( pItem->GetType() == SEPARATOR )
			{
				if( m_pSeparatorSkin )
				{
					WgRect	dest( contentRect.x, yPos + m_sepBorders.top,
								  contentRect.w, m_pSeparatorSkin->PreferredSize().h );

					m_pSeparatorSkin->Render( pDevice, dest, m_state, contentClip );
					yPos += m_sepHeight;
				}
			}
			else
			{
				WgState	state = WG_STATE_DISABLED;

				if( ((WgMenuEntry*)pItem)->IsEnabled() )
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
					m_pEntrySkin->Render( pDevice, tileDest, state, contentClip );


				// Print the text (if any)

				const WgChar * pText = ((WgMenuEntry*)pItem)->GetText().Chars();
				if( pText->Glyph() != 0 )
				{

					WgTextAttr	attr;
					WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextprop(), state );
//					if( m_pSkin )
//						WgTextTool::SetAttrColor( attr, m_pSkin->TextColors(), mode );
					WgTextTool::AddPropAttributes( attr, m_pEntryProp, state );
					entryPen.SetAttributes( attr );
					int y = yPos + (m_entryHeight - entryPen.GetLineHeight())/2 + entryPen.GetBaseline();
					entryPen.SetPos( WgCoord( xPosText, y ) );
					pDevice->PrintLine( entryPen, attr, pText );
				}

				// Print the accel text (if any)

				const WgChar * pAccelText = ((WgMenuEntry*)pItem)->GetAccelText().Chars();
				if( pAccelText->Glyph() != 0 )
				{
					WgTextAttr	attr;
					WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextprop(), state );
//					if( m_pSkin )
//						WgTextTool::SetAttrColor( attr, m_pSkin->TextColors(), mode );
					WgTextTool::AddPropAttributes( attr, m_pKeyAccelProp, state );
					accelPen.SetAttributes( attr );

					int y = yPos + (m_entryHeight - accelPen.GetLineHeight())/2 + accelPen.GetBaseline();
					int width = WgTextTool::lineWidth( 0, attr, state, pAccelText );
					int x = xPosText + textFieldLen - width;

					accelPen.SetPos( WgCoord(x, y) );
					pDevice->PrintLine( accelPen, attr, pAccelText );
				}

				// Show the icon/checkbox/radiobutton (if any)

				switch( pItem->GetType() )
				{
					case ENTRY:
					{
						WgSkinPtr pIcon = ((WgMenuEntry*)pItem)->GetIcon();
						if( pIcon )
						{
							WgSize sz = pIcon->PreferredSize();

							if( sz.w > m_iconFieldWidth )
								sz.w = m_iconFieldWidth;
							if( sz.h > m_entryHeight )
								sz.h = m_entryHeight;

							int y = yPos + (m_entryHeight - sz.h)/2;
							int x = xPosIcon + (m_iconFieldWidth - sz.w)/2;

							pIcon->Render( pDevice, WgRect(x,y,sz), state, contentClip );
						}
					}
					break;

					case CHECKBOX:
					{
						WgState checkboxState = state;

						if( ((WgMenuCheckBox*)pItem)->IsChecked() )
							checkboxState.SetSelected(true);

						if( m_pCheckBoxSkin )
						{
							WgSize sz = m_pCheckBoxSkin->PreferredSize();

							int y = yPos + (m_entryHeight - sz.h)/2;
							int x = xPosIcon + (m_iconFieldWidth - sz.w)/2;

							m_pCheckBoxSkin->Render( pDevice, WgRect(x,y,sz), checkboxState, contentClip );
						}
					}
					break;
					case RADIOBUTTON:
					{
						WgState radiobuttonState = state;

						if( ((WgMenuRadioButton*)pItem)->IsSelected() )
							radiobuttonState.SetSelected(true);

						if( m_pRadioButtonSkin )
						{
							WgSize sz = m_pRadioButtonSkin->PreferredSize();

							int y = yPos + (m_entryHeight - sz.h)/2;
							int x = xPosIcon + (m_iconFieldWidth - sz.w)/2;

							m_pRadioButtonSkin->Render( pDevice, WgRect(x,y,sz), radiobuttonState, contentClip );
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

		pItem = pItem->Next();
		item++;
	}
}

//____ _onEvent() _____________________________________________________________

void WgMenu::_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler )
{
	// TODO: Not handle or swallow key-events if some modifier keys are pressed.

	WgCoord mousePos = pEvent->PointerPos();

	switch( pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
			if( m_selectorCountdown > 0 )
			{
				WgTickEventPtr pTick = WgTickEvent::Cast(pEvent);

				m_selectorCountdown -= pTick->Millisec();
				if( m_selectorCountdown < 0 )
				{
					m_selectorCountdown = 0;
					m_nSelectorKeys = 0;
				}
			}
			return;
		}

		case WG_EVENT_MOUSE_LEAVE:
		{
			// Unmark any selected item unless it is a submenu...

			WgMenuItem * pOldItem = m_items.Get(m_markedItem-1);
			if( pOldItem && pOldItem->GetType() != SUBMENU )
			{
				m_markedItem = 0;
				_requestRender();
			}
		}
		break;

		case WG_EVENT_MOUSE_ENTER:
		case WG_EVENT_MOUSE_MOVE:
		{
			WgMenuItem * pItem = _getItemAtPos( mousePos.x, mousePos.y );

			Uint32 markedItem = 0;
			if( pItem )
			{
				if( pItem->GetType() != SEPARATOR )
				{
					if( ((WgMenuEntry*)pItem)->IsEnabled() )
						markedItem = pItem->Index()+1;
				}
			}

			if( m_markedItem != markedItem )
			{
				// Mark and request render
	
				m_markedItem = markedItem;
				_requestRender();

				// Open/close submenus depending on what item we have marked.

				if( pItem && pItem->GetType() == SUBMENU )
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

		case WG_EVENT_MOUSE_RELEASE:
		{
			WgMenuItem * pItem = _getItemAtPos( mousePos.x, mousePos.y );
			if( pItem )
				SelectItem(pItem);
		}
		break;

		case WG_EVENT_WHEEL_ROLL:
		{
			WgWheelRollEventPtr pEv = WgWheelRollEvent::Cast(pEvent);

			if( pEv->Wheel() == 1 )
			{
				int distance = pEv->Distance();

				_setViewOfs( m_contentOfs - m_entryHeight*distance );
				_updateScrollbar( _getHandlePosition(), _getHandleSize() );
			}
		}

		case WG_EVENT_CHARACTER:
		{
			Uint16 chr = WgCharacterEvent::Cast(pEvent)->Char();
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

		case WG_EVENT_KEY_PRESS:
		case WG_EVENT_KEY_REPEAT:
		{
			WgMenuItem * pItem = 0;
			if( m_markedItem != 0 )
				pItem = m_items.Get( m_markedItem-1 );

			int key = WgKeyEvent::Cast(pEvent)->TranslatedKeyCode();
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
						if( pItem->GetType() == SUBMENU )
						{
							_openSubMenu( (WgMenuSubMenu*) pItem );
						}
					}
					break;

				case WG_KEY_RETURN:
					if( pItem )
					{
						if( pItem->GetType() == SUBMENU )
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
						pItem = pItem->Prev();
						while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsVisible() ) )
							pItem = pItem->Prev();
					}
					break;

				case WG_KEY_DOWN:
					if( pItem )
						pItem = pItem->Next();
					else
						pItem = m_items.First();

					while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsVisible() ) )
							pItem = pItem->Next();
					break;

				case WG_KEY_HOME:
					pItem = m_items.First();
					while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsVisible() ) )
						pItem = pItem->Next();
					break;

				case WG_KEY_END:
					pItem = m_items.Last();
					while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsVisible() ))
						pItem = pItem->Prev();
					break;

				case WG_KEY_PAGE_UP:
				{
					int viewHeight = _getViewSize();

					int distance = m_entryHeight;
					while( pItem != 0 && distance < viewHeight )
					{
						if( pItem->IsVisible() )
						{
							if( pItem->GetType() == SEPARATOR )
								distance += m_sepHeight;
							else
								distance += m_entryHeight;
						}

						pItem = pItem->Prev();
					}

					if( !pItem )
					{
						pItem = m_items.First();
						while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsVisible() ))
							pItem = pItem->Next();
					}
					break;
				}
				case WG_KEY_PAGE_DOWN:
				{
					int viewHeight = _getViewSize();

					int distance = m_entryHeight;
					while( pItem != 0 && distance < viewHeight )
					{
						if( pItem->IsVisible() )
						{
							if( pItem->GetType() == SEPARATOR )
								distance += m_sepHeight;
							else
								distance += m_entryHeight;
						}

						pItem = pItem->Next();
					}

					if( !pItem )
					{
						pItem = m_items.Last();
						while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsVisible() ))
							pItem = pItem->Next();
					}
					break;
				}
			}


			if( pItem )
			{
				Uint32 markedItem = pItem->Index()+1;
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

	// Forward event depending on rules.

	if( pEvent->IsMouseButtonEvent() && WgMouseButtonEvent::Cast(pEvent)->Button() == 1 )
		pHandler->SwallowEvent(pEvent);
	else if( pEvent->IsKeyEvent() )
	{
		int key = WgKeyEvent::Cast(pEvent)->TranslatedKeyCode();
		if( key == WG_KEY_RIGHT || key == WG_KEY_RETURN || key == WG_KEY_UP || key == WG_KEY_DOWN &&
			key == WG_KEY_HOME || key == WG_KEY_END || key == WG_KEY_PAGE_UP || key == WG_KEY_PAGE_DOWN &&
			key == WG_KEY_ESCAPE || key == WG_KEY_LEFT )
			pHandler->SwallowEvent(pEvent);
	}
	else if( pEvent->Type() == WG_EVENT_CHARACTER || pEvent->Type() == WG_EVENT_WHEEL_ROLL )
		pHandler->SwallowEvent(pEvent);
}

//____ _onStateChanged() ______________________________________________________

void WgMenu::_onStateChanged( WgState oldState, WgState newState )
{
	WgWidget::_onStateChanged(oldState,newState);

	if( newState.IsEnabled() != oldState.IsEnabled() && m_scrollbarHook._widget() )
		m_scrollbarHook._widget()->SetEnabled(newState.IsEnabled());

}

//____ SelectItem() _________________________________________________
void WgMenu::SelectItem(WgMenuItem* pItem)
{
	m_pSelectedItem = 0;

	if(pItem == 0 || !pItem->IsVisible() )
		return;

	switch( pItem->GetType() )
	{
		case ENTRY:
		{
			m_pSelectedItem = pItem;

			WgItemInfo * pInfo = new WgItemInfo[1];
			pInfo->id = pItem->GetId();
											//TODO: Add index (and in the future pObject).

			_queueEvent( new WgItemsSelectEvent(this,1,pInfo));
			_itemSelected();
		}
		break;
		case CHECKBOX:
		{
			WgMenuCheckBox * pCheckBox = (WgMenuCheckBox*) pItem;

			if( pCheckBox->IsChecked() )
			{
				pCheckBox->Uncheck();
				_queueEvent( new WgItemToggleEvent(this,-1,pItem->GetId(),WgObjectPtr(),true));
			}
			else
			{
				pCheckBox->Check();
				_queueEvent( new WgItemToggleEvent(this,-1,pItem->GetId(),WgObjectPtr(),false));
			}

			_itemSelected();
		}
		break;
		case RADIOBUTTON:
			if( m_pSelectedItem )
				_queueEvent( new WgItemToggleEvent(this,-1,m_pSelectedItem->GetId(),WgObjectPtr(),false));
			m_pSelectedItem = pItem;
			((WgMenuRadioButton*)pItem)->Select();
			_queueEvent( new WgItemToggleEvent(this,-1,pItem->GetId(),WgObjectPtr(),true));
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
	if( !pWidget && MarkTest( ofs ) )
		return this;

	return pWidget;
}

//____ _openSubMenu() __________________________________________________________

void WgMenu::_openSubMenu( WgMenuSubMenu * pItem )
{
	WgWidgetPtr pMenu = pItem->GetSubMenu();

	if( !pMenu )
		return;

	// Figure out Y-offset for pItem

	Uint32 yOfs = 0;

	WgMenuItem * p = m_items.First();
	while( p != pItem )
	{
		if( p->IsVisible() )
		{
			if( p->GetType() == SEPARATOR )
				yOfs += m_sepHeight;
			else
				yOfs += m_entryHeight;
		}
		p = p->Next();
	}

	// Calculate itemArea

	WgRect	geo = m_pSkin?m_pSkin->ContentRect(ScreenGeo(), WG_STATE_NORMAL):ScreenGeo();
	WgRect itemArea( geo.x, geo.y + yOfs, geo.w, m_entryHeight );

	// 

	WgPopupLayer * pLayer = 0;

	if( Parent() )
		pLayer = Parent()->_getPopupLayer();

	if( pLayer )
	{
		pLayer->OpenPopup( pMenu, this, itemArea - pLayer->ScreenPos(), WG_NORTHEAST );
		m_pOpenSubMenu = pItem;
	}
}


//____ _closeSubMenu() _________________________________________________________

void WgMenu::_closeSubMenu( WgMenuSubMenu * pItem )
{
	WgPopupLayer * pLayer = 0;
	WgWidgetPtr pMenu = pItem->GetSubMenu();

	if( Parent() )
		pLayer = Parent()->_getPopupLayer();

	if( pLayer && pMenu )
	{	
		pLayer->ClosePopup( pMenu );
		m_pOpenSubMenu = 0;
	}
}


//____ _itemSelected() ______________________________________________________

void WgMenu::_itemSelected()
{
	WgPopupLayer * pLayer = 0;

	if( Parent() )
		pLayer = Parent()->_getPopupLayer();

	if( pLayer )
		pLayer->CloseAllPopups();
}

//____ _renderPatches() ________________________________________________________

void WgMenu::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{
	WgRect scrollbarGeo = _scrollbarGeo( _canvas );

	for( const WgRect * pRect = _pPatches->Begin() ; pRect != _pPatches->End() ; pRect++ )
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
	container.Add( WgRect( geo, clip ) );
}

//____ _onMaskPatches() ________________________________________________________

void WgMenu::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE )
	{
		patches.Sub( WgRect( geo, clip ) );
	}
	else if( blendMode == WG_BLENDMODE_BLEND && m_pSkin )
	{
		if( m_pSkin->IsOpaque() )
			patches.Sub( WgRect( geo, clip ) );
		else if( m_pSkin->IsOpaque() )
			patches.Sub( WgRect( geo, clip ) );
	}
}

//____ _onCloneContent() _______________________________________________________

void WgMenu::_onCloneContent( const WgWidget * _pOrg )
{
	const WgMenu * pOrg = static_cast<const WgMenu*>(_pOrg);

	m_iconFieldWidth 		= pOrg->m_iconFieldWidth;
	m_arrowFieldWidth 		= pOrg->m_arrowFieldWidth;
	m_pSeparatorSkin		= pOrg->m_pSeparatorSkin;
	m_sepBorders			= pOrg->m_sepBorders;
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
	//TODO: Implement!
}


//____ _onNewSize() ____________________________________________________________

void WgMenu::_onNewSize( const WgSize& size )
{
		_adjustSize();
}

//____ _onAlphaTest() ___________________________________________________

bool WgMenu::_onAlphaTest( const WgCoord& ofs )
{
	return WgWidget::_onAlphaTest(ofs);
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
		writeGeo = _scrollbarGeo( Size() );
		return const_cast<ScrollbarHook*>(&m_scrollbarHook);
	}
	else
		return 0;
}

//____ _getItemAtPos() ______________________________________________________

WgMenuItem * WgMenu::_getItemAtPos( int x, int y )
{
	WgBorders	contentBorders = _getPadding();

	x -= contentBorders.left;
	y -= contentBorders.top;

	y += m_contentOfs;

	if( y > 0 && x > 0 && x < (int) ( Geo().w - contentBorders.right ) )
	{
		WgMenuItem * pItem = m_items.First();
		while( pItem )
		{
			if( pItem->IsVisible() )
			{
				if( pItem->GetType() == SEPARATOR )
					y -= m_sepHeight;
				else
					y -= m_entryHeight;

				if( y < 0 )
				{
					return pItem;
				}
			}
			pItem = pItem->Next();
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
	WgBorders contentBorders = _getPadding();

	int  w = contentBorders.Width();
	int	 h = contentBorders.Height();

	int minSep = m_sepBorders.Width();
	if( m_pSeparatorSkin )
		minSep += m_pSeparatorSkin->MinSize().w;

	WgMenuItem * pItem = m_items.First();
	while( pItem )
	{
		if( pItem->IsVisible() )
		{
			if( pItem->GetType() == SEPARATOR )
			{
				h += m_sepHeight;
				if( w < minSep )
					w = minSep;
			}
			else
			{
				h += m_entryHeight;

				int minW = ((WgMenuEntry*)pItem)->m_minWidth + contentBorders.Width() + m_iconFieldWidth + m_arrowFieldWidth;

				if( w < minW )
					w = minW;
			}
		}


		pItem = pItem->Next();
	}

	m_contentHeight = h - contentBorders.Height();

	if( h < m_entryHeight + contentBorders.Height() )
		h = m_entryHeight + contentBorders.Height();


	if( w != m_defaultSize.w || h != m_defaultSize.h )
	{
		m_defaultSize.w = w;
		m_defaultSize.h = h;
		_requestResize();
	}

	if( h > Size().h )
	{
		WgScrollbar * pScrollbar = m_scrollbarHook.Scrollbar();
		if( !pScrollbar )
		{
			pScrollbar = new WgVScrollbar();
			pScrollbar->SetSkins( m_pScrollbarBgSkin, m_pScrollbarHandleSkin, m_pScrollbarBtnBwdSkin, m_pScrollbarBtnFwdSkin );
			pScrollbar->SetButtonLayout( m_scrollbarBtnLayout );
			pScrollbar->SetScrollbarTarget(this);
		}
		WgSize scrollbarSize = pScrollbar->PreferredSize();

		m_scrollbarHook.m_size.w = scrollbarSize.w;
		m_scrollbarHook.m_size.h = Size().h - contentBorders.Height();

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
	int viewHeight = Size().h - _getPadding().Height();
	_setViewOfs( m_contentOfs + (viewHeight - m_entryHeight) );
	return _getHandlePosition();
}

//____ _jumpBwd() ______________________________________________________________

float WgMenu::_jumpBwd()
{
	int viewHeight = Size().h - _getPadding().Height();
	_setViewOfs( m_contentOfs - (viewHeight - m_entryHeight) );
	return _getHandlePosition();
}

//____ _wheelRolled() ______________________________________________________________

float WgMenu::_wheelRolled( int distance )
{
	int viewHeight = Size().h - _getPadding().Height();
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

	int viewHeight = Size().h - _getPadding().Height();

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
	return ((float)m_contentOfs) / (m_contentHeight-(Size().h-_getPadding().Height()));
}

//____ _getHandleSize() ________________________________________________________

float WgMenu::_getHandleSize()
{
	return ((float)(_getViewSize())) / m_contentHeight;
}

//____ _getViewSize() ____________________________________________________

int WgMenu::_getViewSize()
{
	return Size().h-_getPadding().Height();
}

//____ _setViewOfs() ______________________________________________________________

void WgMenu::_setViewOfs(int pos)
{
	int viewHeight = Size().h - _getPadding().Height();

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


const char * WgMenu::ScrollbarHook::Type( void ) const
{
	return ClassType();
}

const char * WgMenu::ScrollbarHook::ClassType()
{
	return c_hookType;
}

WgCoord WgMenu::ScrollbarHook::Pos() const
{
	WgSize parentSize = m_pParent->Size();
	WgBorders borders = m_pParent->_getPadding();
	return WgCoord(parentSize.w-m_size.w-borders.right,borders.top);
}

WgSize WgMenu::ScrollbarHook::Size() const
{
	return m_size;
}

WgRect WgMenu::ScrollbarHook::Geo() const
{
	WgSize parentSize = m_pParent->Size();
	WgBorders borders = m_pParent->_getPadding();
	return WgRect(parentSize.w-m_size.w-borders.right,borders.top,m_size);
}

WgCoord WgMenu::ScrollbarHook::ScreenPos() const
{
	WgRect content = m_pParent->ScreenGeo() - m_pParent->_getPadding();
	return WgCoord( content.x + content.w - m_size.w, content.y);
}

WgRect WgMenu::ScrollbarHook::ScreenGeo() const
{
	WgRect content = m_pParent->ScreenGeo() - m_pParent->_getPadding();
	return WgRect( content.x + content.w - m_size.w, content.y, m_size );
}

void WgMenu::ScrollbarHook::_requestRender()
{
	m_pParent->_requestRender( Geo() );
}

void WgMenu::ScrollbarHook::_requestRender( const WgRect& rect )
{
	m_pParent->_requestRender( rect + Pos() );
}

void WgMenu::ScrollbarHook::_requestResize()
{
	// Do nothing.
}

