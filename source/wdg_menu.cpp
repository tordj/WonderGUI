
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

#include	<wdg_menu.h>
#include	<wdg_root.h>
#include	<wdg_dragbars.h>
#include	<wg_surface.h>
#include	<wg_gfx.h>
#include	<wg_char.h>
#include	<wg_font.h>
#include	<wg_texttool.h>
#include	<wg_pen.h>

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
	m_pMarkGfx				= 0;
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
	m_sepHeight		= m_pSepGfx->GetHeight() + m_sepBorders.GetHeight();

	AdjustSize();
	RequestRender();
	return true;
}

//____ SetMarkSource() ________________________________________________________

bool Wdg_Menu::SetMarkSource( const WgBlockSetPtr pGfx, const WgBorders& borders )
{
	if( pGfx == WgBlockSetPtr(0) )
		return false;

	m_pMarkGfx		= pGfx;
	m_markBorders	= borders;

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
		WgPen	pen;
		pen.SetTextProp( m_pEntryProp.GetHandle(), 0, WG_MODE_NORMAL );
		int	heightNormal	= pen.GetLineSpacing();

		pen.SetTextProp( m_pEntryProp.GetHandle(), 0, WG_MODE_MARKED );
		int heightMarked	= pen.GetLineSpacing();

		pen.SetTextProp( m_pEntryProp.GetHandle(), 0, WG_MODE_DISABLED );
		int heightDisabled	= pen.GetLineSpacing();

		if( m_entryHeight < heightNormal )
			m_entryHeight = heightNormal;

		if( m_entryHeight < heightMarked )
			m_entryHeight = heightMarked;

		if( m_entryHeight < heightDisabled )
			m_entryHeight = heightDisabled;
	}

	// We need to modify MinWidth now that fonts might have changed

	WgMenuItem * pItem = m_items.getFirst();
	while( pItem )
	{
		if( pItem->GetType() != SEPARATOR )
			CalcEntryMinWidth( (WgMenuEntry*) pItem );
		pItem = pItem->getNext();
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

void Wdg_Menu::AddItem( WgMenuItem * pItem )
{
	// Calculate minWidth for all derived from ENTRY

	if( pItem->GetType() != SEPARATOR )
		CalcEntryMinWidth( (WgMenuEntry*) pItem );

	// Let item do what it needs to

	pItem->SetMyMenu( this );

	// Add the item.

	m_items.push_back(pItem);
	m_nItems++;

	// Refresh what needs to be refreshed...

	AdjustSize();
	RequestRender();
}

//____ ClearItems() _____________________________________________________
void Wdg_Menu::ClearItems()
{
	Close();
	m_items.clear();
	m_nItems = 0;
}

//____ FindItem() _____________________________________________________
WgMenuItem* Wdg_Menu::FindItem(int id)
{
	WgMenuItem* pItem = m_items.getFirst();
	for(; pItem; pItem = pItem->getNext())
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
		Uint32		itemNb = pItem->getIndex()+1;
		if( itemNb != m_markedItem )
			CloseSubMenu(m_markedItem);
	}
	else
		Emit( WgSignal::PointerOutsideModalPos(), x, y );
}


//____ CalcEntryMinWidth() _____________________________________________________

void Wdg_Menu::CalcEntryMinWidth( WgMenuEntry * pEntry )
{
	int wNormal = 0;
	int wMarked = 0;



	wNormal += WgTextTool::lineWidth( m_pEntryProp, "  " );
	wMarked += WgTextTool::lineWidth( m_pEntryProp, "  " );

	wNormal += WgTextTool::lineWidth( m_pEntryProp, WG_MODE_NORMAL, pEntry->GetText().GetChars() );
	wMarked += WgTextTool::lineWidth( m_pEntryProp, WG_MODE_MARKED, pEntry->GetText().GetChars() );

	wNormal += WgTextTool::lineWidth( m_pKeyAccelProp, WG_MODE_NORMAL, pEntry->GetAccelText().GetChars() );
	wMarked += WgTextTool::lineWidth( m_pKeyAccelProp, WG_MODE_MARKED, pEntry->GetAccelText().GetChars() );


	if( wNormal > wMarked )
		pEntry->m_minWidth = wNormal;
	else
		pEntry->m_minWidth = wMarked;
}

//____ GetContentBorders() ____________________________________________________

WgBorders Wdg_Menu::GetContentBorders() const
{
	if( m_pBgGfx )
		return m_pBgGfx->GetContentBorders();
	else
		return WgBorders(0);
}


//____ DoMyOwnRender() ________________________________________________________

void Wdg_Menu::DoMyOwnRender( const WgRect& window, const WgRect& clip, Uint8 _layer )
{

	// Render background

	if( m_pBgGfx )
	{
		if( m_bEnabled )
			WgGfx::clipBlitBlock( clip, m_pBgGfx->GetBlock(WG_MODE_NORMAL), window );
		else
			WgGfx::clipBlitBlock( clip, m_pBgGfx->GetBlock(WG_MODE_DISABLED), window );
	}


	// Render the menu-items

	WgBorders	contentBorders = GetContentBorders();

	WgMenuItem * pItem = m_items.getFirst();

	Uint32	yPos = window.y + contentBorders.top - m_contentOfs;
	Uint32	xPosText = window.x + contentBorders.left + m_iconFieldWidth;
	Uint32	xPosIcon = window.x + contentBorders.left;
	Uint32	textFieldLen = window.w - contentBorders.GetWidth() - m_iconFieldWidth - m_arrowFieldWidth;

	WgPen	entryPen( WgGfx::GetDevice(), WgCord( xPosText, yPos ), clip );
	WgPen	accelPen( WgGfx::GetDevice(), WgCord( xPosText, yPos ), clip );

	entryPen.SetTextProp( m_pEntryProp );
	accelPen.SetTextProp( m_pKeyAccelProp );


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
									window.w - m_sepBorders.GetWidth(), m_pSepGfx->GetHeight() );

					WgGfx::clipBlitBlock( sepClip, m_pSepGfx->GetBlock(WG_MODE_NORMAL), dest );
					yPos += m_sepHeight;
				}
			}
			else
			{
				WgMode	mode = WG_MODE_DISABLED;

				if( ((WgMenuEntry*)pItem)->IsEnabled() )
					mode = WG_MODE_NORMAL;

				// Draw the marked rectangle and change mode if this menuitem is selected

				if( item == m_markedItem )
				{
					WgRect markClip(clip);
					markClip.Shrink(0, contentBorders.top, 0, contentBorders.bottom);

					WgRect dest(
						window.x + m_markBorders.left,
						yPos + m_markBorders.top,
						window.w - m_markBorders.GetWidth(),
						m_entryHeight - m_markBorders.GetHeight() );
					WgGfx::clipBlitBlock( markClip, m_pMarkGfx->GetBlock(WG_MODE_MARKED), dest );

					mode = WG_MODE_MARKED;
				}

				// Print the text (if any)

				const WgChar * pText = ((WgMenuEntry*)pItem)->GetText().GetChars();
				if( * pText != 0 )
				{
					int y = yPos + (m_entryHeight - entryPen.GetLineHeight())/2 + entryPen.GetBaseline();
					entryPen.SetPos( WgCord( xPosText, y ) );
					WgGfx::printLine( &entryPen, m_pEntryProp, mode, pText );
				}

				// Print the accel text (if any)

				const WgChar * pAccelText = ((WgMenuEntry*)pItem)->GetAccelText().GetChars();
				if( * pAccelText != 0 )
				{
					int y = yPos + (m_entryHeight - accelPen.GetLineHeight())/2 + accelPen.GetBaseline();
					int width = WgTextTool::lineWidth( m_pKeyAccelProp, mode, pAccelText );
					int x = xPosText + textFieldLen - width;

					accelPen.SetPos( WgCord(x, y) );
					WgGfx::printLine( &accelPen, m_pKeyAccelProp, mode, pAccelText );
				}

				// Show the icon/checkbox/radiobutton (if any)

				switch( pItem->GetType() )
				{
					case ENTRY:
					{
						WgBlockSetPtr pIcon = ((WgMenuEntry*)pItem)->GetIcon();
						if( pIcon )
						{
							int w = pIcon->GetWidth();
							int h = pIcon->GetHeight();

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
							int w = pGfx->GetWidth();
							int h = pGfx->GetHeight();

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
							int w = pGfx->GetWidth();
							int h = pGfx->GetHeight();

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

		pItem = pItem->getNext();
		item++;
	}
}

//____ DoMyOwnUpdate() ________________________________________________________

void Wdg_Menu::DoMyOwnUpdate( const WgUpdateInfo& _updateInfo )
{
}

//____ DoMyOwnActionRespond() _________________________________________________
void Wdg_Menu::DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& _info, const WgInput& inputObj )
{
	switch( action )
	{
		case WgInput::POINTER_EXIT:
		{
			// Unmark any selected item unless it is a submenu...

			WgMenuItem * pOldItem = m_items.get(m_markedItem-1);
			if( pOldItem && pOldItem->GetType() != SUBMENU )
			{
				m_markedItem = 0;
				RequestRender();
			}
		}
		break;

		case WgInput::POINTER_OVER:
		{
			WgMenuItem * pItem = GetItemAtAbsPos( _info.x, _info.y );

			Uint32 markedItem = 0;
			if( pItem && !m_bPressOnSlider )
			{
					if( pItem->GetType() != SEPARATOR )
					{
						if( ((WgMenuEntry*)pItem)->IsEnabled() )
							markedItem = pItem->getIndex()+1;
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
	Open( m_pMenuRoot, m_menuPosX, m_menuPosY, m_menuMinW, &m_btnReleaseArea, m_pParentMenu );
}

void Wdg_Menu::Open( Wdg_Root * pRoot, Uint32 x, Uint32 y, Uint32 minW, WgRect * pBtnReleaseArea, Wdg_Menu * pParentMenu )
{
	if( pBtnReleaseArea )
		m_btnReleaseArea = * pBtnReleaseArea;
	else
		m_btnReleaseArea = WgRect(0,0,0,0);

	m_menuMinW = minW;

	m_pParentMenu = pParentMenu;

	if( (Uint32) Width() < minW )
		AdjustSize();

	SetParent(pRoot);
	SetPos( x, y );
	Emit( MenuOpened() );
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
}



//____ OpenSubMenu() __________________________________________________________

void Wdg_Menu::OpenSubMenu( WgMenuSubMenu * pItem )
{
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

	WgMenuItem * p = m_items.getFirst();
	while( p != pItem )
	{
		if( p->IsSetToVisible() )
		{
			if( p->GetType() == SEPARATOR )
				yOfs += m_sepHeight;
			else
				yOfs += m_entryHeight;
		}
		p = p->getNext();
	}

	// Calculate btnReleaseArea

	WgRect releaseArea( m_geo.y + yOfs, m_geo.x, m_entryHeight, m_geo.w );


	// Position at entry and downwards (prefered) or upwards.

	if( m_geo.y + yOfs + pMenu->Height() <= (Uint32) Parent()->Height() )
		yOfs += 0;
	else
		yOfs += m_entryHeight - (Uint32) pMenu->Height();


	pMenu->Open( (Wdg_Root*)Root(), xOfs, yOfs, 0, &releaseArea, this );
}

//____ CloseSubMenu() _________________________________________________________

void Wdg_Menu::CloseSubMenu( Uint32 item )
{
	WgMenuItem * pItem = m_items.get(item-1);

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

	if( y > 0 && x > 0 && x < (int) (m_geo.w - contentBorders.GetWidth() ) )
	{
		WgMenuItem * pItem = m_items.getFirst();
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
			pItem = pItem->getNext();
		}
	}

	return 0;
}

//____ AdjustSize() ___________________________________________________________

void Wdg_Menu::AdjustSize()
{
	WgBorders contentBorders = GetContentBorders();

	Uint32  w = contentBorders.GetWidth();
	Uint32	h = contentBorders.GetHeight();

	Uint32 minSep = m_sepBorders.GetWidth();
	if( m_pSepGfx )
		minSep += m_pSepGfx->GetMinWidth();

	WgMenuItem * pItem = m_items.getFirst();
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

				Uint32 minW = ((WgMenuEntry*)pItem)->m_minWidth + contentBorders.GetWidth() + m_iconFieldWidth + m_arrowFieldWidth;

				if( w < minW )
					w = minW;
			}
		}


		pItem = pItem->getNext();
	}

	m_contentHeight = h - contentBorders.GetHeight();

	if( h < m_entryHeight + contentBorders.GetHeight() )
		h = m_entryHeight + contentBorders.GetHeight();

	if( h > (Uint32) MaxHeight() )
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

			AddCallback( WgSignal::ViewPosY(), Wdg_VDrag::cbSetSliderPos, m_pSlider );
			AddCallback(WgSignal::WheelRoll(1), cbWheelRoll, this);
		}

		int sliderWidth = m_pSlider->MinWidth();
		int sliderHeight = m_pSlider->MinHeight();

		w += sliderWidth;

		if( h < sliderHeight + contentBorders.GetHeight() )
		{
			h = sliderHeight + contentBorders.GetHeight();
			WgWidget::SetHeight(h);							// Need to change height before we can add slider...
		}

		m_pSlider->SetGeometry( WgOrigo::topRight(), -(contentBorders.right+sliderWidth), contentBorders.top,
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


	if( w < (Uint32) MinWidth() )
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
	return ((float)m_contentOfs) / (m_contentHeight-(Height()-GetContentBorders().GetHeight()));
}

//____ GetViewSize() __________________________________________________________

float Wdg_Menu::GetViewSize()
{
	return ((float)(Height()-GetContentBorders().GetHeight())) / m_contentHeight;
}

//____ SetView() ______________________________________________________________

void Wdg_Menu::SetView(float pos)
{
	if( pos < 0.f )
		pos = 0.f;

	if( pos > 1.f )
		pos = 1.f;

	int viewHeight = Height() - GetContentBorders().GetHeight();

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

	int viewHeight = Height() - GetContentBorders().GetHeight();

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
	int viewHeight = Height() - GetContentBorders().GetHeight();
	SetViewPixels( m_contentOfs + (viewHeight - m_entryHeight) );
}

//____ StepViewPageUp() _______________________________________________________

void Wdg_Menu::StepViewPageUp()
{
	int viewHeight = Height() - GetContentBorders().GetHeight();
	SetViewPixels( m_contentOfs - (viewHeight - m_entryHeight) );
}


