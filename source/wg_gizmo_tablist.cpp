#include <assert.h>
#include <memory.h>
#include <algorithm>

#include <wg_gizmo_tablist.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>
#include <wg_util.h>

#include <wg_item_row.h>


static const char	Wdg_Type[] = {"TordJ/TabList"};

//____ WgTab::Constructor __________________________________________________________

WgTab::WgTab( Sint32 _id )
{
	m_id = _id;
	m_bAlert = false;
	m_width = 1;
	m_text.setAlignment(WgOrigo::midCenter());
	m_text.SetWrap(false);
	m_pItemRow = NULL;
	m_bLockedContent = false;
	m_bVisible = true;
}

//____ WgTab::Destructor __________________________________________________________

WgTab::~WgTab()
{
	if( m_pItemRow )
		delete m_pItemRow;
}

//____________________________________________________________________________
//
//	WgGizmoTablist
//____________________________________________________________________________


//____ WgGizmoTablist() _________________________________________________________________

WgGizmoTablist::WgGizmoTablist()
{
	memset(m_sources, 0, sizeof(m_sources));

	m_pTabSelected	= 0;
	m_pTabMarked	= 0;

	m_bAlertOn		= false;
	m_alertModeCnt	= 0;
	m_alertRate		= 250;

	m_minTabWidth	= 0;
	m_overlap		= 0;

//	m_textOrigo		= WgOrigo::topLeft();
	m_textOrigo		= WgOrigo::midCenter();

	m_tabWidthMode	= TabWidthModeNormal;
	m_widestTab		= 0;
}

//____ ~WgGizmoTablist() _________________________________________________________

WgGizmoTablist::~WgGizmoTablist( void )
{
}


//____ Type() _________________________________________________________________

const char * WgGizmoTablist::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoTablist::GetMyType( void )
{
	return Wdg_Type;
}

//____ SetTextProperties() ____________________________________________________

void WgGizmoTablist::SetTextProperties( const WgTextPropPtr& pProp )
{

	if( m_pProp != pProp )
	{
		m_pProp = pProp;

		// Change font for all tabs.

		// the font is set on every render anyway,
		// so i'll comment out this part

		WgTab * pTab = m_tabs.getFirst();
		while( pTab )
		{
			pTab->m_text.setDefaultProperties(m_pProp);
			pTab = pTab->getNext();
		}

		//

		ResizeAllTabs();
		RequestRender();
	}

}

//____ SetSource() ____________________________________________________________

void WgGizmoTablist::SetSource( WgBlockSetPtr pBlockSet, SourceType type )
{
	if(type == SourceTypeAll)
	{
		SetSource( pBlockSet, SourceTypeLeft );
		SetSource( pBlockSet, SourceTypeMiddle );
		SetSource( pBlockSet, SourceTypeRight );
		return;
	}

	m_sources[type] = pBlockSet;
	RequestRender();
}



//____ SetOverlap() ___________________________________________________________

void WgGizmoTablist::SetOverlap( Uint16 overlap )
{
	m_overlap = overlap;
	RequestRender();
}

//____ SetMinTabWidth() _______________________________________________________

void WgGizmoTablist::SetMinTabWidth( Uint16 minWidth )
{
	m_minTabWidth	= minWidth;

	if( ResizeAllTabs() )
		RequestRender();
}

//____ SetAlertRate() _________________________________________________________

void WgGizmoTablist::SetAlertRate( Uint32 millisec )
{
	m_alertRate = millisec;
}

//____ SetTextOrigo() _________________________________________________________

void WgGizmoTablist::SetTextOrigo( WgOrigo origo )
{
	m_textOrigo = origo;
	RequestRender();
}

//____ SetExpandTabsToWidth() _________________________________________________

void WgGizmoTablist::SetTabWidthMode(TabWidthMode mode)
{
	m_tabWidthMode = mode;
}

//____ AddTab() _______________________________________________________________

bool WgGizmoTablist::AddTab( Sint32 id, const char * pText, Uint32 position, const WgBlockSetPtr& pGfx )
{
	if( FindTab(id) )
		return false;					// We already have a tab with this ID...


	WgTab * pTab = new WgTab(id);

	pTab->m_text.setText(pText);
	pTab->m_text.setDefaultProperties(m_pProp);
	pTab->SetSource( pGfx );
	ResizeTab(pTab);

	WgTab * pPos = m_tabs.get(position);
	if( pPos )
		pTab->moveBefore(pPos);
	else
		m_tabs.push_back(pTab);
	RequestRender();
	return true;
}

//____ AddTab() _______________________________________________________________

bool WgGizmoTablist::AddTab( Sint32 id, const Uint16* pText, Uint32 position, const WgBlockSetPtr& pGfx )
{
	if( FindTab(id) )
		return false;					// We already have a tab with this ID...

	WgTab * pTab = new WgTab(id);

	pTab->m_text.setText(pText);
	pTab->m_text.setDefaultProperties(m_pProp);
	pTab->SetSource( pGfx );
	ResizeTab(pTab);

	WgTab * pPos = m_tabs.get(position);
	if( pPos )
		pTab->moveBefore(pPos);
	else
		m_tabs.push_back(pTab);
	RequestRender();
	return true;
}

//____ AddTab() _______________________________________________________________

bool WgGizmoTablist::AddTab( Sint32 id, const WgChar* pText, Uint32 position, const WgBlockSetPtr& pGfx )
{
	if( FindTab(id) )
		return false;					// We already have a tab with this ID...

	WgTab * pTab = new WgTab(id);

	pTab->m_text.setText(pText);
	pTab->m_text.setDefaultProperties(m_pProp);
	pTab->SetSource( pGfx );
	ResizeTab(pTab);

	WgTab * pPos = m_tabs.get(position);
	if( pPos )
		pTab->moveBefore(pPos);
	else
		m_tabs.push_back(pTab);
	RequestRender();
	return true;
}

//____ AddTab() _______________________________________________________________

bool WgGizmoTablist::AddTab( Sint32 id, const WgText * pText, Uint32 position, const WgBlockSetPtr& pGfx )
{
	if( FindTab(id) )
		return false;					// We already have a tab with this ID...

	WgTab * pTab = new WgTab(id);

	pTab->m_text.setText(pText);
	pTab->m_text.setDefaultProperties(m_pProp);
	pTab->SetSource( pGfx );
	ResizeTab(pTab);

	WgTab * pPos = m_tabs.get(position);
	if( pPos )
		pTab->moveBefore(pPos);
	else
		m_tabs.push_back(pTab);
	RequestRender();
	return true;
}

//____ RemoveTab() ____________________________________________________________

bool WgGizmoTablist::RemoveTab( Sint32 id )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		if( m_pTabSelected == pTab )
			m_pTabSelected = 0;
		if( m_pTabMarked == pTab )
			m_pTabMarked = 0;

		delete pTab;
		RequestRender();
		return true;
	}
	return false;
}

//____ RemoveAllTabs() ______________________________________________________________

void WgGizmoTablist::RemoveAllTabs()
{
	m_pTabSelected = 0;
	m_pTabMarked = 0;

	WgTab *pTab = m_tabs.getFirst();
	while(pTab)
	{
		WgTab *pNext = pTab->getNext();
		delete pTab;
		pTab = pNext;
	}

	RequestRender();
}

//____ MoveTab() ______________________________________________________________

bool WgGizmoTablist::MoveTab( Sint32 id, Uint32 position )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		WgTab * pPos = m_tabs.get(position);
		if( pPos )
			pTab->moveBefore(pPos);
		else
			m_tabs.push_back(pTab);
		RequestRender();
		return true;
	}
	return false;
}

//____ SetTabId() ___________________________________________________________

bool WgGizmoTablist::SetTabId( Sint32 id, Sint32 newId )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		if( !FindTab( newId ) )
		{
			pTab->m_id = newId;
		}
	}
	return false;
}

//____ GetTabId() ___________________________________________________________

Sint32 WgGizmoTablist::GetTabId(Uint32 position)
{
	WgTab *pTab = m_tabs.getFirst();
	if( pTab && position )
	{
		position--;
		pTab = pTab->getNext();
	}
	if(pTab)
		return pTab->m_id;
	return -1;
}

//____ SetTabText() ___________________________________________________________

bool WgGizmoTablist::SetTabText( Sint32 id, const char * pText )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		pTab->m_text.setText( pText );
		ResizeTab(pTab);
		RequestRender();
		return true;
	}
	return false;
}

//____ SetTabText() ___________________________________________________________

bool WgGizmoTablist::SetTabText( Sint32 id, const Uint16* pText )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		pTab->m_text.setText( pText );
		ResizeTab(pTab);
		RequestRender();
		return true;
	}
	return false;
}

//____ SetTabText() ___________________________________________________________

bool WgGizmoTablist::SetTabText( Sint32 id, const WgText * pText )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		pTab->m_text.setText( pText );
		ResizeTab(pTab);
		RequestRender();
		return true;
	}
	return false;
}

//____ SetTabTextColor() ____________________________________________________________
bool WgGizmoTablist::SetTabTextColor( Sint32 id, WgColor col )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		pTab->m_text.setColor( col );
		RequestRender();
		return true;
	}
	return false;
}

//____ SelectTab() ____________________________________________________________

bool WgGizmoTablist::SelectTab( Sint32 id )
{
	WgTab * pTab = FindTab(id);

	if( pTab == m_pTabSelected )
		return true;				// Not an error...

	if( pTab )
	{
		m_pTabSelected = pTab;
		pTab->m_bAlert = false;		// Selecting automatically stops any alert.
		ResizeAllTabs();	// fonts have changed
		RequestRender();
		
		if( m_pHook )
			m_pHook->GetEmitter()->Emit( WgSignal::TabSelected(), pTab->m_id );
		return true;
	}

	ResizeAllTabs();	// fonts have changed
	RequestRender();

	return false;
}

//____ GetSelectedTabId() _____________________________________________________________

int WgGizmoTablist::GetSelectedTabId() const
{
	if( m_pTabSelected )
		return m_pTabSelected->m_id;
	return -1;
}

//____ GetSelectedTabPos() _____________________________________________________________

int WgGizmoTablist::GetSelectedTabPos() const
{
	if(m_pTabSelected == 0)
		return -1;

	int iPos = 0;
	WgTab *pTab = m_tabs.getFirst();
	while(pTab)
	{
		if(pTab == m_pTabSelected)
			return iPos;
		pTab = pTab->getNext();
		iPos++;
	}
	return -1;
}

//____ SetAlert() _____________________________________________________________

bool WgGizmoTablist::SetAlert( Sint32 id, bool bAlertOn )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		pTab->m_bAlert = bAlertOn;
		RequestRender();
		return true;
	}

	return false;
}

//____ GetAlert() _____________________________________________________________

bool WgGizmoTablist::GetAlert( Sint32 id )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		return pTab->m_bAlert;
	}

	return false;
}

//____ () ______________________________________________________________
bool WgGizmoTablist::ShowTab( Sint32 id, bool bVisible )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		if(pTab->m_bVisible != bVisible)
		{
			pTab->m_bVisible = bVisible;
			RequestRender();
		}
		return true;
	}

	return false;
}

//____ GetTabCount() ______________________________________________________________

Uint32 WgGizmoTablist::GetTabCount( ) const
{
	Uint32 n = 0;

	for( WgTab * pTab = m_tabs.getFirst(); pTab; pTab = pTab->getNext() )
		n++;

	return n;
}

//____ GetTabWidth() ______________________________________________________________

Uint32 WgGizmoTablist::GetTabWidth( Sint32 id )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		return pTab->m_width;
	}

	return 0;
}

//____ HasTab() ______________________________________________________________

bool WgGizmoTablist::HasTab( Sint32 id )
{
	return FindTab(id) != 0;
}

//____ LockTabContent() ______________________________________________________________

WgItemRow* WgGizmoTablist::LockTabContent( Sint32 id )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		if( pTab->m_bLockedContent )
			return 0;

		if( !pTab->m_pItemRow )
			pTab->m_pItemRow = new WgItemRow();

		pTab->m_bLockedContent = true;

		return pTab->m_pItemRow;
	}

	return 0;
}

//____ UnlockTabContent() ______________________________________________________________

void WgGizmoTablist::UnlockTabContent( Sint32 id )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		pTab->m_bLockedContent = false;
		ResizeTab( pTab );
		RequestRender();
	}
}

//____ GetFirstTab() ______________________________________________________________
WgTab* WgGizmoTablist::GetFirstTab()
{
	return m_tabs.getFirst();
}

WgTab* WgGizmoTablist::GetLastTab()
{
	return m_tabs.getLast();
}

WgTab* WgGizmoTablist::GetFirstVisibleTab()
{
	WgTab* pTab = GetFirstTab();
	while(pTab && !pTab->m_bVisible)
		pTab = pTab->getNext();
	return pTab;
}

WgTab* WgGizmoTablist::GetLastVisibleTab()
{
	WgTab* pTab = GetLastTab();
	while(pTab && !pTab->m_bVisible)
		pTab = pTab->getPrev();
	return pTab;
}

//____ FindTab() ______________________________________________________________

WgTab* WgGizmoTablist::FindTab( Sint32 id )
{
	WgTab * pTab = m_tabs.getFirst();

	while( pTab )
	{
		if( pTab->m_id == id )
			return pTab;
		pTab = pTab->getNext();
	}
	return 0;
}



//____ GetTabSource() _________________________________________________________

WgBlockSetPtr WgGizmoTablist::GetTabSource( WgTab * pTab )
{
	if( pTab->GetSource() )
	{
		return pTab->GetSource();
	}

	if(pTab == GetFirstVisibleTab())
		return m_sources[SourceTypeLeft];

	if(pTab == GetLastVisibleTab())
		return m_sources[SourceTypeRight];

	return m_sources[SourceTypeMiddle];
}

//____ GetTabMode() __________________________________________________________

WgMode	WgGizmoTablist::GetTabMode(const WgTab& tab)
{
	if( !m_bEnabled )
		return WG_MODE_DISABLED;
	else if( m_bAlertOn && tab.m_bAlert )
		return WG_MODE_SPECIAL;
//	else if( m_pTabMarked == &tab && m_pTabSelected == &tab )
//		cord = src.m_srcSelectedMarked;
	else if( m_pTabSelected == &tab )
		return WG_MODE_SELECTED;
	else if( m_pTabMarked == &tab )
		return WG_MODE_MARKED;
	else
		return WG_MODE_NORMAL;

/*
	if( !m_bEnabled )
		return src.m_pTabBlock->GetBlock( WG_MODE_DISABLED );
	else if( m_bAlertOn && tab.m_bAlert )
		return src.m_pTabBlock->GetBlock( WG_MODE_SPECIAL );
//	else if( m_pTabMarked == &tab && m_pTabSelected == &tab )
//		cord = src.m_srcSelectedMarked;
	else if( m_pTabSelected == &tab )
		return src.m_pTabBlock->GetBlock( WG_MODE_SELECTED );
	else if( m_pTabMarked == &tab )
		return src.m_pTabBlock->GetBlock( WG_MODE_MARKED );
	else
		return src.m_pTabBlock->GetBlock( WG_MODE_NORMAL );
*/
}

//____ ResizeTab() ____________________________________________________________

bool WgGizmoTablist::ResizeTab( WgTab * pTab )
{
	bool retVal = false;

	int width = pTab->m_text.width();

	if( pTab->m_pItemRow )
	{
		width += pTab->m_pItemRow->Width();
		if( pTab->m_pItemRow->NbItems() > 0 )
			width += pTab->m_pItemRow->ItemSpacing();
	}

	WgBlockSetPtr pSrc = GetTabSource(pTab);

	width += pSrc->GetContentBorders().GetWidth();

	if( width < m_minTabWidth )
		width = m_minTabWidth;

	if( width < pSrc->GetMinWidth() )
		width = pSrc->GetMinWidth();

	if( m_tabWidthMode == TabWidthModeUnified )
	{
		if(width < m_widestTab)
			width = m_widestTab;
		else if(width > m_widestTab)
		{
			m_widestTab = width;
			for( WgTab *pTab2 = m_tabs.getFirst(); pTab2; pTab2 = pTab2->getNext() )
				pTab2->m_width = m_widestTab;
			retVal = true;
		}
	}

	if( width != pTab->m_width )
	{
		pTab->m_width = width;
		retVal = true;
	}

	return retVal;
}

//____ ResizeAllTabs() ________________________________________________________

bool WgGizmoTablist::ResizeAllTabs()
{
	bool	retval = false;

	WgTab * pTab = m_tabs.getFirst();
	while( pTab )
	{
		if( ResizeTab(pTab) )
			retval = true;
		pTab = pTab->getNext();
	}

	return retval;
}

//____ Pos2Tab() ______________________________________________________________

WgTab * WgGizmoTablist::Pos2Tab( Sint32 x, Sint32 y )
{
	if( x < 0 || y < 0 )
		return 0;

	float scaleFactor = CalcTabScaleFactor();

	bool bMovingUp = true;
	WgTab * pHit = NULL;
	WgTab * pTab = m_tabs.getFirst();
	WgSize	sz = Size();

	while( pTab )
	{
		if(pTab->m_bVisible)
		{
			Uint32 w = (Uint32) (pTab->m_width*scaleFactor + 0.5f);

			if(m_tabWidthMode == TabWidthModeExpand && pTab->getNext() == 0)
				w = sz.w - x;

			if(pTab == m_pTabSelected)
				bMovingUp = false;

			WgBlockSetPtr pSrc = GetTabSource(pTab);
			bool	bHit = false;

			// Tab is hit if position is on a non-transparent (alpha != 0) pixel of the block
			// or inside tabs text-area. 

			if( ((unsigned) x) > pSrc->GetContentBorders().left && ((unsigned) x) < w - pSrc->GetContentBorders().right &&
				((unsigned) y) > pSrc->GetContentBorders().top && y < sz.h - pSrc->GetContentBorders().bottom )
				bHit = true;
			else
				bHit = WgUtil::MarkTestBlock( x, y, pSrc->GetBlock( GetTabMode(*pTab) ), WgRect(0,0,w,sz.h));

			if( bHit )
			{
				// Handle overlap
				if(bMovingUp)
					pHit = pTab;
				else
					return pTab;
			}

			x -= w - m_overlap;
		}
		pTab = pTab->getNext();
	}

	return pHit;
}

//____ OnUpdate() ________________________________________________________

void WgGizmoTablist::OnUpdate( const WgUpdateInfo& _updateInfo )
{
	m_alertModeCnt -= _updateInfo.msDiff;
	if( m_alertModeCnt <= 0 )
	{
		m_bAlertOn = !m_bAlertOn;
		m_alertModeCnt = m_alertRate;		// This is right, we want it to stay in the new mode at least one frame.

		// Check if we have to render something...

		WgTab * pTab = m_tabs.getFirst();
		while( pTab )
		{
			if( pTab->m_bAlert && pTab->m_bVisible )
			{
				RequestRender();			// Somewhat stupid to render all tabs though...
				break;
			}
			pTab = pTab->getNext();
		}
	}
}

//____ CalcTabScaleFactor() ___________________________________________________

float WgGizmoTablist::CalcTabScaleFactor()
{
	float scaleFactor = 1.f;

	int xLen = 0;
	int nTabs = 0;
	WgTab * pTab = m_tabs.getFirst();
	while( pTab )
	{
		if(pTab->m_bVisible)
		{
			xLen += pTab->m_width;
			nTabs++;
		}
		pTab = pTab->getNext();
	}

	int maxLen = Size().w + m_overlap*(nTabs-1);
	if( m_tabWidthMode == TabWidthModeExpand || xLen > maxLen )
	{
		scaleFactor =  maxLen / (float) xLen;
	}

	return scaleFactor;
}


//____ OnRender() ________________________________________________________

void WgGizmoTablist::OnRender( WgGfxDevice * pDevice, const WgRect& window, const WgRect& clip, Uint8 _layer )
{
	Uint32	selectedX	= 0xFFFFFFFF;			// X-offset for selected tab.

	float scaleFactor = CalcTabScaleFactor();

	// Render all normal tabs

	// Render all tabs to the left of the selected tab first


	Uint32 xOfs = window.x;
	int width;
	WgTab * pTab = m_tabs.getFirst();
	while( pTab )
	{
		if(pTab->m_bVisible)
		{
			width = (int)(pTab->m_width*scaleFactor + 0.5f);
			// expand last tab to window edge
			if(m_tabWidthMode == TabWidthModeExpand && pTab == GetLastVisibleTab())
				width = window.x + window.w - xOfs;

			WgRect r( xOfs, window.y, width, window.h );

//			Uint32 yOfs = window.y + window.h - GetTabSource(pTab).m_srcH;

			if( pTab == m_pTabSelected )
				selectedX = xOfs;
			else if( selectedX == 0xFFFFFFFF )
				RenderTab( pDevice, *pTab, r, clip );

			xOfs += width - m_overlap;
		}
		pTab = pTab->getNext();
	}

	if( m_pTabSelected )
	{
		// Now render all tabs to the right of the selected tab
		WgTab * pTab = m_tabs.getLast();
		while( pTab )
		{
			if(pTab->m_bVisible)
			{
				width = (int)(pTab->m_width*scaleFactor + 0.5f);
				// expand last tab to window edge
				if(m_tabWidthMode == TabWidthModeExpand && pTab == GetLastVisibleTab())
					width = window.x + window.w - xOfs;

				xOfs -= width - m_overlap;

				WgRect r( xOfs, window.y, width, window.h );

				RenderTab( pDevice, *pTab, r, clip );
				if( pTab == m_pTabSelected )
					break;
			}
			
			pTab = pTab->getPrev();
		}
	}

}

//____ RenderTab() _______________________________________________________

void WgGizmoTablist::RenderTab( WgGfxDevice * pDevice, WgTab& tab, WgRect dest, const WgRect& clip )
{
	assert( !tab.m_bLockedContent );

	WgBlockSetPtr pSrc = GetTabSource(&tab);

	WgMode	mode = GetTabMode(tab);

	WgBlock block = pSrc->GetBlock(mode);

	pDevice->ClipBlitBlock( clip, block, dest );

	WgRect	r = dest;
	r.Shrink( pSrc->GetContentBorders() );

	if( tab.m_pItemRow )
	{
		((WgItem*)(tab.m_pItemRow))->Render( r, clip );
		// move ahead x offset and reduce width for text part
		int rowWidth = tab.m_pItemRow->Width();
		if( tab.m_pItemRow->NbItems() > 0 )
			rowWidth += tab.m_pItemRow->ItemSpacing();
		r.x += rowWidth;
		r.w -= rowWidth;
	}

	//WgRect	text( xOfs + m_contentRect.x, yOfs + m_contentRect.y, m_contentRect.w + (tab.m_width - m_srcH), m_contentRect.h );

	tab.m_text.setAlignment(m_textOrigo);
	/*if( m_pTabSelected == &tab )
		tab.m_text.setFonts(m_pSelectedFonts);
	else
		tab.m_text.setFonts(m_pFonts);*/

	WgRect clip2( clip, r );
	tab.m_text.setMode(mode);
	pDevice->PrintText( clip2, &tab.m_text, r );
}

//____ OnRefresh() _______________________________________________________

void WgGizmoTablist::OnRefresh( void )
{
	ResizeAllTabs();
	RequestRender();
}

//____ OnAction() _____________________________________________________________

void WgGizmoTablist::OnAction( WgEmitter * pEmitter, WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
	switch( action )
	{
		case WgInput::BUTTON_PRESS:
		{
			WgCord pos = Abs2local( WgCord(info.x, info.y) );

			Sint32	x = pos.x;
			Sint32	y = pos.y;

			WgTab * pTab = Pos2Tab( x, y );
			if( pTab && pTab != m_pTabSelected )
			{
				if( button_key == 1 )
					SelectTab(pTab->m_id);

				pEmitter->Emit( WgSignal::TabPressed(), pTab->m_id );
			}
		}
		break;

		case WgInput::POINTER_OVER:
		{
			WgCord pos = Abs2local( WgCord(info.x, info.y) );

			Sint32	x = pos.x;
			Sint32	y = pos.y;

			WgTab * pTab = Pos2Tab( x, y );
			if( pTab != m_pTabMarked )
			{
				m_pTabMarked = pTab;
				RequestRender();
			}
		}
		break;

		case WgInput::POINTER_EXIT:
			if( m_pTabMarked )
			{
				m_pTabMarked = 0;
				RequestRender();
			}
		break;

        default:
            break;
	}
}

//____ OnCloneContent() _______________________________________________________

void WgGizmoTablist::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoTablist * pOrg = (WgGizmoTablist *) _pOrg;

//	WgChain<WgTab>	m_tabs;

	m_pProp			= pOrg->m_pProp;

	m_textOrigo		= pOrg->m_textOrigo;
	for(int i = 0; i < m_nSourceTypes; i++)
		m_sources[i] = pOrg->m_sources[i];

	m_pTabSelected	= 0;
	m_pTabMarked	= 0;

	m_bAlertOn		= pOrg->m_bAlertOn;
	m_alertModeCnt	= pOrg->m_alertModeCnt;
	m_alertRate		= pOrg->m_alertRate;

	m_minTabWidth	= pOrg->m_minTabWidth;
	m_overlap		= pOrg->m_overlap;

	m_tabWidthMode	= pOrg->m_tabWidthMode;
	m_widestTab		= pOrg->m_widestTab;
}


//____ OnMarkTest() ________________________________________

bool WgGizmoTablist::OnMarkTest( const WgCord& ofs )
{
	return Pos2Tab(ofs.x, ofs.y) != NULL;
}
