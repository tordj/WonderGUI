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

#include <wg_list.h>
#include <wg_eventhandler.h>
#include <wg_iconfield.h>
#include <wg_textfield.h>
#include <wg_gfxdevice.h>


const char WgList::CLASSNAME[] = {"List"};
const char WgListHook::CLASSNAME[] = {"ListHook"};


//____ WgListHook::IsInstanceOf() __________________________________________

bool WgListHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgHook::IsInstanceOf(pClassName);
}

//____ WgListHook::ClassName() _____________________________________________

const char * WgListHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgListHook::Cast() __________________________________________________

WgListHookPtr WgListHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgListHookPtr( static_cast<WgListHook*>(pHook.RawPtr()) );

	return 0;
}

//____ WgListHook::Parent() ___________________________________________________

WgListPtr WgListHook::Parent() const 
{ 
	return static_cast<WgList*>(_parent()); 
}

//____ WgListHook::SetVisible() ________________________________________________

bool WgListHook::SetVisible( bool bVisible )
{
	if( m_bVisible != bVisible )
	{
		m_bVisible = bVisible;
		if( bVisible )
			static_cast<WgList*>(_parent())->_onWidgetAppeared( this );
		else
			static_cast<WgList*>(_parent())->_onWidgetDisappeared( this );
	}
	return true;
}

//____ WgListHook::SetSelected() ________________________________________________

bool WgListHook::SetSelected( bool bSelected )
{
	if( !m_bVisible )
		return false;

	return (static_cast<WgList*>(_parent()))->_selectEntry( this, bSelected, false );
}

//____ Constructor ____________________________________________________________

WgList::WgList()
{
	m_selectMode = WG_SELECT_SINGLE;
}

//____ Destructor _____________________________________________________________

WgList::~WgList()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgList::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgContainer::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgList::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgListPtr WgList::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgListPtr( static_cast<WgList*>(pObject.RawPtr()) );

	return 0;
}

//____ SetEntrySkin() _________________________________________________________

void WgList::SetEntrySkin( const WgSkinPtr& pSkin )
{
	WgSize oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->ContentPadding() : WgSize();

	m_pEntrySkin[0] = pSkin;
	m_pEntrySkin[1] = pSkin;
	m_bOpaqueEntries = pSkin ? pSkin->IsOpaque() : false;

	_onEntrySkinChanged( oldPadding, pSkin ? pSkin->ContentPadding() : WgSize() );
}

bool WgList::SetEntrySkin( const WgSkinPtr& pOddEntrySkin, const WgSkinPtr& pEvenEntrySkin )
{
	WgSize oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->ContentPadding() : WgSize();
	WgSize padding[2];

	if( pOddEntrySkin )
		padding[0] = pOddEntrySkin->ContentPadding();

	if( pEvenEntrySkin )
		padding[1] = pEvenEntrySkin->ContentPadding();

	if( (padding[0].w != padding[1].w) || (padding[0].h != padding[1].h) )
		return false;

	m_pEntrySkin[0] = pOddEntrySkin;
	m_pEntrySkin[1] = pEvenEntrySkin;
	m_bOpaqueEntries = (pOddEntrySkin->IsOpaque() && pEvenEntrySkin->IsOpaque());

	_onEntrySkinChanged( padding[0], pOddEntrySkin ? pOddEntrySkin->ContentPadding() : WgSize() );
	return true;
}

//____ SetSelectMode() ________________________________________________________

bool WgList::SetSelectMode( WgSelectMode mode )
{
	if( mode != m_selectMode )
	{
		m_selectMode = mode;

		//TODO: Unselect all.
	}
	return true;
}

//____ SetLassoSkin() _________________________________________________________

void WgList::SetLassoSkin( const WgSkinPtr& pSkin )
{
	m_pLassoSkin = pSkin;
}

//____ _onEvent() _____________________________________________________________

void WgList::_onEvent( const WgEventPtr& _pEvent )
{
	WgContainer::_onEvent(_pEvent);

	WgState oldState = m_state;

	switch( _pEvent->Type() )
	{
		case WG_EVENT_MOUSE_ENTER:
		{	
			WgListHook * pEntry = _findEntry(ToLocal(_pEvent->PointerGlobalPos()));
			if( pEntry && pEntry != m_pHoveredEntry.RawPtr() )
			{
				WgRect geo;
				if( m_pHoveredEntry )
				{
					_getEntryGeo( geo, m_pHoveredEntry.RawPtr() ); 
					_requestRender(geo);
				}

				_getEntryGeo( geo, pEntry ); 
				_requestRender(geo);
				m_pHoveredEntry = pEntry;
			}
			break;
		}
		case WG_EVENT_MOUSE_LEAVE:
		{
			WgListHook * pEntry = _findEntry(ToLocal(_pEvent->PointerGlobalPos()));
			if( m_pHoveredEntry && !pEntry )
			{
				WgRect geo;
				_getEntryGeo( geo, m_pHoveredEntry.RawPtr() ); 
				_requestRender(geo);
				m_pHoveredEntry = 0;
			}
			break;
		}
		case WG_EVENT_MOUSE_PRESS:
		{
			WgMousePressEventPtr pEvent = WgMousePressEvent::Cast(_pEvent);
			if( m_selectMode != WG_SELECT_NONE && pEvent->Button() == WG_BUTTON_LEFT )
			{
				WgCoord ofs = ToLocal(pEvent->PointerGlobalPos());
				if( !_listWindow().Contains(ofs) )
					break;								// Click on header or somewhere else outside the real list.
				
				WgRect listArea = _listArea();
				WgListHook * pEntry = _findEntry(ofs);

				ofs = listArea.Limit(ofs);
				m_lassoBegin = ofs;
				m_lassoEnd = ofs;

				if( pEntry )
				{
					switch( m_selectMode )
					{
						case WG_SELECT_NONE:
							break;
						case WG_SELECT_SINGLE:
							if( !pEntry->IsSelected() )
							{
								_clearSelected( true );
								_selectEntry( pEntry, true, true );
								m_pFocusedEntry = pEntry;	
							}
							break;
						case WG_SELECT_FLIP:
							_selectEntry( pEntry, !pEntry->IsSelected(), true );
							m_pFocusedEntry = pEntry;
							break;
						case WG_SELECT_MULTI:
							if( pEvent->ModKeys() & WG_MODKEY_SHIFT && m_pFocusedEntry )
							{
								// Select range from focused to clicked entry.

								WgListHook * pFocused = m_pFocusedEntry.RawPtr();
								WgListHook * pFirstSel = WgMin( pEntry, pFocused );
								WgListHook * pLastSel = WgMax( pEntry, pFocused );
								_selectRange( pFirstSel, pLastSel, true, true );

								// Unselect the rest if not CTRL-click.

								if( !(pEvent->ModKeys() & WG_MODKEY_CTRL) )
								{
									WgListHook * pFirst = static_cast<WgListHook*>(_firstHook());
									WgListHook * pLast = static_cast<WgListHook*>(_lastHook());
									if( pFirst < pFirstSel )
										_selectRange( pFirst, static_cast<WgListHook*>(pFirstSel->_prevHook()), false, true );
									if( pLast > pLastSel )
										_selectRange( static_cast<WgListHook*>(pLastSel->_nextHook()), pLast, false, true );
								}
							}
							else
							{
								if( pEvent->ModKeys() & WG_MODKEY_CTRL )
								{
									// CTRL-click: We just flip the entry.
									_selectEntry( pEntry, !pEntry->IsSelected(), true );
								}
								else
								{
									//TODO: Not post unselected/selected for already selected pressed entry.
									_clearSelected( true );
									_selectEntry( pEntry, true, true );
								}
								m_pFocusedEntry = pEntry;
							}
							break;
					}
				}
				else if( WG_SELECT_SINGLE || WG_SELECT_MULTI )
				{
					_clearSelected(true);
					m_pFocusedEntry = 0;
				}

				_pEvent->Swallow();
			}
			break;
		}
		case WG_EVENT_MOUSE_RELEASE:
			if( m_selectMode != WG_SELECT_NONE && WgMouseReleaseEvent::Cast(_pEvent)->Button() == WG_BUTTON_LEFT )
			{
				WgRect dirtyRect( m_lassoBegin, m_lassoEnd );
				_requestRender(dirtyRect);

				m_lassoBegin = m_lassoEnd;
				_pEvent->Swallow();
			}
			break;
		case WG_EVENT_MOUSE_CLICK:
			if( m_selectMode != WG_SELECT_NONE && WgMouseClickEvent::Cast(_pEvent)->Button() == WG_BUTTON_LEFT )
				_pEvent->Swallow();
			break;
		case WG_EVENT_MOUSE_DOUBLE_CLICK:
		case WG_EVENT_MOUSE_REPEAT:
			if( m_selectMode != WG_SELECT_NONE && WgMouseButtonEvent::Cast(_pEvent)->Button() == WG_BUTTON_LEFT )
				_pEvent->Swallow();
			break;
		case WG_EVENT_MOUSE_DRAG:
		{
			WgMouseDragEventPtr pEvent = WgMouseDragEvent::Cast(_pEvent);
			if( (m_selectMode == WG_SELECT_FLIP || m_selectMode == WG_SELECT_MULTI) && pEvent->Button() == WG_BUTTON_LEFT )
			{
				WgCoord ofs = _listArea().Limit(ToLocal(pEvent->PointerGlobalPos()));
				ofs = _listWindow().Limit(ofs);

				WgRect oldLasso( m_lassoBegin, m_lassoEnd );
				WgRect newLasso( m_lassoBegin, ofs );

				_onLassoUpdated( oldLasso, newLasso );

				WgRect dirtyRect = oldLasso;
				dirtyRect.GrowToContain( ofs );
				_requestRender( dirtyRect );
				m_lassoEnd = ofs;
				_pEvent->Swallow();
			}
			break;
		}
	}

	if( m_state != oldState )
		_onStateChanged(oldState);
}

//____ _selectEntry() _________________________________________________________

bool WgList::_selectEntry( WgListHook * pHook, bool bSelected, bool bPostEvent )
{
	WgState	oldState = pHook->m_pWidget->State();

	if( bSelected != oldState.IsSelected() )
	{

		pHook->m_pWidget->m_state.SetSelected(bSelected);
		pHook->m_pWidget->_onStateChanged( oldState );

		if( bPostEvent )
		{
			WgItemInfo * pItemInfo	= new WgItemInfo[1];
			pItemInfo->pObject	= pHook->_widget();
			pItemInfo->id		= pHook->_widget()->Id();

			WgEvent * pEvent;
			if( bSelected )
				pEvent = new WgItemsSelectEvent(this, 1, pItemInfo);
			else
				pEvent = new WgItemsUnselectEvent(this, 1, pItemInfo);
			_eventHandler()->QueueEvent( pEvent );
		}
	}

	return true;
}

//____ _clearSelected() _______________________________________________________

void WgList::_clearSelected( bool bPostEvent )
{
	_selectRange( static_cast<WgListHook*>(_firstHook()), static_cast<WgListHook*>(_lastHook()), false, bPostEvent );
}

//____ _selectRange() _________________________________________________________

int WgList::_selectRange( WgListHook * pFirst, WgListHook * pLast, bool bSelected, bool bPostEvent )
{
	int	nModified = 0;
	WgListHook * pEnd = static_cast<WgListHook*>(pLast->_nextHook());

	// Request render for the range (not necessary, can be faster to take them one by one depending on circumstances).

	WgRect geoFirst;
	WgRect geoLast;
	_getEntryGeo( geoFirst, pFirst );
	_getEntryGeo( geoLast, pLast );
	_requestRender( WgRect::Union(geoFirst,geoLast) );

	// Reserve ItemInfo array of right size if we are going to post event

	WgItemInfo * pItemInfo = 0;
	if( bPostEvent )
	{
		int size = 0;
		for( WgListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<WgListHook*>(pHook->_nextHook()) )
		{
			if( bSelected != pHook->_widget()->State().IsSelected() )
				size++;
		}

		if( size > 0 )
			pItemInfo = new WgItemInfo[size];
	}

	// Loop through entries

	for( WgListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<WgListHook*>(pHook->_nextHook()) )
	{
		WgState	oldState = pHook->m_pWidget->State();
		if( bSelected != oldState.IsSelected() )
		{
			pHook->m_pWidget->m_state.SetSelected(bSelected);
			pHook->m_pWidget->_onStateChanged( oldState );

			if( bPostEvent )
			{
				pItemInfo[nModified].pObject	= pHook->_widget();
				pItemInfo[nModified].id			= pHook->_widget()->Id();

			}
			nModified++;
		}
	}

	// Post event

	if( bPostEvent )
	{
		WgEvent * pEvent;
		if( bSelected )
			pEvent = new WgItemsSelectEvent(this, 1, pItemInfo);
		else
			pEvent = new WgItemsUnselectEvent(this, 1, pItemInfo);
		_eventHandler()->QueueEvent( pEvent );
	}

	return nModified;
}

//____ _flipRange() _________________________________________________________

int WgList::_flipRange( WgListHook * pFirst, WgListHook * pLast, bool bPostEvent )
{
	int nSelected = 0;
	int nDeselected = 0;
	WgListHook * pEnd = static_cast<WgListHook*>(pLast->_nextHook());

	// Request render for the range

	WgRect geoFirst;
	WgRect geoLast;
	_getEntryGeo( geoFirst, pFirst );
	_getEntryGeo( geoLast, pLast );
	_requestRender( WgRect::Union(geoFirst,geoLast) );

	// Reserve ItemInfo array of right size if we are going to post event

	WgItemInfo * pSelectedItemsInfo = 0;
	WgItemInfo * pDeselectedItemsInfo = 0;
	if( bPostEvent )
	{
		int nToSelect = 0;
		int nToDeselect = 0;

		for( WgListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<WgListHook*>(pHook->_nextHook()) )
		{
			if( pHook->_widget()->State().IsSelected() )
				nToDeselect++;
			else
				nToSelect++;
		}

		if( nToSelect > 0 )
			pSelectedItemsInfo = new WgItemInfo[nToSelect];
		if( nToDeselect > 0 )
			pDeselectedItemsInfo = new WgItemInfo[nToDeselect];
	}

	// Loop through entries

	for( WgListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<WgListHook*>(pHook->_nextHook()) )
	{
		WgState	oldState = pHook->m_pWidget->State();

		pHook->m_pWidget->m_state.SetSelected(!oldState.IsSelected());
		pHook->m_pWidget->_onStateChanged( oldState );

		if( bPostEvent )
		{
			WgItemInfo * p;
			if( oldState.IsSelected() )
				p = &pDeselectedItemsInfo[nSelected++];
			else
				p = &pSelectedItemsInfo[nDeselected++];

			p->pObject	= pHook->_widget();
			p->id		= pHook->_widget()->Id();
		}
	}

	// Post event

	if( bPostEvent )
	{
		if( nSelected > 0 )
			_eventHandler()->QueueEvent( new WgItemsSelectEvent(this, 1, pSelectedItemsInfo) );

		if( nDeselected > 0 )
			_eventHandler()->QueueEvent( new WgItemsUnselectEvent(this, 1, pDeselectedItemsInfo) );
	}

	return nSelected + nDeselected;
}

//____ _renderHeader() ________________________________________________________

void WgList::_renderHeader( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip, 
									const WgSkinPtr& pSkin, WgTextField * pText, WgIconField * pLabelIcon, 
									WgIconField * pSortIcon, WgState state, bool bShowSortIcon, bool bInvertedSort )
{
	WgRect canvas( _canvas );
	WgRect clip( _canvas, _clip );

	if( pSkin )
	{
		pSkin->Render( pDevice, canvas, state, _clip );
		canvas = pSkin->ContentRect( canvas, state );
	}

	WgRect sortRect = pSortIcon->GetIconRect( canvas );
	WgRect labelRect = pSortIcon->GetTextRect( canvas, sortRect );
	WgRect iconRect = pLabelIcon->GetIconRect( labelRect );
	labelRect = pLabelIcon->GetTextRect( labelRect, iconRect );

	if( bShowSortIcon && !pSortIcon->IsEmpty() )
	{
		WgState iconState = state;
		iconState.SetSelected( bInvertedSort );
		pSortIcon->Skin()->Render( pDevice, sortRect, iconState, _clip );
	}

	if( !pLabelIcon->IsEmpty() )
		pLabelIcon->Skin()->Render( pDevice, iconRect, state, _clip );

	if( !pText->IsEmpty() )
		pText->OnRender( pDevice, labelRect, _clip );
}
