#include <assert.h>
#include <memory.h>
#include <limits.h>
#include <algorithm>

#include <wg_gizmo_tablist.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>
#include <wg_util.h>

#include <wg_item_row.h>


static const char	c_gizmoType[] = {"TabList"};

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
	m_maxOverlap	= 0;

	m_tabWidthMode		= INDIVIDUAL_WIDTH;
	m_tabExpandMode		= NO_EXPAND;
	m_tabCompressMode	= NO_COMPRESS;

	m_textOrigo		= WgOrigo::midCenter();

	m_bTabOpaqueForMouse = false;
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

		WgTab * pTab = m_tabs.First();
		while( pTab )
		{
			pTab->m_text.setProperties(m_pProp);
			pTab = pTab->Next();
		}

		//

		ResizeTabs();
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
	ResizeTabs();
	RequestRender();
}

//____ SetTabExpandMode() _____________________________________________________

void WgGizmoTablist::SetTabExpandMode( TabExpandMode mode )
{
	m_tabExpandMode = mode;
	ResizeTabs();
	RequestRender();
}

//____ SetTabCompressMode() ___________________________________________________

void WgGizmoTablist::SetTabCompressMode( TabCompressMode mode )
{
	m_tabCompressMode = mode;
	ResizeTabs();
	RequestRender();
}

//____ SetTabWidthMode() ______________________________________________________

void WgGizmoTablist::SetTabWidthMode( TabWidthMode mode )
{
	m_tabWidthMode = mode;
	ResizeTabs();
	RequestRender();
}


//____ SetOverlap() ___________________________________________________________

void WgGizmoTablist::SetOverlap( int overlap )
{
	m_overlap = overlap;
	ResizeTabs();
	RequestRender();
}

//____ SetMaxOverlap() ________________________________________________________

void WgGizmoTablist::SetMaxOverlap( int maxOverlap )
{
	m_maxOverlap = maxOverlap;

	if( m_tabCompressMode == OVERLAP_TABS )
	{
		ResizeTabs();
		RequestRender();
	}
}

//____ SetMinTabWidth() _______________________________________________________

void WgGizmoTablist::SetMinTabWidth( int minWidth )
{
	m_minTabWidth	= minWidth;

	ResizeTabs();
	RequestRender();
}

//____ SetAlertRate() _________________________________________________________

void WgGizmoTablist::SetAlertRate( int millisec )
{
	m_alertRate = millisec;
}

//____ SetTextOrigo() _________________________________________________________

void WgGizmoTablist::SetTextOrigo( WgOrigo origo )
{
	m_textOrigo = origo;
	RequestRender();
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

	WgTab * pPos = m_tabs.Get(position);
	if( pPos )
		pTab->MoveBefore(pPos);
	else
		m_tabs.PushBack(pTab);

	ResizeTabs();
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
		ResizeTabs();
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
		ResizeTabs();
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
		ResizeTabs();				// fonts have changed
		RequestRender();

		Emit( WgSignal::TabSelected(), pTab->m_id );
		return true;
	}

	ResizeTabs();		// fonts have changed
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

//____ ShowTabl() ______________________________________________________________

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
			ResizeTabs();
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
		ResizeTabs();
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

//____ GetLastTab() ______________________________________________________________

WgTab* WgGizmoTablist::GetLastTab()
{
	return m_tabs.Last();
}

//____ GetFirstVisibleTab() ___________________________________________________

WgTab* WgGizmoTablist::GetFirstVisibleTab()
{
	WgTab* pTab = GetFirstTab();
	while(pTab && !pTab->m_bVisible)
		pTab = pTab->Next();
	return pTab;
}

//____ GetLastVisibleTab() ____________________________________________________

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
}

//____ ResizeTabs() ___________________________________________________________

void WgGizmoTablist::ResizeTabs()
{
	if( m_tabs.Size() == 0 )
		return;

	// First we calculate and set the start width for all tabs

	switch( m_tabWidthMode )
	{
		case UNIFIED_WIDTH:
		{
			int widest = 0;
			WgTab * pTab = m_tabs.First();
			while( pTab )
			{
				int w = CalcTabsWantedWidth(pTab);
				if( w > widest )
					widest = w;
				pTab = pTab->Next();
			}

			pTab = m_tabs.First();
			while( pTab )
			{
				pTab->m_width = widest;
				pTab->m_advance = widest - m_overlap;
				pTab = pTab->Next();
			}

		}
			break;

		default:
			assert(0);					// Should never happen, but we will default to INDIVIDUAL_WIDTH if it does.
		case INDIVIDUAL_WIDTH:
		{
			WgTab * pTab = m_tabs.First();
			while( pTab )
			{
				pTab->m_width = CalcTabsWantedWidth(pTab);
				pTab->m_advance = pTab->m_width - m_overlap;
				pTab = pTab->Next();
			}
			break;
		}
	}

	// Count total width

	int totalWidth = 0;

	WgTab * pTab = m_tabs.First();

	for( int i = 0 ; i < m_tabs.Size()-1 ; i++ )
	{
		totalWidth += pTab->m_advance;				// Add advance for all but last tab.
		pTab = pTab->Next();
	}

	totalWidth += pTab->m_width;					// Add width for last tab.

	// Shrink or grow tabs and advance as applicable

	if( totalWidth < Size().w )
	{
		switch( m_tabExpandMode )
		{
			case GROW_TABS:
			{
				int goal = Size().w;
				int diff = goal - totalWidth;

				int combWidth = 0;							// Combined width of tabs, ignoring overlaps.
				WgTab * pTab = m_tabs.First();
				while( pTab )
				{
					combWidth += pTab->m_width;
					pTab = pTab->Next();
				}

				float	growFactor = diff / (float) combWidth;
				float	fractions = 0.f;

				pTab = m_tabs.First();
				while( pTab )
				{
					float inc = fractions + pTab->m_width * growFactor;			
					fractions = inc - (int) inc;
					pTab->m_width += (int) inc;
					pTab->m_advance += (int) inc;
					diff -= (int) inc;
					pTab = pTab->Next();
				}

				// Make sure last tab ends exactly on pixel.

				if( diff > 0 )
				{
					m_tabs.Last()->m_width += diff;
					m_tabs.Last()->m_advance += diff;
				}

				break;
			}
			case SPREAD_TABS:
			{
				int diff = Size().w - totalWidth;
				int nSpaces = m_tabs.Size()-1;

				float	incFactor = diff / (float) nSpaces;
				float	fractions = 0.f;

				WgTab * pTab = m_tabs.First();
				for( int i = 0 ; i < nSpaces ; i++ )
				{
					float inc = fractions + incFactor;			
					fractions = inc - (int) inc;
					pTab->m_advance += (int) inc;
					diff -= (int) inc;
					pTab = pTab->Next();
				}

				// Make sure last tab ends exactly on pixel, i.e
				// we use up all the diff.

				if( diff > 0 )
					pTab->m_advance += diff;

				break;
			}

			case UNIFY_TABS:
			{
				// Set widest tab as our unifiedWidth

				int unifiedWidth = 0;							// Combined width of tabs, ignoring overlaps.
				WgTab * pTab = m_tabs.First();
				while( pTab )
				{
					if( pTab->m_width > unifiedWidth )
						unifiedWidth = pTab->m_width;
					pTab = pTab->Next();
				}
				
				// Calculate number of pixels we want to expand in total

				int totalExpand = 0;
				pTab = m_tabs.First();
				while( pTab )
				{
					if( pTab->m_width < unifiedWidth )
						totalExpand += unifiedWidth - pTab->m_width;
					pTab = pTab->Next();
				}

				// translate to growFactor

				int goal = Size().w;
				int diff = goal - totalWidth;

				if( diff > totalExpand )
					diff = totalExpand;

				float	growFactor = diff / (float) totalExpand;
				float	fractions = 0.f;

				// 

				pTab = m_tabs.First();
				while( pTab )
				{
					if( pTab->m_width < unifiedWidth )
					{
						float inc = fractions + (unifiedWidth - pTab->m_width) * growFactor;			
						fractions = inc - (int) inc;
						pTab->m_width += (int) inc;
						pTab->m_advance += (int) inc;
						diff -= (int) inc;
					}
					pTab = pTab->Next();
				}

				// Make sure last tab ends exactly on pixel.
				// We do this by adding any leftover diff to
				// last tab that isn't of the unified width.

				if( diff > 0 )
				{
					pTab = m_tabs.Last();
					while( pTab )
					{
						if( pTab->m_width < unifiedWidth )
						{
							pTab->m_width += diff;
							pTab->m_advance += diff;
							break;
						}
						pTab = pTab->Prev();
					}
				}
				break;
			}

			default:
				assert(false);				// Should never happen, but we will default to NO_EXPAND if it does.
			case NO_EXPAND:
				break;
		}
	}
	else if( totalWidth > Size().w )
	{
		switch( m_tabCompressMode )
		{
			case SHRINK_TABS:
			{
				// We start out with all tabs at minimum width (m_minTabWidth) and add extra space
				// relative to their width beyond the minimum.

				int minTotalWidth = m_tabs.Size()*m_minTabWidth - (m_tabs.Size()-1)*m_overlap;
				int widthToShare = Size().w - minTotalWidth;

				if( widthToShare <= 0 )
				{
					// Just set all tabs to minimum width

					WgTab * pTab = m_tabs.First();
					while( pTab )
					{
						pTab->m_width = m_minTabWidth;
						pTab->m_advance = m_minTabWidth - m_overlap;
						pTab = pTab->Next();
					}
				}
				else
				{
					// Add widthToShare in relation to their current width.

					float	scaleFactor = widthToShare / (float) (totalWidth - minTotalWidth);
					float	fractions = 0.f;

					WgTab * pTab = m_tabs.First();
					while( pTab )
					{
						float width = fractions + m_minTabWidth + (pTab->m_width - m_minTabWidth)*scaleFactor;
						fractions = width - (int) width;

						pTab->m_width = (int) width;
						pTab->m_advance = ((int) width) - m_overlap;
						widthToShare -= (int) width;
						pTab = pTab->Next();
					}

					if( widthToShare > 0 )
					{
						m_tabs.Last()->m_width += widthToShare;
						m_tabs.Last()->m_advance += widthToShare;
					}
				}
				break;
			}
			case OVERLAP_TABS:
			{
				if( m_maxOverlap <= m_overlap )
					break;

				int diff = totalWidth - Size().w;
				int nSpaces = m_tabs.Size()-1;

				float	decFactor = diff / (float) nSpaces;
				float	fractions = 0.f;

				if( decFactor > (float) m_maxOverlap - m_overlap )
					decFactor = (float) m_maxOverlap - m_overlap;

				WgTab * pTab = m_tabs.First();
				for( int i = 0 ; i < nSpaces ; i++ )
				{
					float dec = fractions + decFactor;			
					fractions = dec - (int) dec;
					pTab->m_advance -= (int) dec;
					diff -= (int) dec;
					pTab = pTab->Next();
				}

				// Make sure last tab ends exactly on pixel, i.e
				// we use up all the diff.

				if( diff > 0 && decFactor < m_maxOverlap - m_overlap )
					pTab->m_advance -= diff;

				break;
			}
			default:
				assert(false);				// Should never happen, but we will default to NO_COMPRESS if it does.
			case NO_COMPRESS:
				break;
		}
	}
}

//____ CalcTabsWantedWidth() _______________________________________________________

int WgGizmoTablist::CalcTabsWantedWidth( WgTab * pTab )
{
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

	return width;
}


//____ Pos2Tab() ______________________________________________________________

WgTab * WgGizmoTablist::Pos2Tab( int x, int y )
{
	if( x < 0 || y < 0 )
		return 0;

	bool bMovingUp = true;
	WgTab * pHit = NULL;
	WgTab * pTab = m_tabs.First();
	WgSize	sz = Size();

	while( pTab )
	{
		if(pTab->m_bVisible)
		{
			Uint32 w = pTab->m_width;

//			if(pTab == m_pTabSelected)
//				bMovingUp = false;

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
/*				// Handle overlap
				if(bMovingUp)
					pHit = pTab;
				else
					return pTab;
*/

				if( pTab == m_pTabSelected )
					return pTab;
				else
					pHit = pTab;
			}

			x -= pTab->m_advance;
		}
		pTab = pTab->Next();
	}

	return pHit;
}

//____ _onUpdate() ________________________________________________________

void WgGizmoTablist::_onUpdate( const WgUpdateInfo& _updateInfo )
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

//____ _onRender() ________________________________________________________

void WgGizmoTablist::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& clip, Uint8 _layer )
{
	Uint32	selectedX	= INT_MAX;			// X-offset for selected tab.

	// Render all normal tabs first

	int xOfs = (int)_canvas.x;
	WgTab * pTab = m_tabs.First();
	while( pTab )
	{
		if(pTab->m_bVisible)
		{
			WgRect r( xOfs, _canvas.y, pTab->m_width, _canvas.h );

			if( pTab == m_pTabSelected )
				selectedX = xOfs;
			else
				RenderTab( pDevice, *pTab, r, clip );

			xOfs += pTab->m_advance;
		}
		pTab = pTab->Next();
	}

	// Render selected tab last, makes sure it comes out ontop.

	if( m_pTabSelected && m_pTabSelected->m_bVisible )
	{
		WgRect r( selectedX, _canvas.y, m_pTabSelected->m_width, _canvas.h );
		RenderTab( pDevice, *m_pTabSelected, r, clip );
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

	tab.m_text.setAlignment(m_textOrigo);

	WgRect clip2( clip, r );
	tab.m_text.setMode(mode);
	pDevice->PrintText( clip2, &tab.m_text, r );
}

//____ _onRefresh() ____________________________________________________________

void WgGizmoTablist::_onRefresh( void )
{
	ResizeTabs();
	RequestRender();
}

//____ _onNewSize() ____________________________________________________________

void WgGizmoTablist::_onNewSize( const WgSize& size )
{
	ResizeTabs();
}



//____ _onAction() _____________________________________________________________

void WgGizmoTablist::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
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

//____ _onCloneContent() _______________________________________________________

void WgGizmoTablist::_onCloneContent( const WgGizmo * _pOrg )
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
	m_maxOverlap	= pOrg->m_maxOverlap;

	m_tabWidthMode			= pOrg->m_tabWidthMode;
	m_tabExpandMode			= pOrg->m_tabExpandMode;
	m_tabCompressMode		= pOrg->m_tabCompressMode;
	m_widestTab				= pOrg->m_widestTab;
	m_bTabOpaqueForMouse	= pOrg->m_bTabOpaqueForMouse;

}


//____ _onAlphaTest() ________________________________________

bool WgGizmoTablist::_onAlphaTest( const WgCord& ofs )
{
	return Pos2Tab(ofs.x, ofs.y) != NULL;
}
