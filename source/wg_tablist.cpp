#include <assert.h>
#include <memory.h>
#include <limits.h>
#include <algorithm>

#include <wg_tablist.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>
#include <wg_util.h>
#include <wg_eventhandler.h>



static const char	c_widgetType[] = {"Tablist"};

//____ WgTab::Constructor __________________________________________________________

WgTab::WgTab( int id )
{
	m_id = id;
	m_bAlert = false;
	m_width = 1;
	m_text.setAlignment(WG_CENTER);
	m_text.SetWrap(false);
	m_bVisible = true;
}

//____ WgTab::Destructor __________________________________________________________

WgTab::~WgTab()
{
}

//____________________________________________________________________________
//
//	WgTablist
//____________________________________________________________________________


//____ WgTablist() _________________________________________________________________

WgTablist::WgTablist()
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

	m_textAlignment		= WG_CENTER;

	m_bTabOpaqueForMouse = false;
	m_widestTab		= 0;
}

//____ ~WgTablist() _________________________________________________________

WgTablist::~WgTablist( void )
{
}


//____ Type() _________________________________________________________________

const char * WgTablist::Type( void ) const
{
	return GetClass();
}

//____ GetClass() _____________________________________________________________

const char * WgTablist::GetClass( void )
{
	return c_widgetType;
}

//____ SetTextProperties() ____________________________________________________

void WgTablist::SetTextProperties( const WgTextpropPtr& pProp )
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

		_resizeTabs();
		_requestRender();
	}

}

//____ SetSource() ____________________________________________________________

void WgTablist::SetSource( WgBlocksetPtr pBlockSet, SourceType type )
{
	if(type == SourceTypeAll)
	{
		SetSource( pBlockSet, SourceTypeLeft );
		SetSource( pBlockSet, SourceTypeMiddle );
		SetSource( pBlockSet, SourceTypeRight );
		return;
	}

	m_sources[type] = pBlockSet;
	_resizeTabs();
	_requestRender();
}

//____ SetTabExpandMode() _____________________________________________________

void WgTablist::SetTabExpandMode( TabExpandMode mode )
{
	m_tabExpandMode = mode;
	_resizeTabs();
	_requestRender();
}

//____ SetTabCompressMode() ___________________________________________________

void WgTablist::SetTabCompressMode( TabCompressMode mode )
{
	m_tabCompressMode = mode;
	_resizeTabs();
	_requestRender();
}

//____ SetTabWidthMode() ______________________________________________________

void WgTablist::SetTabWidthMode( TabWidthMode mode )
{
	m_tabWidthMode = mode;
	_resizeTabs();
	_requestRender();
}


//____ SetOverlap() ___________________________________________________________

void WgTablist::SetOverlap( int overlap )
{
	m_overlap = overlap;
	_resizeTabs();
	_requestRender();
}

//____ SetMaxOverlap() ________________________________________________________

void WgTablist::SetMaxOverlap( int maxOverlap )
{
	m_maxOverlap = maxOverlap;

	if( m_tabCompressMode == OVERLAP_TABS )
	{
		_resizeTabs();
		_requestRender();
	}
}

//____ SetMinTabWidth() _______________________________________________________

void WgTablist::SetMinTabWidth( int minWidth )
{
	m_minTabWidth	= minWidth;

	_resizeTabs();
	_requestRender();
}

//____ SetAlertRate() _________________________________________________________

void WgTablist::SetAlertRate( int millisec )
{
	m_alertRate = millisec;
}

//____ SetTextAlignment() _________________________________________________________

void WgTablist::SetTextAlignment( WgOrigo alignment )
{
	m_textAlignment = alignment;
	_requestRender();
}

//____ AddTab() _______________________________________________________________

bool WgTablist::AddTab( int id, const WgCharSeq& text, int position, const WgBlocksetPtr& pGfx )
{
	if( _findTab(id) )
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

	_resizeTabs();
	_requestRender();
	return true;
}

//____ RemoveTab() ____________________________________________________________

bool WgTablist::RemoveTab( int id )
{
	WgTab * pTab = _findTab(id);
	if( pTab )
	{
		if( m_pTabSelected == pTab )
			m_pTabSelected = 0;
		if( m_pTabMarked == pTab )
			m_pTabMarked = 0;

		delete pTab;
		_resizeTabs();
		_requestRender();
		return true;
	}
	return false;
}

//____ RemoveAllTabs() ______________________________________________________________

void WgTablist::RemoveAllTabs()
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

	_requestRender();
}

//____ MoveTab() ______________________________________________________________

bool WgTablist::MoveTab( int id, int position )
{
	WgTab * pTab = _findTab(id);
	if( pTab )
	{
		WgTab * pPos = m_tabs.Get(position);
		if( pPos )
			pTab->MoveBefore(pPos);
		else
			m_tabs.PushBack(pTab);
		_requestRender();
		return true;
	}
	return false;
}

//____ SetTabId() ___________________________________________________________

bool WgTablist::SetTabId( int id, int newId )
{
	WgTab * pTab = _findTab(id);
	if( pTab )
	{
		if( !_findTab( newId ) )
		{
			pTab->m_id = newId;
		}
	}
	return false;
}

//____ GetTabId() ___________________________________________________________

int WgTablist::GetTabId(int position) const
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

bool WgTablist::SetTabText( int id, const WgCharSeq& text )
{
	WgTab * pTab = _findTab(id);
	if( pTab )
	{
		pTab->m_text.setText( text );
		_resizeTabs();
		_requestRender();
		return true;
	}
	return false;
}

//____ SetTabTextColor() ____________________________________________________________
bool WgTablist::SetTabTextColor( int id, WgColor col )
{
	WgTab * pTab = _findTab(id);
	if( pTab )
	{
		pTab->m_text.setColor( col );
		_requestRender();
		return true;
	}
	return false;
}

//____ SelectTab() ____________________________________________________________

bool WgTablist::SelectTab( int id )
{
	WgTab * pTab = _findTab(id);

	if( pTab == m_pTabSelected )
		return true;				// Not an error...

	if( pTab )
	{
		m_pTabSelected = pTab;
		pTab->m_bAlert = false;		// Selecting automatically stops any alert.
		_resizeTabs();				// fonts have changed
		_requestRender();

		WgEventHandler * pHandler = _eventHandler();
		if( pHandler )
			pHandler->QueueEvent( new WgEvent::TabSelect(this, pTab->m_id) );
		return true;
	}

	_resizeTabs();		// fonts have changed
	_requestRender();

	return false;
}

//____ GetSelectedTabId() _____________________________________________________________

int WgTablist::GetSelectedTabId() const
{
	if( m_pTabSelected )
		return m_pTabSelected->m_id;
	return -1;
}

//____ GetSelectedTabPos() _____________________________________________________________

int WgTablist::GetSelectedTabPos() const
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

bool WgTablist::SetAlert( int id, bool bAlertOn )
{
	WgTab * pTab = _findTab(id);
	if( pTab )
	{
		pTab->m_bAlert = bAlertOn;
		_requestRender();
		return true;
	}

	return false;
}

//____ GetAlert() _____________________________________________________________

bool WgTablist::GetAlert( int id ) const
{
	WgTab * pTab = _findTab(id);
	if( pTab )
	{
		return pTab->m_bAlert;
	}

	return false;
}

//____ ShowTab() ______________________________________________________________

bool WgTablist::ShowTab( int id, bool bVisible )
{
	WgTab * pTab = _findTab(id);
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
			_resizeTabs();
			_requestRender();
		}
		return true;
	}

	return false;
}

//____ GetTabCount() ______________________________________________________________

int WgTablist::GetTabCount( ) const
{
	int n = 0;

	for( WgTab * pTab = m_tabs.First(); pTab; pTab = pTab->Next() )
		n++;

	return n;
}

//____ GetTabWidth() ______________________________________________________________

Uint32 WgTablist::GetTabWidth( int id ) const
{
	WgTab * pTab = _findTab(id);
	if( pTab )
	{
		return pTab->m_width;
	}

	return 0;
}

//____ HasTab() ______________________________________________________________

bool WgTablist::HasTab( int id ) const
{
	return _findTab(id) != 0;
}


//////////////////////////////////////////////////////////////////////////
WgTab* WgTablist::GetSelectedTab() const
{
	return m_pTabSelected;
}

//____ GetFirstTab() ______________________________________________________________

WgTab* WgTablist::GetFirstTab() const
{
	return m_tabs.First();
}

//____ GetLastTab() ______________________________________________________________

WgTab* WgTablist::GetLastTab() const
{
	return m_tabs.Last();
}

//____ GetFirstVisibleTab() ___________________________________________________

WgTab* WgTablist::GetFirstVisibleTab() const
{
	WgTab* pTab = GetFirstTab();
	while(pTab && !pTab->m_bVisible)
		pTab = pTab->Next();
	return pTab;
}

//____ GetLastVisibleTab() ____________________________________________________

WgTab* WgTablist::GetLastVisibleTab() const
{
	WgTab* pTab = GetLastTab();
	while(pTab && !pTab->m_bVisible)
		pTab = pTab->Prev();
	return pTab;
}

//____ _findTab() ______________________________________________________________

WgTab* WgTablist::_findTab( int id ) const
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



//____ _getTabSource() _________________________________________________________

WgBlocksetPtr WgTablist::_getTabSource( WgTab * pTab ) const
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

//____ _getTabMode() __________________________________________________________

WgMode	WgTablist::_getTabMode(const WgTab& tab) const
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

//____ PreferredSize() ______________________________________________________________

WgSize WgTablist::PreferredSize() const
{
	if( m_tabs.Size() == 0 )
		return WgSize(0,0);

	// Calculate preferred width

	int preferredWidth = 0;

	switch( m_tabWidthMode )
	{
		case UNIFIED_WIDTH:
		{
			int widest = 0;
			int nTabs = 0;
			WgTab * pTab = m_tabs.First();
			while( pTab )
			{
				int w = _calcTabsWantedWidth(pTab);
				if( w > widest )
					widest = w;
				nTabs++;
				pTab = pTab->Next();
			}

			preferredWidth = widest*nTabs - m_overlap*(nTabs-1);
			break;
		}

		default:
			assert(0);					// Should never happen, but we will default to INDIVIDUAL_WIDTH if it does.
		case INDIVIDUAL_WIDTH:
		{
			int nTabs = 0;
			WgTab * pTab = m_tabs.First();
			while( pTab )
			{
				preferredWidth += _calcTabsWantedWidth(pTab);
				nTabs++;
				pTab = pTab->Next();
			}

			preferredWidth -= m_overlap*(nTabs-1);

			break;
		}
	}

	// Calculate best height

	int preferredHeight = 0;
	WgTab * pTab = m_tabs.First();
	while( pTab )
	{
		int h = 0;

		WgBlocksetPtr pBg = _getTabSource(pTab);
		if( pBg )
		{
			h = pBg->Height();
			int textH = pTab->m_text.height();
			if( h - pBg->Padding().Height() < textH )
				h = textH + pBg->Padding().Height();
		}
		else
			h = pTab->m_text.height();

		if( h > preferredHeight )
			preferredHeight = h;

		pTab = pTab->Next();
	}

	//

	return WgSize( preferredWidth, preferredHeight );

}


//____ _resizeTabs() ___________________________________________________________

void WgTablist::_resizeTabs()
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
				int w = _calcTabsWantedWidth(pTab);
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
				pTab->m_width = _calcTabsWantedWidth(pTab);
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

				float growFactor;
				if( combWidth )
					growFactor = diff / (float) combWidth;
				else
					growFactor = 0;

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

				float	growFactor;
				if( totalExpand )
					growFactor = diff / (float)totalExpand;
				else
					growFactor = 0.f;

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

//____ _calcTabsWantedWidth() _______________________________________________________

int WgTablist::_calcTabsWantedWidth( WgTab * pTab ) const
{
	int width = pTab->m_text.width();
/*
	if( pTab->m_pItemRow )
	{
		width += pTab->m_pItemRow->Width();
		if( pTab->m_pItemRow->NbItems() > 0 )
			width += pTab->m_pItemRow->ItemSpacing();
	}
*/
	WgBlocksetPtr pSrc = _getTabSource(pTab);
	if( pSrc )
	{
		width += pSrc->Padding().Width();

		if( width < pSrc->MinWidth() )
			width = pSrc->MinWidth();
	}

	if( width < m_minTabWidth )
		width = m_minTabWidth;

	return width;
}


//____ _pos2Tab() ______________________________________________________________

WgTab * WgTablist::_pos2Tab( int x, int y ) const
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

			WgBlocksetPtr pSrc = _getTabSource(pTab);
			bool	bHit = false;

			// Tab is hit if position is on a non-transparent (alpha != 0) pixel of the block
			// or inside tabs text-area.

			if( m_bTabOpaqueForMouse && x >= 0 && y >= 0 && x < (int) w && y < sz.h )
				bHit = true;
			else if( ((unsigned) x) > pSrc->Padding().left && ((unsigned) x) < w - pSrc->Padding().right &&
				((unsigned) y) > pSrc->Padding().top && y < sz.h - pSrc->Padding().bottom )
				bHit = true;
			else
				bHit = WgUtil::MarkTestBlock( WgCoord(x, y), pSrc->GetBlock( _getTabMode(*pTab), WgSize(w,sz.h) ), WgRect(0,0,w,sz.h), m_markOpacity);

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
/*
void WgTablist::_onUpdate( const WgUpdateInfo& _updateInfo )
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
				_requestRender();			// Somewhat stupid to render all tabs though...
				break;
			}
			pTab = pTab->Next();
		}
	}
}
*/

//____ _onRender() ________________________________________________________

void WgTablist::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& clip )
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
				_renderTab( pDevice, *pTab, r, clip );

			xOfs += pTab->m_advance;
		}
		pTab = pTab->Next();
	}

	// Render selected tab last, makes sure it comes out ontop.

	if( m_pTabSelected && m_pTabSelected->m_bVisible )
	{
		WgRect r( selectedX, _canvas.y, m_pTabSelected->m_width, _canvas.h );
		_renderTab( pDevice, *m_pTabSelected, r, clip );
	}
}

//____ _renderTab() _______________________________________________________

void WgTablist::_renderTab( WgGfxDevice * pDevice, WgTab& tab, WgRect dest, const WgRect& clip )
{
	WgBlocksetPtr pSrc = _getTabSource(&tab);

	WgMode	mode = _getTabMode(tab);

	WgBlock block = pSrc->GetBlock(mode,dest);

	pDevice->ClipBlitBlock( clip, block, dest );

	WgRect	r = block.ContentRect( dest );

/*	if( tab.m_pItemRow )
	{
		((WgItem*)(tab.m_pItemRow))->Render( r, clip );
		// move ahead x offset and reduce width for text part
		int rowWidth = tab.m_pItemRow->Width();
		if( tab.m_pItemRow->NbItems() > 0 )
			rowWidth += tab.m_pItemRow->ItemSpacing();
		r.x += rowWidth;
		r.w -= rowWidth;
	}
*/
	tab.m_text.setAlignment(m_textAlignment);

	WgRect clip2( clip, r );
	tab.m_text.setMode(mode);
	if( pSrc )
		tab.m_text.SetBgBlockColors( pSrc->TextColors() );
	pDevice->PrintText( clip2, &tab.m_text, r );
}

//____ _onRefresh() ____________________________________________________________

void WgTablist::_onRefresh( void )
{
	_resizeTabs();
	_requestRender();
}

//____ _onNewSize() ____________________________________________________________

void WgTablist::_onNewSize( const WgSize& size )
{
	_resizeTabs();
}


//____ _onEvent() ______________________________________________________________

void WgTablist::_onEvent( const WgEvent::Event * _pEvent, WgEventHandler * pHandler )
{
	switch( _pEvent->Type() )
	{
		case WG_EVENT_TICK:
		{
			const WgEvent::Tick * pEvent = static_cast<const WgEvent::Tick*>(_pEvent);

			m_alertModeCnt -= pEvent->Millisec();
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
						_requestRender();			// Somewhat stupid to render all tabs though...
						break;
					}
					pTab = pTab->Next();
				}
			}
			break;
		}

		case WG_EVENT_MOUSEBUTTON_PRESS:
		{
			const WgEvent::MouseButtonEvent * pEvent = static_cast<const WgEvent::MouseButtonEvent*>(_pEvent);

			WgCoord pos = pEvent->PointerPos();

			WgTab * pTab = _pos2Tab( pos.x, pos.y );
			if( pTab && pTab != m_pTabSelected )
			{
				if( pEvent->Button() == 1 )
					SelectTab(pTab->m_id);

				pHandler->QueueEvent( new WgEvent::TabPress(this, pTab->m_id, pEvent->Button()) );
			}
		}
		break;

		case WG_EVENT_MOUSE_ENTER:
		case WG_EVENT_MOUSE_MOVE:
		{
			WgCoord pos = _pEvent->PointerPos();

			WgTab * pTab = _pos2Tab( pos.x, pos.y );
			if( pTab != m_pTabMarked )
			{
				m_pTabMarked = pTab;
				_requestRender();
			}
		}
		break;

		case WG_EVENT_MOUSE_LEAVE:
			if( m_pTabMarked )
			{
				m_pTabMarked = 0;
				_requestRender();
			}
		break;

        default:
            break;
	}

	// Forward event depending on rules.

	if( _pEvent->IsMouseButtonEvent() )
	{
		if( static_cast<const WgEvent::MouseButtonEvent*>(_pEvent)->Button() != 1 )
			pHandler->ForwardEvent( _pEvent );
	}
	else
		pHandler->ForwardEvent( _pEvent );

}

//____ _onCloneContent() _______________________________________________________

void WgTablist::_onCloneContent( const WgWidget * _pOrg )
{
	WgTablist * pOrg = (WgTablist *) _pOrg;

//	WgChain<WgTab>	m_tabs;

	m_pProp			= pOrg->m_pProp;

	m_textAlignment		= pOrg->m_textAlignment;
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

bool WgTablist::_onAlphaTest( const WgCoord& ofs )
{
	return _pos2Tab(ofs.x, ofs.y) != NULL;
}
