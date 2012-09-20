
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

#include	<wg_gizmo_menu.h>
#include	<wg_gizmo_menulayer.h>
#include	<wg_gizmo_slider.h>
#include	<wg_surface.h>
#include	<wg_gfxdevice.h>
#include	<wg_char.h>
#include	<wg_font.h>
#include	<wg_texttool.h>
#include	<wg_pen.h>
#include	<wg_base.h>
#include	<wg_eventhandler.h>
#include	<wg_patches.h>

static const char	c_gizmoType[] = {"Menu"};
static const char	c_hookType[] = {"MenuSliderHook"};


//____ Constructor _____________________________________________________________

WgGizmoMenu::WgGizmoMenu()
{
	m_nItems				= 0;
	m_markedItem			= 0;
	m_pSelectedItem			= 0;

	m_pBgGfx				= 0;
	m_pSepGfx				= 0;
	m_pCbGfxChecked			= 0;
	m_pCbGfxUnchecked		= 0;
	m_pRbGfxSelected		= 0;
	m_pRbGfxUnselected		= 0;

	m_iconFieldWidth		= 0;
	m_arrowFieldWidth		= 0;

	m_pArrowAnim			= 0;
	m_arrowAnimCount		= 0;

	m_entryHeight			= 0;
	m_sepHeight				= 2;

	m_sliderBtnLayout		= WgGizmoSlider::DEFAULT;

	m_contentHeight			= 0;
	m_contentOfs			= 0;

	m_nSelectorKeys			= 0;
	m_selectorCountdown		= 0;

	m_sliderHook.m_pParent = this;
	m_pOpenSubMenu			= 0;

	_refreshEntryHeight();
}

//____ ~WgGizmoMenu() __________________________________________________________

WgGizmoMenu::~WgGizmoMenu()
{
}


//____ Type() _________________________________________________________________

const char * WgGizmoMenu::Type() const
{
	return GetClass();
}

//____ GetClass() _____________________________________________________________

const char * WgGizmoMenu::GetClass( void )
{
	return c_gizmoType;
}

//____ SetBgSource() __________________________________________________________

bool WgGizmoMenu::SetBgSource( const WgBlocksetPtr pBgGfx, Uint8 iconFieldWidth, Uint8 arrowFieldWidth )
{
	m_pBgGfx			= pBgGfx;

	m_iconFieldWidth	= iconFieldWidth;
	m_arrowFieldWidth	= arrowFieldWidth;

	_adjustSize();
	_requestRender();
	return true;

}

//____ SetSeparatorSource() ___________________________________________________

bool WgGizmoMenu::SetSeparatorSource( const WgBlocksetPtr pGfx, const WgBorders& borders )
{
	if( pGfx == WgBlocksetPtr(0) )
		return false;

	m_pSepGfx		= pGfx;
	m_sepBorders	= borders;
	m_sepHeight		= m_pSepGfx->Height() + m_sepBorders.Height();

	_adjustSize();
	_requestRender();
	return true;
}

//____ SetArrowSource() _______________________________________________________

bool WgGizmoMenu::SetArrowSource( WgGfxAnim * pAnim )
{
	if( pAnim == 0 )
		return false;

	m_pArrowAnim = pAnim;
	m_arrowAnimCount = 0;

	_requestRender();
	return true;
}

//____ SetTextProperties() _____________________________________________________

bool WgGizmoMenu::SetTextProperties( const WgTextpropPtr& pEntryProp, const WgTextpropPtr& pKeyAccelProp )
{
	m_pEntryProp 	= pEntryProp;
	m_pKeyAccelProp = pKeyAccelProp;

	// We need to modify MinWidth now that fonts might have changed

	WgMenuitem * pItem = m_items.First();
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

void WgGizmoMenu::_refreshEntryHeight()
{
		WgPen		pen;
		WgTextAttr	attr;
		WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextprop(), WG_MODE_NORMAL );
		WgTextTool::AddPropAttributes(attr, m_pEntryProp, WG_MODE_NORMAL );

		pen.SetAttributes( attr );
		int	heightNormal	= pen.GetLineSpacing();

		attr.Clear();
		WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextprop(), WG_MODE_MARKED );
		WgTextTool::AddPropAttributes(attr, m_pEntryProp, WG_MODE_MARKED );
		pen.SetAttributes( attr );
		int heightMarked	= pen.GetLineSpacing();

		attr.Clear();
		WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextprop(), WG_MODE_DISABLED );
		WgTextTool::AddPropAttributes(attr, m_pEntryProp, WG_MODE_DISABLED );
		pen.SetAttributes( attr );
		int heightDisabled	= pen.GetLineSpacing();

		if( m_entryHeight < heightNormal )
			m_entryHeight = heightNormal;

		if( m_entryHeight < heightMarked )
			m_entryHeight = heightMarked;

		if( m_entryHeight < heightDisabled )
			m_entryHeight = heightDisabled;

		//

		if( m_pTileBlocks[0] )
			m_entryHeight +=  m_pTileBlocks[0]->Padding().Height();

		if( m_entryHeight < m_minTileSize.h )
			m_entryHeight = m_minTileSize.h;
}


//____ SetSliderSource() ______________________________________________________

bool WgGizmoMenu::SetSliderSource(  WgBlocksetPtr pBgGfx, WgBlocksetPtr pBarGfx, WgBlocksetPtr pBtnBwdGfx, WgBlocksetPtr pBtnFwdGfx )
{
	m_pSliderBgGfx		= pBgGfx;
	m_pSliderBarGfx		= pBarGfx;
	m_pSliderBtnFwdGfx	= pBtnFwdGfx;
	m_pSliderBtnBwdGfx	= pBtnBwdGfx;

	if( m_sliderHook.Slider() )
	{
		//TODO: Adapt to changes in sliders minimum width.

		m_sliderHook.Slider()->SetSource( pBgGfx, pBarGfx, pBtnBwdGfx, pBtnFwdGfx );
	}
	return true;
}

//____ SetSliderButtonLayout() ________________________________________________

bool WgGizmoMenu::SetSliderButtonLayout(  WgGizmoSlider::ButtonLayout layout )
{
	m_sliderBtnLayout = layout;

	if( m_sliderHook.Slider() )
		m_sliderHook.Slider()->SetButtonLayout( layout );

	return true;
}


//____ SetCheckBoxSource() ____________________________________________________

bool WgGizmoMenu::SetCheckBoxSource( const WgBlocksetPtr pUnchecked, const WgBlocksetPtr pChecked )
{
	m_pCbGfxChecked		= pChecked;
	m_pCbGfxUnchecked	= pUnchecked;

	_requestRender();
	return true;
}

//____ SetRadioButtonSource() _________________________________________________

bool WgGizmoMenu::SetRadioButtonSource( const WgBlocksetPtr pUnselected, const WgBlocksetPtr pSelected )
{
	m_pRbGfxSelected			= pSelected;
	m_pRbGfxUnselected			= pUnselected;

	_requestRender();
	return true;
}

//____ GetEntryHeight() _______________________________________________________

int WgGizmoMenu::GetEntryHeight() const
{
	return m_entryHeight;
}

//____ AddItem() ______________________________________________________________

int WgGizmoMenu::AddItem( WgMenuitem * pItem )
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

int WgGizmoMenu::InsertItem( WgMenuitem * pEntry, int pos )
{
	// Calculate minWidth for all derived from ENTRY

	if( pEntry->GetType() != SEPARATOR )
		_calcEntryMinWidth( (WgMenuEntry*) pEntry );

	// Let item do what it needs to

	pEntry->SetMyMenu( this );

	// Add the item.

	WgMenuitem * pPos = m_items.Get(pos);
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

bool WgGizmoMenu::RemoveItem( WgMenuitem * pEntry )
{
	if( !m_items.IsMemberOf(pEntry) )
		return false;

	pEntry->Disconnect();
	m_nItems--;
	_adjustSize();
	_requestRender();
	return true;
}

WgMenuitem* WgGizmoMenu::RemoveItem( int pos )
{
	WgMenuitem * pEntry = GetItem(pos);
	RemoveItem(pEntry);
	return pEntry;
}

//____ RemoveAllItems() _______________________________________________________

void WgGizmoMenu::RemoveAllItems()
{
	WgMenuitem* pItem = m_items.First();
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

bool WgGizmoMenu::DeleteItem( WgMenuitem * pEntry )
{
	if( !m_items.IsMemberOf(pEntry) )
		return false;

	delete pEntry;
	m_nItems--;
	_adjustSize();
	_requestRender();
	return true;
}

bool WgGizmoMenu::DeleteItem( int pos )
{
	return DeleteItem(GetItem(pos));
}

//____ DeleteAllItems() _______________________________________________________

void WgGizmoMenu::DeleteAllItems()
{
	m_items.Clear();
	m_nItems = 0;
	_adjustSize();
	_requestRender();
}

//____ GetItemPos() ___________________________________________________________

int WgGizmoMenu::GetItemPos( WgMenuitem* pEntry )
{
	if( m_items.IsMemberOf( pEntry ) )
		return pEntry->Index();
	else
		return -1;
}

//____ GetItem() ______________________________________________________________

WgMenuitem* WgGizmoMenu::GetItem( int pos )
{
	return m_items.Get(pos);
}

//____ FindItem() ______________________________________________________________

WgMenuitem* WgGizmoMenu::FindItem(int id)
{
	WgMenuitem* pItem = m_items.First();
	for(; pItem; pItem = pItem->Next())
	{
		if(pItem->GetId() == id)
			return pItem;
	}
	return 0;
}

//____ DefaultSize() ___________________________________________________________

WgSize WgGizmoMenu::DefaultSize() const
{
	return m_defaultSize;
}

//____ WidthForHeight() ________________________________________________________

int  WgGizmoMenu::WidthForHeight( int height ) const
{
	if( height >= m_defaultSize.h )
		return m_defaultSize.w;

	int sliderWidth = 0;

	if( m_pSliderBgGfx && m_pSliderBgGfx->Width() > sliderWidth )
		sliderWidth = m_pSliderBgGfx->Width();

	if( m_pSliderBarGfx && m_pSliderBarGfx->Width() > sliderWidth )
		sliderWidth = m_pSliderBarGfx->Width();

	if( m_pSliderBtnFwdGfx && m_pSliderBtnFwdGfx->Width() > sliderWidth )
		sliderWidth = m_pSliderBtnFwdGfx->Width();

	if( m_pSliderBtnBwdGfx && m_pSliderBtnBwdGfx->Width() > sliderWidth )
		sliderWidth = m_pSliderBtnBwdGfx->Width();

	return m_defaultSize.w + sliderWidth;
}



//____ _calcEntryMinWidth() _____________________________________________________

void WgGizmoMenu::_calcEntryMinWidth( WgMenuEntry * pEntry )
{
	WgTextAttr	entryAttr;
	WgTextTool::AddPropAttributes(entryAttr, WgBase::GetDefaultTextprop(), WG_MODE_NORMAL );
	WgTextTool::AddPropAttributes(entryAttr, m_pEntryProp, WG_MODE_NORMAL);
	WgTextAttr	accelAttr;
	WgTextTool::AddPropAttributes(accelAttr, WgBase::GetDefaultTextprop(), WG_MODE_NORMAL );
	WgTextTool::AddPropAttributes(accelAttr, m_pKeyAccelProp, WG_MODE_NORMAL);

	int wNormal = WgTextTool::lineWidth( 0, entryAttr, "  " );
	int wMarked = WgTextTool::lineWidth( 0, entryAttr, "  " );

	wNormal += WgTextTool::lineWidth( 0, entryAttr, WG_MODE_NORMAL, pEntry->GetText().Chars() );
	wNormal += WgTextTool::lineWidth( 0, accelAttr, WG_MODE_NORMAL, pEntry->GetAccelText().Chars() );

	entryAttr.Clear();
	WgTextTool::AddPropAttributes(entryAttr, WgBase::GetDefaultTextprop(), WG_MODE_MARKED );
	WgTextTool::AddPropAttributes(entryAttr, m_pEntryProp, WG_MODE_MARKED);
	accelAttr.Clear();
	WgTextTool::AddPropAttributes(accelAttr, WgBase::GetDefaultTextprop(), WG_MODE_MARKED );
	WgTextTool::AddPropAttributes(accelAttr, m_pKeyAccelProp, WG_MODE_MARKED);

	wMarked += WgTextTool::lineWidth( 0, entryAttr, WG_MODE_NORMAL, pEntry->GetText().Chars() );
	wMarked += WgTextTool::lineWidth( 0, accelAttr, WG_MODE_NORMAL, pEntry->GetAccelText().Chars() );


	if( wNormal > wMarked )
		pEntry->m_minWidth = wNormal;
	else
		pEntry->m_minWidth = wMarked;
}

//____ _getPadding() ____________________________________________________

WgBorders WgGizmoMenu::_getPadding() const
{
	if( m_pBgGfx )
		return m_pBgGfx->Padding();
	else
		return WgBorders(0);
}

//____ _scrollItemIntoView() ___________________________________________________

void WgGizmoMenu::_scrollItemIntoView( WgMenuitem * pItem, bool bForceAtTop )
{
	WgMenuitem * p = m_items.First();
	unsigned int ofs = 0;

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

	_updateSlider( _getSliderPosition(), _getSliderSize() );
}

//____ _markFirstFilteredEntry() _______________________________________________

void WgGizmoMenu::_markFirstFilteredEntry()
{
	WgMenuitem * pItem = m_items.First();

	while( pItem )
	{
		if( pItem->GetType() != SEPARATOR )
		{
			WgString str = ((WgMenuEntry*)pItem)->GetText();

			if( str.Length() >= (unsigned) m_nSelectorKeys )
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

void WgGizmoMenu::_onRender( WgGfxDevice * pDevice, const WgRect& canvas, const WgRect& window, const WgRect& clip, Uint8 layer )
{

	// Render background

	if( m_pBgGfx )
	{
		if( m_bEnabled )
			pDevice->ClipBlitBlock( clip, m_pBgGfx->GetBlock(WG_MODE_NORMAL,window), window );
		else
			pDevice->ClipBlitBlock( clip, m_pBgGfx->GetBlock(WG_MODE_DISABLED,window), window );
	}


	// Render the menu-items

	WgBorders	contentBorders = _getPadding();
	WgRect		contentClip( canvas - contentBorders, clip );		// A clip rectangle for content.

	WgMenuitem * pItem = m_items.First();

	Uint32	yPos = window.y + contentBorders.top - m_contentOfs;
	Uint32	xPosText = window.x + contentBorders.left + m_iconFieldWidth;
	Uint32	xPosIcon = window.x + contentBorders.left;
	Uint32	textFieldLen = window.w - contentBorders.Width() - m_iconFieldWidth - m_arrowFieldWidth;

	WgPen	entryPen( pDevice, WgCoord( xPosText, yPos ), contentClip );
	WgPen	accelPen( pDevice, WgCoord( xPosText, yPos ), contentClip );


	unsigned int	item = 1;
	while( pItem )
	{
		if( pItem->IsSetToVisible() )
		{
			if( pItem->GetType() == SEPARATOR )
			{
				if( m_pSepGfx )
				{
					WgRect sepClip(clip, WgRect(canvas.x, canvas.y + contentBorders.top, canvas.w, canvas.h - contentBorders.Height() ) );

					WgRect	dest( window.x + m_sepBorders.left, yPos + m_sepBorders.top,
									window.w - m_sepBorders.Width(), m_pSepGfx->Height() );

					pDevice->ClipBlitBlock( sepClip, m_pSepGfx->GetBlock(WG_MODE_NORMAL,dest), dest );
					yPos += m_sepHeight;
				}
			}
			else
			{
				WgMode	mode = WG_MODE_DISABLED;

				if( ((WgMenuEntry*)pItem)->IsEnabled() )
					mode = WG_MODE_NORMAL;

				if( item == m_markedItem )
					mode = WG_MODE_MARKED;

				// Render the tile for this entry

				WgRect tileDest(	window.x + contentBorders.left,
									yPos,
									window.w - contentBorders.Width(),
									m_entryHeight );

				WgRect tileClip( contentClip, tileDest);

				_renderTile( pDevice, tileClip, tileDest, item-1, mode );

				// Print the text (if any)

				const WgChar * pText = ((WgMenuEntry*)pItem)->GetText().Chars();
				if( pText->Glyph() != 0 )
				{

					WgTextAttr	attr;
					WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextprop(), mode );
					if( m_pBgGfx )
						WgTextTool::SetAttrColor( attr, m_pBgGfx->TextColors(), mode );
					WgTextTool::AddPropAttributes( attr, m_pEntryProp, mode );
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
					WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextprop(), mode );
					if( m_pBgGfx )
						WgTextTool::SetAttrColor( attr, m_pBgGfx->TextColors(), mode );
					WgTextTool::AddPropAttributes( attr, m_pKeyAccelProp, mode );
					accelPen.SetAttributes( attr );

					int y = yPos + (m_entryHeight - accelPen.GetLineHeight())/2 + accelPen.GetBaseline();
					int width = WgTextTool::lineWidth( 0, attr, mode, pAccelText );
					int x = xPosText + textFieldLen - width;

					accelPen.SetPos( WgCoord(x, y) );
					pDevice->PrintLine( accelPen, attr, pAccelText );
				}

				// Show the icon/checkbox/radiobutton (if any)

				switch( pItem->GetType() )
				{
					case ENTRY:
					{
						WgBlocksetPtr pIcon = ((WgMenuEntry*)pItem)->GetIcon();
						if( pIcon )
						{
							int w = pIcon->Width();
							int h = pIcon->Height();

							//

							if( w > m_iconFieldWidth )
								w = m_iconFieldWidth;
							if( h > m_entryHeight )
								h = m_entryHeight;

							int y = yPos + (m_entryHeight - h)/2;
							int x = xPosIcon + (m_iconFieldWidth - w)/2;

							pDevice->ClipBlitBlock( contentClip, pIcon->GetBlock(mode), WgRect(x,y,w,h) );
						}
					}
					break;

					case CHECKBOX:
					{
						WgBlocksetPtr	pGfx;

						if( ((WgMenuCheckBox*)pItem)->IsChecked() )
							pGfx = m_pCbGfxChecked;
						else
							pGfx = m_pCbGfxUnchecked;

						if( pGfx )
						{
							int w = pGfx->Width();
							int h = pGfx->Height();

							int y = yPos + (m_entryHeight - h)/2;
							int x = xPosIcon + (m_iconFieldWidth - w)/2;

							pDevice->ClipBlitBlock( contentClip, pGfx->GetBlock(mode), WgRect(x,y,w,h) );
						}
					}
					break;
					case RADIOBUTTON:
					{
						WgBlocksetPtr	pGfx;

						if( ((WgMenuRadioButton*)pItem)->IsSelected() )
							pGfx = m_pRbGfxSelected;
						else
							pGfx = m_pRbGfxUnselected;

						if( pGfx )
						{
							int w = pGfx->Width();
							int h = pGfx->Height();

							int y = yPos + (m_entryHeight - h)/2;
							int x = xPosIcon + (m_iconFieldWidth - w)/2;

							pDevice->ClipBlitBlock( contentClip, pGfx->GetBlock(mode), WgRect(x,y,w,h) );
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

void WgGizmoMenu::_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler )
{
	WgCoord mousePos = pEvent->PointerPos();

	switch( pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
			if( m_selectorCountdown > 0 )
			{
				const WgEvent::Tick * pTick = static_cast<const WgEvent::Tick*>(pEvent);

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

			WgMenuitem * pOldItem = m_items.Get(m_markedItem-1);
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
			WgMenuitem * pItem = _getItemAtPos( mousePos.x, mousePos.y );

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

		case WG_EVENT_MOUSEBUTTON_RELEASE:
		{
			WgMenuitem * pItem = _getItemAtPos( mousePos.x, mousePos.y );
			if( pItem )
				SelectItem(pItem);
		}
		break;

		case WG_EVENT_MOUSEWHEEL_ROLL:
		{
			const WgEvent::MouseWheelRoll * pEv = static_cast<const WgEvent::MouseWheelRoll*>(pEvent);

			if( pEv->Wheel() == 1 )
			{
				int distance = pEv->Distance();

				_setViewOfs( m_contentOfs - m_entryHeight*distance );
				_updateSlider( _getSliderPosition(), _getSliderSize() );
			}
		}

		case WG_EVENT_CHARACTER:
		{
			Uint16 chr = static_cast<const WgEvent::Character*>(pEvent)->Char();
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
			WgMenuitem * pItem = 0;
			if( m_markedItem != 0 )
				pItem = m_items.Get( m_markedItem-1 );

			int key = static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode();
			switch( key )
			{
				case WG_KEY_ESCAPE:
					if( m_pOpenSubMenu )
						_closeSubMenu( m_pOpenSubMenu );
					else
						pHandler->ForwardEvent( pEvent );								
					break;
					
				case WG_KEY_LEFT:
					if( m_pOpenSubMenu )
						_closeSubMenu( m_pOpenSubMenu );
					else
						pHandler->ForwardEvent( pEvent );								
					break;
				
				case WG_KEY_RIGHT:
					if( pItem )
					{
						if( pItem->GetType() == SUBMENU )
							_openSubMenu( (WgMenuSubMenu*) pItem );
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
						while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsSetToVisible() ) )
							pItem = pItem->Prev();
					}
					break;

				case WG_KEY_DOWN:
					if( pItem )
						pItem = pItem->Next();
					else
						pItem = m_items.First();

					while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsSetToVisible() ) )
							pItem = pItem->Next();
					break;

				case WG_KEY_HOME:
					pItem = m_items.First();
					while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsSetToVisible() ) )
						pItem = pItem->Next();
					break;

				case WG_KEY_END:
					pItem = m_items.Last();
					while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsSetToVisible() ))
						pItem = pItem->Prev();
					break;

				case WG_KEY_PAGE_UP:
				{
					int viewHeight = _getViewSize();

					int distance = m_entryHeight;
					while( pItem != 0 && distance < viewHeight )
					{
						if( pItem->IsSetToVisible() )
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
						while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsSetToVisible() ))
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
						if( pItem->IsSetToVisible() )
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
						while( pItem != 0 && (pItem->GetType() == SEPARATOR || !pItem->IsSetToVisible() ))
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

	if( pEvent->IsMouseButtonEvent() )
	{
		if( static_cast<const WgEvent::MouseButtonEvent*>(pEvent)->Button() != 1 )
			pHandler->ForwardEvent( pEvent );
	}
	else if( pEvent->IsKeyEvent() )
	{
		int key = static_cast<const WgEvent::KeyEvent*>(pEvent)->TranslatedKeyCode();
		if( key != WG_KEY_RIGHT && key != WG_KEY_RETURN && key != WG_KEY_UP && key != WG_KEY_DOWN &&
			key != WG_KEY_HOME && key != WG_KEY_END && key != WG_KEY_PAGE_UP && key != WG_KEY_PAGE_DOWN &&
			key != WG_KEY_ESCAPE && key != WG_KEY_LEFT )
			pHandler->ForwardEvent( pEvent );		
	}
	else if( pEvent->Type() != WG_EVENT_CHARACTER && pEvent->Type() != WG_EVENT_MOUSEWHEEL_ROLL )
		pHandler->ForwardEvent( pEvent );
}

//____ SelectItem() _________________________________________________
void WgGizmoMenu::SelectItem(WgMenuitem* pItem)
{
	m_pSelectedItem = 0;

	if(pItem == 0 || !pItem->IsSetToVisible() )
		return;

	switch( pItem->GetType() )
	{
		case ENTRY:
			m_pSelectedItem = pItem;
			_queueEvent( new WgEvent::MenuitemSelect(this,pItem->GetId()));
			_itemSelected();
		break;
		case CHECKBOX:
		{
			WgMenuCheckBox * pCheckBox = (WgMenuCheckBox*) pItem;

			if( pCheckBox->IsChecked() )
			{
				pCheckBox->Uncheck();
				_queueEvent( new WgEvent::MenuitemUncheck(this,pItem->GetId()));
			}
			else
			{
				pCheckBox->Check();
				_queueEvent( new WgEvent::MenuitemCheck(this,pItem->GetId()));
			}

			_itemSelected();
		}
		break;
		case RADIOBUTTON:
			m_pSelectedItem = pItem;
			((WgMenuRadioButton*)pItem)->Select();
			_queueEvent( new WgEvent::MenuitemSelect(this,pItem->GetId()));
			_itemSelected();
		break;

		default:
            break;
	}
}

//____ FindGizmo() _____________________________________________________________

WgGizmo * WgGizmoMenu::FindGizmo( const WgCoord& ofs, WgSearchMode mode )
{
	WgGizmo * pGizmo = WgGizmoContainer::FindGizmo(ofs, mode);
	if( !pGizmo && MarkTest( ofs ) )
		return this;

	return pGizmo;
}

//____ _openSubMenu() __________________________________________________________

void WgGizmoMenu::_openSubMenu( WgMenuSubMenu * pItem )
{
	WgGizmo * pMenu = pItem->GetSubMenu();

	if( !pMenu )
		return;

	// Figure out Y-offset for pItem

	Uint32 yOfs = 0;

	WgMenuitem * p = m_items.First();
	while( p != pItem )
	{
		if( p->IsSetToVisible() )
		{
			if( p->GetType() == SEPARATOR )
				yOfs += m_sepHeight;
			else
				yOfs += m_entryHeight;
		}
		p = p->Next();
	}

	// Calculate itemArea

	WgRect	geo = ScreenGeo() - _getPadding();
	WgRect itemArea( geo.x, geo.y + yOfs, geo.w, m_entryHeight );

	// 

	WgGizmoMenulayer * pLayer = 0;

	if( Parent() )
		pLayer = Parent()->_getMenuLayer();

	if( pLayer )
	{
		pLayer->OpenMenu( pMenu, this, itemArea - pLayer->ScreenPos(), WG_NORTHEAST );
		m_pOpenSubMenu = pItem;
	}
}


//____ _closeSubMenu() _________________________________________________________

void WgGizmoMenu::_closeSubMenu( WgMenuSubMenu * pItem )
{
	WgGizmoMenulayer * pLayer = 0;
	WgGizmo * pMenu = pItem->GetSubMenu();

	if( Parent() )
		pLayer = Parent()->_getMenuLayer();

	if( pLayer && pMenu )
	{	
		pLayer->CloseMenu( pMenu );
		m_pOpenSubMenu = 0;
	}
}


//____ _itemSelected() ______________________________________________________

void WgGizmoMenu::_itemSelected()
{
	WgGizmoMenulayer * pLayer = 0;

	if( Parent() )
		pLayer = Parent()->_getMenuLayer();

	if( pLayer )
		pLayer->CloseAllMenus();
}

//____ _renderPatches() ________________________________________________________

void WgGizmoMenu::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer )
{
	WgRect sliderGeo = _sliderGeo( _canvas );

	for( const WgRect * pRect = _pPatches->Begin() ; pRect != _pPatches->End() ; pRect++ )
	{
		// Render menu itself

		WgRect clip( _canvas, *pRect );
		if( clip.w > 0 || clip.h > 0 )
			_onRender( pDevice, _canvas, _window, clip, _layer );

		// Render slider if present.

		if( m_sliderHook.Gizmo() )
		{
			WgRect clip( sliderGeo, *pRect );
			if( clip.w > 0 || clip.h > 0 )
				((WgGizmoSlider*)m_sliderHook.Gizmo())->_onRender( pDevice, sliderGeo, sliderGeo, clip, _layer );
		}
	}
}

//____ _onCollectPatches() _____________________________________________________

void WgGizmoMenu::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
	container.Add( WgRect( geo, clip ) );
}

//____ _onMaskPatches() ________________________________________________________

void WgGizmoMenu::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE )
	{
		patches.Sub( WgRect( geo, clip ) );
	}
	else if( blendMode == WG_BLENDMODE_BLEND && m_pBgGfx )
	{
		if( m_pBgGfx->IsOpaque() )
			patches.Sub( WgRect( geo, clip ) );
		else if( m_pBgGfx->HasOpaqueCenter() )
			patches.Sub( WgRect( geo - m_pBgGfx->Frame(), clip ) );
	}
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoMenu::_onCloneContent( const WgGizmo * _pOrg )
{
	const WgGizmoMenu * pOrg = static_cast<const WgGizmoMenu*>(_pOrg);

	m_pBgGfx 			= pOrg->m_pBgGfx;
	m_iconFieldWidth 	= pOrg->m_iconFieldWidth;
	m_arrowFieldWidth 	= pOrg->m_arrowFieldWidth;
	m_pSepGfx			= pOrg->m_pSepGfx;
	m_sepBorders		= pOrg->m_sepBorders;
	m_pArrowAnim		= pOrg->m_pArrowAnim;
	m_entryHeight		= pOrg->m_entryHeight;
	m_sepHeight			= pOrg->m_sepHeight;
	m_pEntryProp		= pOrg->m_pEntryProp;
	m_pKeyAccelProp		= pOrg->m_pKeyAccelProp;
	m_pCbGfxChecked		= pOrg->m_pCbGfxChecked;
	m_pCbGfxUnchecked	= pOrg->m_pCbGfxUnchecked;
	m_pRbGfxSelected	= pOrg->m_pRbGfxSelected;
	m_pRbGfxUnselected	= pOrg->m_pRbGfxUnselected;
	m_pSliderBgGfx		= pOrg->m_pSliderBgGfx;
	m_pSliderBarGfx		= pOrg->m_pSliderBarGfx;
	m_pSliderBtnFwdGfx	= pOrg->m_pSliderBtnFwdGfx;
	m_pSliderBtnBwdGfx	= pOrg->m_pSliderBtnBwdGfx;
	m_sliderBtnLayout	= pOrg->m_sliderBtnLayout;

	WgTileHolder::_cloneContent( pOrg );

	//TODO: Implement cloning of menu items!
}

//____ _onRefresh() ____________________________________________________________
void WgGizmoMenu::_onRefresh()
{
	//TODO: Implement!
}


//____ _onNewSize() ____________________________________________________________

void WgGizmoMenu::_onNewSize( const WgSize& size )
{
		_adjustSize();
}

//____ _onAlphaTest() ___________________________________________________

bool WgGizmoMenu::_onAlphaTest( const WgCoord& ofs )
{
	return true;
}

//____ _onEnable() _____________________________________________________________

void WgGizmoMenu::_onEnable()
{
	if( m_sliderHook.Gizmo() )
		m_sliderHook.Gizmo()->Enable();

	_requestRender();
}

//____ _onDisable() ____________________________________________________________

void WgGizmoMenu::_onDisable()
{
	if( m_sliderHook.Gizmo() )
		m_sliderHook.Gizmo()->Disable();

	_requestRender();
}


//____ _firstHook() ____________________________________________________________

WgHook * WgGizmoMenu::_firstHook() const
{
	if( m_sliderHook.Gizmo() )
		return const_cast<SliderHook*>(&m_sliderHook);
	else
		return 0;
}

//____ _sliderGeo() ___________________________________________________________

WgRect WgGizmoMenu::_sliderGeo( const WgRect& menuGeo ) const
{
	if( m_sliderHook.Gizmo() )
	{
		WgRect contentGeo = menuGeo - _getPadding();
		WgRect sliderGeo( contentGeo.x + contentGeo.w - m_sliderHook.m_size.w, contentGeo.y, m_sliderHook.m_size.w, contentGeo.h );	//TODO: Slider is now hardcoded to right side.
		return sliderGeo;
	}
	else
		return WgRect();
}


//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgGizmoMenu::_firstHookWithGeo( WgRect& writeGeo ) const
{
	if( m_sliderHook.Gizmo() )
	{
		writeGeo = _sliderGeo( Size() );
		return const_cast<SliderHook*>(&m_sliderHook);
	}
	else
		return 0;
}

//____ _getItemAtPos() ______________________________________________________

WgMenuitem * WgGizmoMenu::_getItemAtPos( int x, int y )
{
	WgBorders	contentBorders = _getPadding();

	x -= contentBorders.left;
	y -= contentBorders.top;

	y += m_contentOfs;

	if( y > 0 && x > 0 && x < (int) ( Geo().w - contentBorders.Width() ) )
	{
		WgMenuitem * pItem = m_items.First();
		while( pItem )
		{
			if( pItem->IsSetToVisible() )
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

void WgGizmoMenu::_itemModified()
{
	_adjustSize();
	_requestRender();
}

//____ _adjustSize() ___________________________________________________________

void WgGizmoMenu::_adjustSize()
{
	WgBorders contentBorders = _getPadding();

	int  w = contentBorders.Width();
	int	 h = contentBorders.Height();

	int minSep = m_sepBorders.Width();
	if( m_pSepGfx )
		minSep += m_pSepGfx->MinWidth();

	WgMenuitem * pItem = m_items.First();
	while( pItem )
	{
		if( pItem->IsSetToVisible() )
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
		WgGizmoSlider * pSlider = m_sliderHook.Slider();
		if( !pSlider )
		{
			pSlider = new WgGizmoVSlider();
			pSlider->SetSource( m_pSliderBgGfx, m_pSliderBarGfx, m_pSliderBtnBwdGfx, m_pSliderBtnFwdGfx );
			pSlider->SetButtonLayout( m_sliderBtnLayout );
			pSlider->SetSliderTarget(this);
		}
		WgSize sliderSize = pSlider->DefaultSize();

		m_sliderHook.m_size.w = sliderSize.w;
		m_sliderHook.m_size.h = Size().h - contentBorders.Height();

		m_sliderHook._attachGizmo(pSlider);

		_updateSlider( _getSliderPosition(), _getSliderSize() );
	}
	else
	{
		if( m_sliderHook.Gizmo() )
		{
			delete m_sliderHook._releaseGizmo();
		}
	}

	_setViewOfs(m_contentOfs);		// Refresh offset.
}


//____ _stepFwd() ______________________________________________________________

float WgGizmoMenu::_stepFwd()
{
	_setViewOfs( m_contentOfs + m_entryHeight );
	return _getSliderPosition();
}

//____ _stepBwd() ______________________________________________________________

float WgGizmoMenu::_stepBwd()
{
	_setViewOfs( m_contentOfs - m_entryHeight );
	return _getSliderPosition();
}

//____ _jumpFwd() ______________________________________________________________

float WgGizmoMenu::_jumpFwd()
{
	int viewHeight = Size().h - _getPadding().Height();
	_setViewOfs( m_contentOfs + (viewHeight - m_entryHeight) );
	return _getSliderPosition();
}

//____ _jumpBwd() ______________________________________________________________

float WgGizmoMenu::_jumpBwd()
{
	int viewHeight = Size().h - _getPadding().Height();
	_setViewOfs( m_contentOfs - (viewHeight - m_entryHeight) );
	return _getSliderPosition();
}

//____ _wheelRolled() ______________________________________________________________

float WgGizmoMenu::_wheelRolled( int distance )
{
	int viewHeight = Size().h - _getPadding().Height();
	_setViewOfs( m_contentOfs + m_entryHeight*distance );
	return _getSliderPosition();
}


//____ _setPosition() __________________________________________________________

float WgGizmoMenu::_setPosition( float fraction )
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


//____ _getGizmo() _____________________________________________________________

WgGizmo* WgGizmoMenu::_getGizmo()
{
	return this;
}

//____ _getSliderPosition() ____________________________________________________

float WgGizmoMenu::_getSliderPosition()
{
	return ((float)m_contentOfs) / (m_contentHeight-(Size().h-_getPadding().Height()));
}

//____ _getSliderSize() ________________________________________________________

float WgGizmoMenu::_getSliderSize()
{
	return ((float)(_getViewSize())) / m_contentHeight;
}

//____ _getViewSize() ____________________________________________________

int WgGizmoMenu::_getViewSize()
{
	return Size().h-_getPadding().Height();
}

//____ _setViewOfs() ______________________________________________________________

void WgGizmoMenu::_setViewOfs(int pos)
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


//____ _tilesModified() _______________________________________________________

void WgGizmoMenu::_tilesModified()
{
	_refreshEntryHeight();
	_adjustSize();
	_requestRender();
}




const char * WgGizmoMenu::SliderHook::Type( void ) const
{
	return ClassType();
}

const char * WgGizmoMenu::SliderHook::ClassType()
{
	return c_hookType;
}

WgCoord WgGizmoMenu::SliderHook::Pos() const
{
	WgSize parentSize = m_pParent->Size();
	WgBorders borders = m_pParent->_getPadding();
	return WgCoord(parentSize.w-m_size.w-borders.right,borders.top);
}

WgSize WgGizmoMenu::SliderHook::Size() const
{
	return m_size;
}

WgRect WgGizmoMenu::SliderHook::Geo() const
{
	WgSize parentSize = m_pParent->Size();
	WgBorders borders = m_pParent->_getPadding();
	return WgRect(parentSize.w-m_size.w-borders.right,borders.top,m_size);
}

WgCoord WgGizmoMenu::SliderHook::ScreenPos() const
{
	WgRect content = m_pParent->ScreenGeo() - m_pParent->_getPadding();
	return WgCoord( content.x + content.w - m_size.w, content.y);
}

WgRect WgGizmoMenu::SliderHook::ScreenGeo() const
{
	WgRect content = m_pParent->ScreenGeo() - m_pParent->_getPadding();
	return WgRect( content.x + content.w - m_size.w, content.y, m_size );
}

void WgGizmoMenu::SliderHook::_requestRender()
{
	m_pParent->_requestRender( Geo() );
}

void WgGizmoMenu::SliderHook::_requestRender( const WgRect& rect )
{
	m_pParent->_requestRender( rect + Pos() );
}

void WgGizmoMenu::SliderHook::_requestResize()
{
	// Do nothing.
}

