
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

#include	<wdg_menu.h>
#include	<wdg_root.h>
#include	<wdg_dragbars.h>
#include	<wg_surface.h>
#include	<wg_gfx.h>
#include	<wg_char.h>
#include	<wg_font.h>
#include	<wg_texttool.h>
#include	<wg_pen.h>
#include	<wg_base.h>

using namespace WgSignal;

static const char	Wdg_Type[] = {"TordJ/Menu"};

//____ NewOfMyType() __________________________________________________________

WgWidget * Wdg_Menu::NewOfMyType() const
{
	return new Wdg_Menu;
}


//____ Init() _________________________________________________________________

void Wdg_Menu::Init( void )
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

	m_entryHeight			= 4;
	m_sepHeight				= 2;

	m_pMenuRoot				= 0;
	m_pParentMenu			= 0;
	m_menuPosX				= 0;
	m_menuPosY				= 0;
	m_menuMinW				= 0;
	m_btnReleaseArea		= WgRect(0, 0, 0, 0);

	m_sliderBtnLayout		= WgGizmoDragbar::DEFAULT;
	m_pSlider				= 0;

	m_contentHeight			= 0;
	m_contentOfs			= 0;
	m_bPressOnSlider		= false;

	m_savedMousePos.x		= -1;
	m_savedMousePos.y		= -1;

	m_nSelectorKeys			= 0;
	m_selectorCountdown		= 0;

	m_bIsOpeningSubMenu = false;

	WgWidget::Modal();				// A menu is always modal...
}

//____ ~Wdg_Menu() ____________________________________________________________

Wdg_Menu::~Wdg_Menu( void )
{
	if( m_pSlider )
		delete m_pSlider;
}


//____ Type() _________________________________________________________________

const char * Wdg_Menu::Type( void ) const
{
	return GetMyType();
}

const char * Wdg_Menu::GetMyType( void )
{
	return Wdg_Type;
}

//____ SetBgSource() __________________________________________________________

bool Wdg_Menu::SetBgSource( const WgBlockSetPtr pBgGfx, Uint8 iconFieldWidth, Uint8 arrowFieldWidth )
{
	m_pBgGfx			= pBgGfx;

	m_iconFieldWidth	= iconFieldWidth;
	m_arrowFieldWidth	= arrowFieldWidth;

	AdjustSize();
	RequestRender();
	return true;

}

//____ SetSeparatorSource() ___________________________________________________

bool Wdg_Menu::SetSeparatorSource( const WgBlockSetPtr pGfx, const WgBorders& borders )
{
	if( pGfx == WgBlockSetPtr(0) )
		return false;

	m_pSepGfx		= pGfx;
	m_sepBorders	= borders;
	m_sepHeight		= m_pSepGfx->Height() + m_sepBorders.Height();

	AdjustSize();
	RequestRender();
	return true;
}

//____ SetArrowSource() _______________________________________________________

bool Wdg_Menu::SetArrowSource( WgGfxAnim * pAnim )
{
	if( pAnim == 0 )
		return false;

	m_pArrowAnim = pAnim;
	m_arrowAnimCount = 0;

	RequestRender();
	return true;
}

//____ SetTextProperties() _____________________________________________________

bool Wdg_Menu::SetTextProperties( const WgTextPropPtr& pEntryProp, const WgTextPropPtr& pKeyAccelProp )
{
	m_pEntryProp 	= pEntryProp;
	m_pKeyAccelProp = pKeyAccelProp;

	// Make sure that m_entryHeight is high enough since fonts might have changed...

	if( m_pEntryProp )
	{
		WgPen		pen;
		WgTextAttr	attr;
		WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextProp(), WG_MODE_NORMAL );
		WgTextTool::AddPropAttributes(attr, m_pEntryProp, WG_MODE_NORMAL );

		pen.SetAttributes( attr );
		int	heightNormal	= pen.GetLineSpacing();

		attr.Clear();
		WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextProp(), WG_MODE_MARKED );
		WgTextTool::AddPropAttributes(attr, m_pEntryProp, WG_MODE_MARKED );
		pen.SetAttributes( attr );
		int heightMarked	= pen.GetLineSpacing();

		attr.Clear();
		WgTextTool::AddPropAttributes(attr, WgBase::GetDefaultTextProp(), WG_MODE_DISABLED );
		WgTextTool::AddPropAttributes(attr, m_pEntryProp, WG_MODE_DISABLED );
		pen.SetAttributes( attr );
		int heightDisabled	= pen.GetLineSpacing();

		if( m_entryHeight < heightNormal )
			m_entryHeight = heightNormal;

		if( m_entryHeight < heightMarked )
			m_entryHeight = heightMarked;

		if( m_entryHeight < heightDisabled )
			m_entryHeight = heightDisabled;
	}

	// We need to modify MinWidth now that fonts might have changed

	WgMenuItem * pItem = m_items.First();
	while( pItem )
	{
		if( pItem->GetType() != SEPARATOR )
			CalcEntryMinWidth( (WgMenuEntry*) pItem );
		pItem = pItem->Next();
	}

	// Refresh everything affected...

	AdjustSize();

	RequestRender();
	return true;
}


//____ SetSliderSource() ______________________________________________________

bool Wdg_Menu::SetSliderSource(  WgBlockSetPtr pBgGfx, WgBlockSetPtr pBarGfx, WgBlockSetPtr pBtnBwdGfx, WgBlockSetPtr pBtnFwdGfx )
{
	m_pSliderBgGfx		= pBgGfx;
	m_pSliderBarGfx		= pBarGfx;
	m_pSliderBtnFwdGfx	= pBtnFwdGfx;
	m_pSliderBtnBwdGfx	= pBtnBwdGfx;

	if( m_pSlider )
	{
		//TODO: Adapt to changes in sliders minimum width.

		m_pSlider->SetSource( pBgGfx, pBarGfx, pBtnBwdGfx, pBtnFwdGfx );
	}
	return true;
}

//____ SetSliderButtonLayout() ________________________________________________

bool Wdg_Menu::SetSliderButtonLayout(  WgGizmoDragbar::ButtonLayout layout )
{
	m_sliderBtnLayout = layout;

	if( m_pSlider )
		m_pSlider->SetButtonLayout( layout );

	return true;
}


//____ SetCheckBoxSource() ____________________________________________________

bool Wdg_Menu::SetCheckBoxSource( const WgBlockSetPtr pUnchecked, const WgBlockSetPtr pChecked )
{
	m_pCbGfxChecked		= pChecked;
	m_pCbGfxUnchecked	= pUnchecked;

	RequestRender();
	return true;
}

//____ SetRadioButtonSource() _________________________________________________

bool Wdg_Menu::SetRadioButtonSource( const WgBlockSetPtr pUnselected, const WgBlockSetPtr pSelected )
{
	m_pRbGfxSelected			= pSelected;
	m_pRbGfxUnselected			= pUnselected;

	RequestRender();
	return true;
}

//____ SetOpenParams() _______________________________________________________
void Wdg_Menu::SetOpenParams(Wdg_Root * pRoot, Uint32 x, Uint32 y, Uint32 minW, const WgRect *releaseArea, Wdg_Menu * pParentMenu )
{
	m_pMenuRoot = pRoot;
	m_pParentMenu = pParentMenu;
	m_menuPosX = x;
	m_menuPosY = y;
	m_menuMinW = minW;
	if(releaseArea)
		m_btnReleaseArea = *releaseArea;
	else
		m_btnReleaseArea = WgRect(0, 0, 0, 0);
}

//____ GetEntryHeight() _______________________________________________________

int Wdg_Menu::GetEntryHeight() const
{
	return m_entryHeight;
}

//____ SetEntryHeight() _______________________________________________________

bool Wdg_Menu::SetEntryHeight( Uint8 height )
{
	if( height < 4 )
		return false;

	m_entryHeight = height;

	AdjustSize();
	RequestRender();
	return true;
}

//____ AddItem() ______________________________________________________________

int Wdg_Menu::AddItem( WgMenuItem * pItem )
{
	// Calculate minWidth for all derived from ENTRY

	if( pItem->GetType() != SEPARATOR )
		CalcEntryMinWidth( (WgMenuEntry*) pItem );

	// Let item do what it needs to

	pItem->SetMyMenu( this );

	// Add the item.

	m_items.PushBack(pItem);
	m_nItems++;

	// Refresh what needs to be refreshed...

	AdjustSize();
	RequestRender();

	return m_nItems-1;		// Position of this item.
}

//____ InsertItem() ___________________________________________________________

int Wdg_Menu::InsertItem( WgMenuItem * pEntry, int pos )
{
	// Calculate minWidth for all derived from ENTRY

	if( pEntry->GetType() != SEPARATOR )
		CalcEntryMinWidth( (WgMenuEntry*) pEntry );

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

	AdjustSize();
	RequestRender();

	return pos;		// Position of this item.

}

//____ RemoveItem() ___________________________________________________________

bool Wdg_Menu::RemoveItem( WgMenuItem * pEntry )
{
	if( !m_items.IsMemberOf(pEntry) )
		return false;

	pEntry->Disconnect();
	m_nItems--;
	AdjustSize();
	RequestRender();
	return true;
}

WgMenuItem* Wdg_Menu::RemoveItem( int pos )
{
	WgMenuItem * pEntry = GetItem(pos);
	RemoveItem(pEntry);
	return pEntry;
}

//____ RemoveAllItems() _______________________________________________________

void Wdg_Menu::RemoveAllItems()
{
	Close();

	WgMenuItem* pItem = m_items.First();
	while( pItem )
	{
		pItem->SetMyMenu(0);
		pItem->Disconnect();
		pItem = m_items.First();
	}

	m_nItems = 0;
	AdjustSize();
}

//____ DeleteItem() ___________________________________________________________

bool Wdg_Menu::DeleteItem( WgMenuItem * pEntry )
{
	if( !m_items.IsMemberOf(pEntry) )
		return false;

	delete pEntry;
	m_nItems--;
	AdjustSize();
	RequestRender();
	return true;
}

bool Wdg_Menu::DeleteItem( int pos )
{
	return DeleteItem(GetItem(pos));
}

//____ DeleteAllItems() _______________________________________________________

void Wdg_Menu::DeleteAllItems()
{
	Close();
	m_items.Clear();
	m_nItems = 0;
	AdjustSize();
}

//____ GetItemPos() ___________________________________________________________

int Wdg_Menu::GetItemPos( WgMenuItem* pEntry )
{
	if( m_items.IsMemberOf( pEntry ) )
		return pEntry->Index();
	else
		return -1;
}

//____ GetItem() ______________________________________________________________

WgMenuItem* Wdg_Menu::GetItem( int pos )
{
	return m_items.Get(pos);
}

//____ FindItem() _____________________________________________________

WgMenuItem* Wdg_Menu::FindItem(int id)
{
	WgMenuItem* pItem = m_items.First();
	for(; pItem; pItem = pItem->Next())
	{
		if(pItem->GetId() == id)
			return pItem;
	}
	return 0;
}

//____ MoveOutsideModal() _____________________________________________________

void Wdg_Menu::MoveOutsideModal( int x, int y )
{
	WgMenuItem * pItem = GetItemAtAbsPos(x,y);

	if( pItem )
	{
		Uint32		itemNb = pItem->Index()+1;
		if( itemNb != m_markedItem )
			CloseSubMenu(m_markedItem);
	}
	else
		Emit( WgSignal::PointerOutsideModalPos(), x, y );
}


//____ CalcEntryMinWidth() _____________________________________________________

void Wdg_Menu::CalcEntryMinWidth( WgMenuEntry * pEntry )
{
	WgTextAttr	entryAttr;
	WgTextTool::AddPropAttributes(entryAttr, WgBase::GetDefaultTextProp(), WG_MODE_NORMAL );
	WgTextTool::AddPropAttributes(entryAttr, m_pEntryProp, WG_MODE_NORMAL);
	WgTextAttr	accelAttr;
	WgTextTool::AddPropAttributes(accelAttr, WgBase::GetDefaultTextProp(), WG_MODE_NORMAL );
	WgTextTool::AddPropAttributes(accelAttr, m_pKeyAccelProp, WG_MODE_NORMAL);

	int wNormal = WgTextTool::lineWidth( 0, entryAttr, "  " );
	int wMarked = WgTextTool::lineWidth( 0, entryAttr, "  " );

	wNormal += WgTextTool::lineWidth( 0, entryAttr, WG_MODE_NORMAL, pEntry->GetText().Chars() );
	wNormal += WgTextTool::lineWidth( 0, accelAttr, WG_MODE_NORMAL, pEntry->GetAccelText().Chars() );

	entryAttr.Clear();
	WgTextTool::AddPropAttributes(entryAttr, WgBase::GetDefaultTextProp(), WG_MODE_MARKED );
	WgTextTool::AddPropAttributes(entryAttr, m_pEntryProp, WG_MODE_MARKED);
	accelAttr.Clear();
	WgTextTool::AddPropAttributes(accelAttr, WgBase::GetDefaultTextProp(), WG_MODE_MARKED );
	WgTextTool::AddPropAttributes(accelAttr, m_pKeyAccelProp, WG_MODE_MARKED);

	wMarked += WgTextTool::lineWidth( 0, entryAttr, WG_MODE_NORMAL, pEntry->GetText().Chars() );
	wMarked += WgTextTool::lineWidth( 0, accelAttr, WG_MODE_NORMAL, pEntry->GetAccelText().Chars() );


	if( wNormal > wMarked )
		pEntry->m_minWidth = wNormal;
	else
		pEntry->m_minWidth = wMarked;
}

//____ GetContentBorders() ____________________________________________________

WgBorders Wdg_Menu::GetContentBorders() const
{
	if( m_pBgGfx )
		return m_pBgGfx->ContentBorders();
	else
		return WgBorders(0);
}

//____ ScrollItemIntoView() ___________________________________________________

void Wdg_Menu::ScrollItemIntoView( WgMenuItem * pItem, bool bForceAtTop )
{
	WgMenuItem * p = m_items.First();
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
		SetViewPixels(ofs);
	else if( ofs + m_entryHeight > m_contentOfs + GetViewSizePixels() )
		SetViewPixels( ofs + m_entryHeight - GetViewSizePixels() );
}

//____ MarkFirstFilteredEntry() _______________________________________________

void Wdg_Menu::MarkFirstFilteredEntry()
{
	WgMenuItem * pItem = m_items.First();

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
		RequestRender();
		ScrollItemIntoView( pItem );
	}

}


//____ DoMyOwnRender() ________________________________________________________

void Wdg_Menu::DoMyOwnRender( const WgRect& window, const WgRect& clip, Uint8 _layer )
{

	// Render background

	if( m_pBgGfx )
	{
		if( m_bEnabled )
			WgGfx::clipBlitBlock( clip, m_pBgGfx->GetBlock(WG_MODE_NORMAL,window), window );
		else
			WgGfx::clipBlitBlock( clip, m_pBgGfx->GetBlock(WG_MODE_DISABLED,window), window );
	}


	// Render the menu-items

	WgBorders	contentBorders = GetContentBorders();

	WgMenuItem * pItem = m_items.First();

	Uint32	yPos = window.y + contentBorders.top - m_contentOfs;
	Uint32	xPosText = window.x + contentBorders.left + m_iconFieldWidth;
	Uint32	xPosIcon = window.x + contentBorders.left;
	Uint32	textFieldLen = window.w - contentBorders.Width() - m_iconFieldWidth - m_arrowFieldWidth;

	WgPen	entryPen( WgGfx::GetDevice(), WgCoord( xPosText, yPos ), clip );
	WgPen	accelPen( WgGfx::GetDevice(), WgCoord( xPosText, yPos ), clip );


	unsigned int	item = 1;
	while( pItem )
	{
		if( pItem->IsSetToVisible() )
		{
			if( pItem->GetType() == SEPARATOR )
			{
				if( m_pSepGfx )
				{
					WgRect sepClip(clip);
					sepClip.Shrink(0, contentBorders.top, 0, contentBorders.bottom);

					WgRect	dest( window.x + m_sepBorders.left, yPos + m_sepBorders.top,
									window.w - m_sepBorders.Width(), m_pSepGfx->Height() );

					WgGfx::clipBlitBlock( sepClip, m_pSepGfx->GetBlock(WG_MODE_NORMAL,dest), dest );
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

				WgRect tileClip = clip - contentBorders;			// Why do we need this? Something to do with scrolled content?

				WgRect tileDest(	window.x + contentBorders.left,
									yPos,
									window.w - contentBorders.Width(),
									m_entryHeight );


				_renderTile( WgGfx::GetDevice(), tileClip, tileDest, item-1, mode );

				// Draw the marked rectangle and change mode if this menuitem is selected
/*
				if( item == m_markedItem )
				{
					WgRect markClip(clip);
					markClip.Shrink(0, contentBorders.top, 0, contentBorders.bottom);

					WgRect dest(
						window.x + m_markBorders.left,
						yPos + m_markBorders.top,
						window.w - m_markBorders.Width(),
						m_entryHeight - m_markBorders.Height() );
					WgGfx::clipBlitBlock( markClip, m_pMarkGfx->GetBlock(WG_MODE_MARKED,dest), dest );



					mode = WG_MODE_MARKED;
				}
*/
				// Print the text (if any)

				const WgChar * pText = ((WgMenuEntry*)pItem)->GetText().Chars();
				if( pText->Glyph() != 0 )
				{

					WgTextAttr	attr;
					WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextProp(), mode );
					if( m_pBgGfx )
						WgTextTool::SetAttrColor( attr, m_pBgGfx->TextColors(), mode );
					WgTextTool::AddPropAttributes( attr, m_pEntryProp, mode );
					entryPen.SetAttributes( attr );
					int y = yPos + (m_entryHeight - entryPen.GetLineHeight())/2 + entryPen.GetBaseline();
					entryPen.SetPos( WgCoord( xPosText, y ) );
					WgGfx::printLine( entryPen, attr, pText );
				}

				// Print the accel text (if any)

				const WgChar * pAccelText = ((WgMenuEntry*)pItem)->GetAccelText().Chars();
				if( pAccelText->Glyph() != 0 )
				{
					WgTextAttr	attr;
					WgTextTool::AddPropAttributes( attr, WgBase::GetDefaultTextProp(), mode );
					if( m_pBgGfx )
						WgTextTool::SetAttrColor( attr, m_pBgGfx->TextColors(), mode );
					WgTextTool::AddPropAttributes( attr, m_pKeyAccelProp, mode );
					accelPen.SetAttributes( attr );

					int y = yPos + (m_entryHeight - accelPen.GetLineHeight())/2 + accelPen.GetBaseline();
					int width = WgTextTool::lineWidth( 0, attr, mode, pAccelText );
					int x = xPosText + textFieldLen - width;

					accelPen.SetPos( WgCoord(x, y) );
					WgGfx::printLine( accelPen, attr, pAccelText );
				}

				// Show the icon/checkbox/radiobutton (if any)

				switch( pItem->GetType() )
				{
					case ENTRY:
					{
						WgBlockSetPtr pIcon = ((WgMenuEntry*)pItem)->GetIcon();
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

							WgGfx::clipBlitBlock( clip, pIcon->GetBlock(mode), WgRect(x,y,w,h) );
						}
					}
					break;

					case CHECKBOX:
					{
						WgBlockSetPtr	pGfx;

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

							WgGfx::clipBlitBlock( clip, pGfx->GetBlock(mode), WgRect(x,y,w,h) );
						}
					}
					break;
					case RADIOBUTTON:
					{
						WgBlockSetPtr	pGfx;

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

							WgGfx::clipBlitBlock( clip, pGfx->GetBlock(mode), WgRect(x,y,w,h) );
						}
					}
					break;
					case SUBMENU:
					{

					}
					break;
					default:
					break;
				}




				// If submenu, show the submenu arrow




				yPos += m_entryHeight;
			}
		}

		pItem = pItem->Next();
		item++;
	}
}

//____ DoMyOwnUpdate() ________________________________________________________

void Wdg_Menu::DoMyOwnUpdate( const WgUpdateInfo& _updateInfo )
{
	if( m_selectorCountdown > 0 )
	{
		m_selectorCountdown -= _updateInfo.msDiff;
		if( m_selectorCountdown < 0 )
		{
			m_selectorCountdown = 0;
			m_nSelectorKeys = 0;
		}
	}
}

//____ DoMyOwnActionRespond() _________________________________________________
void Wdg_Menu::DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& _info, const WgInput& inputObj )
{
	switch( action )
	{
		case WgInput::POINTER_EXIT:
		{
			// Unmark any selected item unless it is a submenu...

			WgMenuItem * pOldItem = m_items.Get(m_markedItem-1);
			if( pOldItem && pOldItem->GetType() != SUBMENU )
			{
				m_markedItem = 0;
				RequestRender();
			}
		}
		break;

		case WgInput::POINTER_OVER:
		{
			if( _info.x != m_savedMousePos.x || _info.y != m_savedMousePos.y )
			{
				m_savedMousePos.x = _info.x;
				m_savedMousePos.y = _info.y;

				WgMenuItem * pItem = GetItemAtAbsPos( _info.x, _info.y );

				Uint32 markedItem = 0;
				if( pItem && !m_bPressOnSlider )
				{
						if( pItem->GetType() != SEPARATOR )
						{
							if( ((WgMenuEntry*)pItem)->IsEnabled() )
								markedItem = pItem->Index()+1;
						}

				}

				if(	markedItem != m_markedItem )
				{
					// We might need to close any previously marked submenu...

					if( m_markedItem != 0 )
					{
						CloseSubMenu(m_markedItem);
					}

					//

					m_markedItem = markedItem;
				}

				RequestRender();

				// Open submenu if we are over a submenu entry.

				if( pItem && pItem->GetType() == SUBMENU )
				{
					OpenSubMenu( (WgMenuSubMenu*) pItem );
				}
			}
		}
		break;

		case WgInput::BUTTON_RELEASE_OUTSIDE_MODAL:
			if( m_bPressOnSlider )
			{
				m_bPressOnSlider = false;
				break;
			}

			if( !m_btnReleaseArea.Contains( _info.x, _info.y ) )
			{
				Close();

				if( m_pParentMenu )
					m_pParentMenu->Close();
			}


		break;
		case WgInput::BUTTON_PRESS_OUTSIDE_MODAL:
			Close();
			if( m_pParentMenu )
				m_pParentMenu->Close();
		break;

		case WgInput::BUTTON_RELEASE:
		{
			if( m_bPressOnSlider )
			{
				m_bPressOnSlider = false;
				break;
			}

			WgMenuItem * pItem = GetItemAtAbsPos( _info.x, _info.y );
			if( pItem )
			{
				if( pItem->GetType() == ENTRY || pItem->GetType() == CHECKBOX || pItem->GetType() == RADIOBUTTON )
					Emit( MenuEntryPressed(), pItem );

				SelectItem(pItem);
			}
		}
		break;


		case WgInput::CHARACTER:
		{
			if( _info.character != 0 )
			{
				m_selectorCountdown = c_selectorCountdownStart;

				if( m_nSelectorKeys < c_maxSelectorKeys )
				{
					m_selectorKeys[m_nSelectorKeys++] = towlower( _info.character );
					MarkFirstFilteredEntry();
				}
			}
		}
		break;

		case WgInput::KEY_PRESS:
		case WgInput::KEY_REPEAT:
		{
			WgMenuItem * pItem = 0;
			if( m_markedItem != 0 )
				pItem = m_items.Get( m_markedItem-1 );


			switch( button_key )
			{
				case WG_KEY_ESCAPE:
					Close();
					pItem = 0;				// So we won't mark an item in the closed menu.
					break;

				case WG_KEY_RIGHT:
					if( pItem )
					{
						if( pItem->GetType() == SUBMENU )
							OpenSubMenu( (WgMenuSubMenu*) pItem );
					}
					break;

				case WG_KEY_LEFT:
					if( m_pParentMenu )
							Close();
					break;

				case WG_KEY_RETURN:
					if( pItem )
					{
						if( pItem->GetType() == ENTRY || pItem->GetType() == CHECKBOX || pItem->GetType() == RADIOBUTTON )
							Emit( MenuEntryPressed(), pItem );

						if( pItem->GetType() == SUBMENU )
							OpenSubMenu( (WgMenuSubMenu*) pItem );
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

				case WG_KEY_PAGEUP:
				{
					int viewHeight = GetViewSizePixels();

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
				case WG_KEY_PAGEDOWN:
				{
					int viewHeight = GetViewSizePixels();

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
					ScrollItemIntoView(pItem);
					RequestRender();
				}
			}

		}
		break;

        default:
            break;

	}

}

//____ SelectItem() _________________________________________________
void Wdg_Menu::SelectItem(WgMenuItem* pItem)
{
	m_pSelectedItem = 0;

	if(pItem == 0 || !pItem->IsSetToVisible() )
		return;

	switch( pItem->GetType() )
	{
		case ENTRY:
			m_pSelectedItem = pItem;
			Emit( MenuEntrySelected(), pItem );
			Close();
			if( m_pParentMenu )
				m_pParentMenu->Close();
		break;
		case CHECKBOX:
		{
			WgMenuCheckBox * pCheckBox = (WgMenuCheckBox*) pItem;

			if( pCheckBox->IsChecked() )
			{
				pCheckBox->Uncheck();
				Emit( MenuEntryUnchecked(), pItem );
			}
			else
			{
				pCheckBox->Check();
				Emit( MenuEntryChecked(), pItem );
			}

			Close();
			if( m_pParentMenu )
				m_pParentMenu->Close();
		}
		break;
		case RADIOBUTTON:
			m_pSelectedItem = pItem;
			((WgMenuRadioButton*)pItem)->Select();
			Emit( MenuEntrySelected(), pItem );
			Close();
			if( m_pParentMenu )
				m_pParentMenu->Close();
		break;

		default:
            break;
	}
}

//____ Open() _________________________________________________________________

void Wdg_Menu::Open( )
{
	Open( m_pMenuRoot, m_menuPosX, m_menuPosY, m_menuMinW, 0, &m_btnReleaseArea, m_pParentMenu );
}

void Wdg_Menu::Open( Wdg_Root * pRoot, Uint32 x, Uint32 y, Uint32 minW, WgMenuItem * pMarkedItem, WgRect * pBtnReleaseArea, Wdg_Menu * pParentMenu )
{
	if( pBtnReleaseArea )
		m_btnReleaseArea = * pBtnReleaseArea;
	else
		m_btnReleaseArea = WgRect(0,0,0,0);

	m_menuMinW = minW;

	m_pParentMenu = pParentMenu;

	if( (Uint32) Width() < minW )
		AdjustSize();

	m_savedMousePos.x = -1;
	m_savedMousePos.y = -1;

	m_nSelectorKeys			= 0;
	m_selectorCountdown		= 0;

	SetParent(pRoot);
	SetPos( x, y );
	Emit( MenuOpened() );

	_releasePointer();	// Was pressed on other widget.
	GrabInputFocus();
	if( pMarkedItem && pMarkedItem->Chain() == &m_items )
	{
		m_markedItem = pMarkedItem->Index()+1;
		RequestRender();
		ScrollItemIntoView( pMarkedItem, true );
	}
}

//____ Close() ________________________________________________________________

void Wdg_Menu::Close()
{
	// Close any selected submenu (if any). Don't bother to check type,
	// method does the checking.

	if( m_markedItem )
		CloseSubMenu(m_markedItem);

	//
	m_markedItem = 0;
	if(m_pParent != 0)
	{
		SetParent(0);
		Emit( MenuClosed() );
	}

	if( m_pParentMenu )
		m_pParentMenu->GrabInputFocus();
}



//____ OpenSubMenu() __________________________________________________________

void Wdg_Menu::OpenSubMenu( WgMenuSubMenu * pItem )
{
	m_bIsOpeningSubMenu = true;

	Wdg_Menu * pMenu = pItem->GetSubMenu();

	if( !pMenu )
		return;

	// Position immediately right (preferably) or left of us.

	Uint32 xOfs = m_geo.x;

	if( pMenu->Width() <= Parent()->Width() - m_geo.x - m_geo.w )
		xOfs += m_geo.w;
	else
		xOfs -= pMenu->m_geo.w;


	// Figure out Y-offset for pItem

	Uint32 yOfs = GetContentBorders().top + m_geo.y;

	WgMenuItem * p = m_items.First();
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

	// Calculate btnReleaseArea

	WgRect releaseArea( m_geo.y + yOfs, m_geo.x, m_entryHeight, m_geo.w );


	// Position at entry and downwards (prefered) or upwards.

	if( m_geo.y + yOfs + pMenu->Height() <= (Uint32) Parent()->Height() )
		yOfs += 0;
	else
		yOfs += m_entryHeight - (Uint32) pMenu->Height();


	pMenu->Open( (Wdg_Root*)Root(), xOfs, yOfs, 0, 0, &releaseArea, this );

	m_bIsOpeningSubMenu = false;
}

//____ CloseSubMenu() _________________________________________________________

void Wdg_Menu::CloseSubMenu( Uint32 item )
{
	WgMenuItem * pItem = m_items.Get(item-1);

	if( pItem->GetType() == SUBMENU )
	{
		Wdg_Menu * p = ((WgMenuSubMenu*)pItem)->GetSubMenu();
		if( p )
			p->Close();
	}
}


//____ DoMyOwnInputFocusChange() ______________________________________________

void Wdg_Menu::DoMyOwnInputFocusChange( bool _bFocus )
{
	if(!_bFocus && !m_bIsOpeningSubMenu)
	{
		Close();
	}
}

//____ DoMyOwnGeometryChange() ________________________________________________

void Wdg_Menu::DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo )
{
	if( oldGeo.w != newGeo.w || oldGeo.h != newGeo.h )
		AdjustSize();
}

//____ DoMyOwnMarkTest() ___________________________________________________

bool Wdg_Menu::DoMyOwnMarkTest( int _x, int _y )
{
	return true;
}



//____ GetItemAtAbsPos() ______________________________________________________

WgMenuItem * Wdg_Menu::GetItemAtAbsPos( int x, int y )
{
	WgBorders	contentBorders = GetContentBorders();

	Abs2local( &x, &y );
	x -= contentBorders.left;
	y -= contentBorders.top;

	y += m_contentOfs;

	if( y > 0 && x > 0 && x < (int) (m_geo.w - contentBorders.Width() ) )
	{
		WgMenuItem * pItem = m_items.First();
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

//____ AdjustSize() ___________________________________________________________

void Wdg_Menu::AdjustSize()
{
	WgBorders contentBorders = GetContentBorders();

	int  w = contentBorders.Width();
	int	 h = contentBorders.Height();

	int minSep = m_sepBorders.Width();
	if( m_pSepGfx )
		minSep += m_pSepGfx->MinWidth();

	WgMenuItem * pItem = m_items.First();
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

	if( h > MaxHeight() )
	{
		h = MaxHeight();

		if( !m_pSlider )
		{

			m_pSlider = new Wdg_VDrag( WgRect(0,0,100,200),0 );

			m_pSlider->SetSource( m_pSliderBgGfx, m_pSliderBarGfx, m_pSliderBtnBwdGfx, m_pSliderBtnFwdGfx );
			m_pSlider->SetButtonLayout( m_sliderBtnLayout );
			m_pSlider->SetSize(0,0);						// Make it as small as possible.

			m_pSlider->AddCallback( WgSignal::SliderPos(), cbSliderPos, this );
			m_pSlider->AddCallback( WgSignal::Forward(), cbSliderFwd, this );
			m_pSlider->AddCallback( WgSignal::Back(), cbSliderBwd, this );
			m_pSlider->AddCallback( WgSignal::PrevPage(), cbSliderPrevPage, this );
			m_pSlider->AddCallback( WgSignal::NextPage(), cbSliderNextPage, this );

			m_pSlider->AddCallback( WgSignal::ButtonPress(1), cbPressOnSlider, this );
			m_pSlider->AddCallback( WgSignal::ButtonRelease(1), cbReleasedSlider, this );

			AddCallback( WgSignal::ViewPosY(), WgGizmoDragbar::cbSetSliderPos, (WgGizmoDragbar*) m_pSlider );
			AddCallback(WgSignal::WheelRoll(1), cbWheelRoll, this);
		}

		WgSize sliderSize = m_pSlider->DefaultSize();

		w += sliderSize.w;

		if( h < sliderSize.h + contentBorders.Height() )
		{
			h = sliderSize.h + contentBorders.Height();
			WgWidget::SetHeight(h);							// Need to change height before we can add slider...
		}

		m_pSlider->SetGeometry( WgOrigo::topRight(), -(contentBorders.right+sliderSize.w), contentBorders.top,
								WgOrigo::bottomRight(), -contentBorders.right, -contentBorders.bottom );

		if( !m_pSlider->Parent() )
		{
			m_pSlider->SetParent(this);
			m_bPressOnSlider = false;		// Precaution...
		}
	}
	else
	{
		if( m_pSlider )
			m_pSlider->SetParent(0);
	}


	if( w < MinWidth() )
		w = MinWidth();

	if( w < m_menuMinW )
		w = m_menuMinW;

	WgWidget::SetSize(w, h);

	if( m_pSlider )
		m_pSlider->SetSlider( GetViewOfs(), GetViewSize() );
}

//____ GetViewOfs() ___________________________________________________________

float Wdg_Menu::GetViewOfs()
{
	return ((float)m_contentOfs) / (m_contentHeight-(Height()-GetContentBorders().Height()));
}

//____ GetViewSize() __________________________________________________________

float Wdg_Menu::GetViewSize()
{
	return ((float)(GetViewSizePixels())) / m_contentHeight;
}

//____ GetViewSizePixels() ____________________________________________________

int Wdg_Menu::GetViewSizePixels()
{
	return Height()-GetContentBorders().Height();
}


//____ SetView() ______________________________________________________________

void Wdg_Menu::SetView(float pos)
{
	if( pos < 0.f )
		pos = 0.f;

	if( pos > 1.f )
		pos = 1.f;

	int viewHeight = Height() - GetContentBorders().Height();

	int ofs = (int) (pos * (m_contentHeight-viewHeight));

	if( ofs != (signed int) m_contentOfs )
	{
		m_contentOfs = ofs;
		Emit( ViewPosY(), pos );
		RequestRender();
	}

}

//____ SetViewPixels() ______________________________________________________________

void Wdg_Menu::SetViewPixels(int pos)
{
	if( pos < 0 )
		pos = 0;

	int viewHeight = Height() - GetContentBorders().Height();

	if( pos + viewHeight > (int) m_contentHeight )
		pos = m_contentHeight - viewHeight;

	if( pos != (int) m_contentOfs )
	{
		m_contentOfs = pos;
		Emit( ViewPosY(), GetViewOfs() );

		RequestRender();
	}
}

//____ StepView() _________________________________________________________

void Wdg_Menu::StepWheelRoll(int distance)
{
	distance /= 40;
	while(distance++ < 0)
		StepViewDown();
	while(--distance > 0)
		StepViewUp();
}

//____ StepViewDown() _________________________________________________________

void Wdg_Menu::StepViewDown()
{
	SetViewPixels( m_contentOfs + m_entryHeight );
}

//____ StepViewUp() ___________________________________________________________

void Wdg_Menu::StepViewUp()
{
	SetViewPixels( m_contentOfs - m_entryHeight );
}

//____ StepViewPageDown() _____________________________________________________

void Wdg_Menu::StepViewPageDown()
{
	int viewHeight = Height() - GetContentBorders().Height();
	SetViewPixels( m_contentOfs + (viewHeight - m_entryHeight) );
}

//____ StepViewPageUp() _______________________________________________________

void Wdg_Menu::StepViewPageUp()
{
	int viewHeight = Height() - GetContentBorders().Height();
	SetViewPixels( m_contentOfs - (viewHeight - m_entryHeight) );
}

//____ _tilesModified() _______________________________________________________

void Wdg_Menu::_tilesModified()
{
	RequestRender();
}