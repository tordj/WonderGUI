#include <assert.h>
#include <memory.h>
#include <algorithm>

#include <wg_gizmo_tablist.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>
#include <wg_util.h>

#include <wg_item_row.h>


static const char	c_gizmoType[] = {"TordJ/TabList"};

//____ WgTab::Constructor __________________________________________________________

WgTab::WgTab( int id )
{
	m_id = id;
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

	m_bTabOpaqueForMouse = false;
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
	return c_gizmoType;
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

		WgTab * pTab = m_tabs.First();
		while( pTab )
		{
			pTab->m_text.setProperties(m_pProp);
			pTab = pTab->Next();
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

bool WgGizmoTablist::AddTab( int id, const WgCharSeq& text, int position, const WgBlockSetPtr& pGfx )
{
	if( FindTab(id) )
		return false;					// We already have a tab with this ID...

	if( position == -1 )
		position = INT_MAX;

	WgTab * pTab = new WgTab(id);

	pTab->m_text.setText(text);
	pTab->m_text.setProperties(m_pProp);
	pTab->SetSource( pGfx );
	ResizeTab(pTab);

	WgTab * pPos = m_tabs.Get(position);
	if( pPos )
		pTab->MoveBefore(pPos);
	else
		m_tabs.PushBack(pTab);
	RequestRender();
	return true;
}

//____ RemoveTab() ____________________________________________________________

bool WgGizmoTablist::RemoveTab( int id )
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

	WgTab *pTab = m_tabs.First();
	while(pTab)
	{
		WgTab *pNext = pTab->Next();
		delete pTab;
		pTab = pNext;
	}

	RequestRender();
}

//____ MoveTab() ______________________________________________________________

bool WgGizmoTablist::MoveTab( int id, int position )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		WgTab * pPos = m_tabs.Get(position);
		if( pPos )
			pTab->MoveBefore(pPos);
		else
			m_tabs.PushBack(pTab);
		RequestRender();
		return true;
	}
	return false;
}

//____ SetTabId() ___________________________________________________________

bool WgGizmoTablist::SetTabId( int id, int newId )
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

int WgGizmoTablist::GetTabId(int position)
{
	WgTab *pTab = m_tabs.First();
	if( pTab && position )
	{
		position--;
		pTab = pTab->Next();
	}
	if(pTab)
		return pTab->m_id;
	return -1;
}

//____ SetTabText() ___________________________________________________________

bool WgGizmoTablist::SetTabText( int id, const WgCharSeq& text )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		pTab->m_text.setText( text );
		ResizeTab(pTab);
		RequestRender();
		return true;
	}
	return false;
}

//____ SetTabTextColor() ____________________________________________________________
bool WgGizmoTablist::SetTabTextColor( int id, WgColor col )
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

bool WgGizmoTablist::SelectTab( int id )
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

		Emit( WgSignal::TabSelected(), pTab->m_id );
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
	WgTab *pTab = m_tabs.First();
	while(pTab)
	{
		if(pTab == m_pTabSelected)
			return iPos;
		pTab = pTab->Next();
		iPos++;
	}
	return -1;
}

//____ SetAlert() _____________________________________________________________

bool WgGizmoTablist::SetAlert( int id, bool bAlertOn )
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

bool WgGizmoTablist::GetAlert( int id )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		return pTab->m_bAlert;
	}

	return false;
}

//____ () ______________________________________________________________
bool WgGizmoTablist::ShowTab( int id, bool bVisible )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		if(pTab->m_bVisible != bVisible)
		{
			if(!bVisible)
			{
				if( m_pTabSelected == pTab )
					m_pTabSelected = 0;
				if( m_pTabMarked == pTab )
					m_pTabMarked = 0;
			}

			pTab->m_bVisible = bVisible;
			RequestRender();
		}
		return true;
	}

	return false;
}

//____ GetTabCount() ______________________________________________________________

int WgGizmoTablist::GetTabCount( ) const
{
	int n = 0;

	for( WgTab * pTab = m_tabs.First(); pTab; pTab = pTab->Next() )
		n++;

	return n;
}

//____ GetTabWidth() ______________________________________________________________

Uint32 WgGizmoTablist::GetTabWidth( int id )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		return pTab->m_width;
	}

	return 0;
}

//____ HasTab() ______________________________________________________________

bool WgGizmoTablist::HasTab( int id )
{
	return FindTab(id) != 0;
}

//____ LockTabContent() ______________________________________________________________

WgItemRow* WgGizmoTablist::LockTabContent( int id )
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

void WgGizmoTablist::UnlockTabContent( int id )
{
	WgTab * pTab = FindTab(id);
	if( pTab )
	{
		pTab->m_bLockedContent = false;
		ResizeTab( pTab );
		RequestRender();
	}
}

//////////////////////////////////////////////////////////////////////////
WgTab* WgGizmoTablist::GetSelectedTab()
{
	return m_pTabSelected;
}

//____ GetFirstTab() ______________________________________________________________
WgTab* WgGizmoTablist::GetFirstTab()
{
	return m_tabs.First();
}

WgTab* WgGizmoTablist::GetLastTab()
{
	return m_tabs.Last();
}

WgTab* WgGizmoTablist::GetFirstVisibleTab()
{
	WgTab* pTab = GetFirstTab();
	while(pTab && !pTab->m_bVisible)
		pTab = pTab->Next();
	return pTab;
}

WgTab* WgGizmoTablist::GetLastVisibleTab()
{
	WgTab* pTab = GetLastTab();
	while(pTab && !pTab->m_bVisible)
		pTab = pTab->Prev();
	return pTab;
}

//____ FindTab() ______________________________________________________________

WgTab* WgGizmoTablist::FindTab( int id )
{
	WgTab * pTab = m_tabs.First();

	while( pTab )
	{
		if( pTab->m_id == id )
			return pTab;
		pTab = pTab->Next();
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

	width += pSrc->GetContentBorders().width();

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
			for( WgTab *pTab2 = m_tabs.First(); pTab2; pTab2 = pTab2->Next() )
				pTab2->m_width = m_widestTab;
			retVal = true;
		}
	}

	if( width != (int) pTab->m_width )
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

	WgTab * pTab = m_tabs.First();
	while( pTab )
	{
		if( ResizeTab(pTab) )
			retval = true;
		pTab = pTab->Next();
	}

	return retval;
}

//____ Pos2Tab() ______________________________________________________________

WgTab * WgGizmoTablist::Pos2Tab( int x, int y )
{
	if( x < 0 || y < 0 )
		return 0;

	float scaleFactor = CalcTabScaleFactor();

	bool bMovingUp = true;
	WgTab * pHit = NULL;
	WgTab * pTab = m_tabs.First();
	WgSize	sz = Size();

	while( pTab )
	{
		if(pTab->m_bVisible)
		{
			Uint32 w = (Uint32) (pTab->m_width*scaleFactor + 0.5f);

			if(m_tabWidthMode == TabWidthModeExpand && pTab->Next() == 0)
				w = sz.w - x;

			if(pTab == m_pTabSelected)
				bMovingUp = false;

			WgBlockSetPtr pSrc = GetTabSource(pTab);
			bool	bHit = false;

			// Tab is hit if position is on a non-transparent (alpha != 0) pixel of the block
			// or inside tabs text-area.

			if( m_bTabOpaqueForMouse && x >= 0 && y >= 0 && x < (int) w && y < sz.h )
				bHit = true;
			else if( ((unsigned) x) > pSrc->GetContentBorders().left && ((unsigned) x) < w - pSrc->GetContentBorders().right &&
				((unsigned) y) > pSrc->GetContentBorders().top && y < sz.h - pSrc->GetContentBorders().bottom )
				bHit = true;
			else
				bHit = WgUtil::MarkTestBlock( WgCord(x, y), pSrc->GetBlock( GetTabMode(*pTab) ), WgRect(0,0,w,sz.h));

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
		pTab = pTab->Next();
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

		WgTab * pTab = m_tabs.First();
		while( pTab )
		{
			if( pTab->m_bAlert && pTab->m_bVisible )
			{
				RequestRender();			// Somewhat stupid to render all tabs though...
				break;
			}
			pTab = pTab->Next();
		}
	}
}

//____ CalcTabScaleFactor() ___________________________________________________

float WgGizmoTablist::CalcTabScaleFactor()
{
	float scaleFactor = 1.f;

	int xLen = 0;
	int nTabs = 0;
	WgTab * pTab = m_tabs.First();
	while( pTab )
	{
		if(pTab->m_bVisible)
		{
			xLen += pTab->m_width;
			nTabs++;
		}
		pTab = pTab->Next();
	}

	if(m_tabWidthMode == TabWidthModeExpand2)
	{
		m_overlap = -(Size().w - xLen) / (nTabs - 1);
	}

	int maxLen = Size().w + m_overlap*(nTabs-1);
	if( m_tabWidthMode == TabWidthModeExpand || xLen > maxLen )
	{
		scaleFactor =  maxLen / (float) xLen;
	}

	return scaleFactor;
}


//____ OnRender() ________________________________________________________

void WgGizmoTablist::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& clip, Uint8 _layer )
{
	Uint32	selectedX	= 0xFFFFFFFF;			// X-offset for selected tab.

	float scaleFactor = CalcTabScaleFactor();

	// Render all normal tabs

	// Render all tabs to the left of the selected tab first


	int xOfs = (int)_canvas.x;
	int width;
	WgTab * pTab = m_tabs.First();
	while( pTab )
	{
		if(pTab->m_bVisible)
		{
			width = (int)(pTab->m_width*scaleFactor + 0.5f);
			// expand last tab to canvas edge
			if(m_tabWidthMode == TabWidthModeExpand && pTab == GetLastVisibleTab())
				width = _canvas.x + _canvas.w - xOfs;

			WgRect r( xOfs, _canvas.y, width, _canvas.h );

//			Uint32 yOfs = window.y + window.h - GetTabSource(pTab).m_srcH;

			if( pTab == m_pTabSelected )
				selectedX = xOfs;
			else if( selectedX == 0xFFFFFFFF )
				RenderTab( pDevice, *pTab, r, clip );

			xOfs += width - m_overlap;
		}
		pTab = pTab->Next();
	}

	if( m_pTabSelected && m_pTabSelected->m_bVisible )
	{
		// Now render all tabs to the right of the selected tab
		WgTab * pTab = m_tabs.Last();
		while( pTab )
		{
			if(pTab->m_bVisible)
			{
				// expand last tab to window edge
				if(m_tabWidthMode == TabWidthModeExpand && pTab == GetLastVisibleTab())
				{
					xOfs -= width - m_overlap;
				}
				else
				{
					width = (int)(pTab->m_width*scaleFactor + 0.5f);
					xOfs -= width - m_overlap;
				}


				WgRect r( xOfs, _canvas.y, width, _canvas.h );

				RenderTab( pDevice, *pTab, r, clip );
				if( pTab == m_pTabSelected )
					break;
			}

			pTab = pTab->Prev();
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
	r.shrink( pSrc->GetContentBorders() );

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

void WgGizmoTablist::OnAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
	switch( action )
	{
		case WgInput::BUTTON_PRESS:
		{
			WgCord pos = Abs2local( WgCord(info.x, info.y) );

			int	x = pos.x;
			int	y = pos.y;

			WgTab * pTab = Pos2Tab( x, y );
			if( pTab && pTab != m_pTabSelected )
			{
				if( button_key == 1 )
					SelectTab(pTab->m_id);

				Emit( WgSignal::TabPressed(), pTab->m_id );
			}
		}
		break;

		case WgInput::POINTER_OVER:
		{
			WgCord pos = Abs2local( WgCord(info.x, info.y) );

			int	x = pos.x;
			int	y = pos.y;

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

	m_tabWidthMode			= pOrg->m_tabWidthMode;
	m_widestTab				= pOrg->m_widestTab;
	m_bTabOpaqueForMouse	= pOrg->m_bTabOpaqueForMouse;

}


//____ OnAlphaTest() ________________________________________

bool WgGizmoTablist::OnAlphaTest( const WgCord& ofs )
{
	return Pos2Tab(ofs.x, ofs.y) != NULL;
}
