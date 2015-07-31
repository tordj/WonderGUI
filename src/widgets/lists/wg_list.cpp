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
#include <wg_msgrouter.h>
#include <wg_iconfield.h>
#include <wg_textfield.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	
	const char WgList::CLASSNAME[] = {"List"};
	const char WgListHook::CLASSNAME[] = {"ListHook"};
	
	
	//____ WgListHook::isInstanceOf() __________________________________________
	
	bool WgListHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgHook::isInstanceOf(pClassName);
	}
	
	//____ WgListHook::className() _____________________________________________
	
	const char * WgListHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ WgListHook::cast() __________________________________________________
	
	WgListHook_p WgListHook::cast( const WgHook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return WgListHook_p( static_cast<WgListHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	//____ WgListHook::parent() ___________________________________________________
	
	WgList_p WgListHook::parent() const 
	{ 
		return static_cast<WgList*>(_parent()); 
	}
	
	//____ WgListHook::setVisible() ________________________________________________
	
	bool WgListHook::setVisible( bool bVisible )
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
	
	//____ WgListHook::setSelected() ________________________________________________
	
	bool WgListHook::setSelected( bool bSelected )
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
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgList::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgContainer::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgList::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgList_p WgList::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgList_p( static_cast<WgList*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setEntrySkin() _________________________________________________________
	
	void WgList::setEntrySkin( const WgSkin_p& pSkin )
	{
		WgSize oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->contentPadding() : WgSize();
	
		m_pEntrySkin[0] = pSkin;
		m_pEntrySkin[1] = pSkin;
		m_bOpaqueEntries = pSkin ? pSkin->isOpaque() : false;
	
		_onEntrySkinChanged( oldPadding, pSkin ? pSkin->contentPadding() : WgSize() );
	}
	
	bool WgList::setEntrySkin( const WgSkin_p& pOddEntrySkin, const WgSkin_p& pEvenEntrySkin )
	{
		WgSize oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->contentPadding() : WgSize();
		WgSize padding[2];
	
		if( pOddEntrySkin )
			padding[0] = pOddEntrySkin->contentPadding();
	
		if( pEvenEntrySkin )
			padding[1] = pEvenEntrySkin->contentPadding();
	
		if( (padding[0].w != padding[1].w) || (padding[0].h != padding[1].h) )
			return false;
	
		m_pEntrySkin[0] = pOddEntrySkin;
		m_pEntrySkin[1] = pEvenEntrySkin;
		m_bOpaqueEntries = (pOddEntrySkin->isOpaque() && pEvenEntrySkin->isOpaque());
	
		_onEntrySkinChanged( padding[0], pOddEntrySkin ? pOddEntrySkin->contentPadding() : WgSize() );
		return true;
	}
	
	//____ setSelectMode() ________________________________________________________
	
	bool WgList::setSelectMode( WgSelectMode mode )
	{
		if( mode != m_selectMode )
		{
			m_selectMode = mode;
	
			//TODO: Unselect all.
		}
		return true;
	}
	
	//____ setLassoSkin() _________________________________________________________
	
	void WgList::setLassoSkin( const WgSkin_p& pSkin )
	{
		m_pLassoSkin = pSkin;
	}
	
	//____ _onMsg() _____________________________________________________________
	
	void WgList::_onMsg( const WgMsg_p& _pMsg )
	{
		WgContainer::_onMsg(_pMsg);
	
		WgState oldState = m_state;
	
		switch( _pMsg->type() )
		{
			case WG_MSG_MOUSE_ENTER:
			{	
				WgListHook * pEntry = _findEntry(toLocal(_pMsg->pointerPos()));
				if( pEntry && pEntry != m_pHoveredEntry.rawPtr() )
				{
					WgRect geo;
					if( m_pHoveredEntry )
					{
						_getEntryGeo( geo, m_pHoveredEntry.rawPtr() ); 
						_requestRender(geo);
					}
	
					_getEntryGeo( geo, pEntry ); 
					_requestRender(geo);
					m_pHoveredEntry = pEntry;
				}
				break;
			}
			case WG_MSG_MOUSE_LEAVE:
			{
				WgListHook * pEntry = _findEntry(toLocal(_pMsg->pointerPos()));
				if( m_pHoveredEntry && !pEntry )
				{
					WgRect geo;
					_getEntryGeo( geo, m_pHoveredEntry.rawPtr() ); 
					_requestRender(geo);
					m_pHoveredEntry = 0;
				}
				break;
			}
			case WG_MSG_MOUSE_PRESS:
			{
				WgMousePressMsg_p pMsg = WgMousePressMsg::cast(_pMsg);
				if( m_selectMode != WG_SELECT_NONE && pMsg->button() == WG_BUTTON_LEFT )
				{
					WgCoord ofs = toLocal(pMsg->pointerPos());
					if( !_listWindow().contains(ofs) )
						break;								// Click on header or somewhere else outside the real list.
					
					WgRect listArea = _listArea();
					WgListHook * pEntry = _findEntry(ofs);
	
					ofs = listArea.limit(ofs);
					m_lassoBegin = ofs;
					m_lassoEnd = ofs;
	
					if( pEntry )
					{
						switch( m_selectMode )
						{
							case WG_SELECT_NONE:
								break;
							case WG_SELECT_SINGLE:
								if( !pEntry->isSelected() )
								{
									_clearSelected( true );
									_selectEntry( pEntry, true, true );
									m_pFocusedEntry = pEntry;	
								}
								break;
							case WG_SELECT_FLIP:
								_selectEntry( pEntry, !pEntry->isSelected(), true );
								m_pFocusedEntry = pEntry;
								break;
							case WG_SELECT_MULTI:
								if( pMsg->modKeys() & WG_MODKEY_SHIFT && m_pFocusedEntry )
								{
									// Select range from focused to clicked entry.
	
									WgListHook * pFocused = m_pFocusedEntry.rawPtr();
									WgListHook * pFirstSel = WgMin( pEntry, pFocused );
									WgListHook * pLastSel = WgMax( pEntry, pFocused );
									_selectRange( pFirstSel, pLastSel, true, true );
	
									// Unselect the rest if not CTRL-click.
	
									if( !(pMsg->modKeys() & WG_MODKEY_CTRL) )
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
									if( pMsg->modKeys() & WG_MODKEY_CTRL )
									{
										// CTRL-click: We just flip the entry.
										_selectEntry( pEntry, !pEntry->isSelected(), true );
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
	
					_pMsg->swallow();
				}
				break;
			}
			case WG_MSG_MOUSE_RELEASE:
				if( m_selectMode != WG_SELECT_NONE && WgMouseReleaseMsg::cast(_pMsg)->button() == WG_BUTTON_LEFT )
				{
					WgRect dirtyRect( m_lassoBegin, m_lassoEnd );
					_requestRender(dirtyRect);
	
					m_lassoBegin = m_lassoEnd;
					_pMsg->swallow();
				}
				break;
			case WG_MSG_MOUSE_CLICK:
				if( m_selectMode != WG_SELECT_NONE && WgMouseClickMsg::cast(_pMsg)->button() == WG_BUTTON_LEFT )
					_pMsg->swallow();
				break;
			case WG_MSG_MOUSE_DOUBLE_CLICK:
			case WG_MSG_MOUSE_REPEAT:
				if( m_selectMode != WG_SELECT_NONE && WgMouseButtonMsg::cast(_pMsg)->button() == WG_BUTTON_LEFT )
					_pMsg->swallow();
				break;
			case WG_MSG_MOUSE_DRAG:
			{
				WgMouseDragMsg_p pMsg = WgMouseDragMsg::cast(_pMsg);
				if( (m_selectMode == WG_SELECT_FLIP || m_selectMode == WG_SELECT_MULTI) && pMsg->button() == WG_BUTTON_LEFT )
				{
					WgCoord ofs = _listArea().limit(toLocal(pMsg->pointerPos()));
					ofs = _listWindow().limit(ofs);
	
					WgRect oldLasso( m_lassoBegin, m_lassoEnd );
					WgRect newLasso( m_lassoBegin, ofs );
	
					_onLassoUpdated( oldLasso, newLasso );
	
					WgRect dirtyRect = oldLasso;
					dirtyRect.growToContain( ofs );
					_requestRender( dirtyRect );
					m_lassoEnd = ofs;
					_pMsg->swallow();
				}
				break;
			}
		}
	
		if( m_state != oldState )
			_onStateChanged(oldState);
	}
	
	//____ _selectEntry() _________________________________________________________
	
	bool WgList::_selectEntry( WgListHook * pHook, bool bSelected, bool bPostMsg )
	{
		WgState	oldState = pHook->m_pWidget->state();
	
		if( bSelected != oldState.isSelected() )
		{
	
			pHook->m_pWidget->m_state.setSelected(bSelected);
			pHook->m_pWidget->_onStateChanged( oldState );
	
			if( bPostMsg )
			{
				WgItemInfo * pItemInfo	= new WgItemInfo[1];
				pItemInfo->pObject	= pHook->_widget();
				pItemInfo->id		= pHook->_widget()->id();
	
				WgMsg * pMsg;
				if( bSelected )
					pMsg = new WgItemsSelectMsg(this, 1, pItemInfo);
				else
					pMsg = new WgItemsUnselectMsg(this, 1, pItemInfo);
				WgBase::msgRouter()->post( pMsg );
			}
		}
	
		return true;
	}
	
	//____ _clearSelected() _______________________________________________________
	
	void WgList::_clearSelected( bool bPostMsg )
	{
		_selectRange( static_cast<WgListHook*>(_firstHook()), static_cast<WgListHook*>(_lastHook()), false, bPostMsg );
	}
	
	//____ _selectRange() _________________________________________________________
	
	int WgList::_selectRange( WgListHook * pFirst, WgListHook * pLast, bool bSelected, bool bPostMsg )
	{
		int	nModified = 0;
		WgListHook * pEnd = static_cast<WgListHook*>(pLast->_nextHook());
	
		// Request render for the range (not necessary, can be faster to take them one by one depending on circumstances).
	
		WgRect geoFirst;
		WgRect geoLast;
		_getEntryGeo( geoFirst, pFirst );
		_getEntryGeo( geoLast, pLast );
		_requestRender( WgRect::getUnion(geoFirst,geoLast) );
	
		// Reserve ItemInfo array of right size if we are going to post message
	
		WgItemInfo * pItemInfo = 0;
		if( bPostMsg )
		{
			int size = 0;
			for( WgListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<WgListHook*>(pHook->_nextHook()) )
			{
				if( bSelected != pHook->_widget()->state().isSelected() )
					size++;
			}
	
			if( size > 0 )
				pItemInfo = new WgItemInfo[size];
		}
	
		// Loop through entries
	
		for( WgListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<WgListHook*>(pHook->_nextHook()) )
		{
			WgState	oldState = pHook->m_pWidget->state();
			if( bSelected != oldState.isSelected() )
			{
				pHook->m_pWidget->m_state.setSelected(bSelected);
				pHook->m_pWidget->_onStateChanged( oldState );
	
				if( bPostMsg )
				{
					pItemInfo[nModified].pObject	= pHook->_widget();
					pItemInfo[nModified].id			= pHook->_widget()->id();
	
				}
				nModified++;
			}
		}
	
		// Post message
	
		if( bPostMsg )
		{
			WgMsg * pMsg;
			if( bSelected )
				pMsg = new WgItemsSelectMsg(this, 1, pItemInfo);
			else
				pMsg = new WgItemsUnselectMsg(this, 1, pItemInfo);
			WgBase::msgRouter()->post( pMsg );
		}
	
		return nModified;
	}
	
	//____ _flipRange() _________________________________________________________
	
	int WgList::_flipRange( WgListHook * pFirst, WgListHook * pLast, bool bPostMsg )
	{
		int nSelected = 0;
		int nDeselected = 0;
		WgListHook * pEnd = static_cast<WgListHook*>(pLast->_nextHook());
	
		// Request render for the range
	
		WgRect geoFirst;
		WgRect geoLast;
		_getEntryGeo( geoFirst, pFirst );
		_getEntryGeo( geoLast, pLast );
		_requestRender( WgRect::getUnion(geoFirst,geoLast) );
	
		// Reserve ItemInfo array of right size if we are going to post message
	
		WgItemInfo * pSelectedItemsInfo = 0;
		WgItemInfo * pDeselectedItemsInfo = 0;
		if( bPostMsg )
		{
			int nToSelect = 0;
			int nToDeselect = 0;
	
			for( WgListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<WgListHook*>(pHook->_nextHook()) )
			{
				if( pHook->_widget()->state().isSelected() )
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
			WgState	oldState = pHook->m_pWidget->state();
	
			pHook->m_pWidget->m_state.setSelected(!oldState.isSelected());
			pHook->m_pWidget->_onStateChanged( oldState );
	
			if( bPostMsg )
			{
				WgItemInfo * p;
				if( oldState.isSelected() )
					p = &pDeselectedItemsInfo[nSelected++];
				else
					p = &pSelectedItemsInfo[nDeselected++];
	
				p->pObject	= pHook->_widget();
				p->id		= pHook->_widget()->id();
			}
		}
	
		// Post message
	
		if( bPostMsg )
		{
			if( nSelected > 0 )
				WgBase::msgRouter()->post( new WgItemsSelectMsg(this, 1, pSelectedItemsInfo) );
	
			if( nDeselected > 0 )
				WgBase::msgRouter()->post( new WgItemsUnselectMsg(this, 1, pDeselectedItemsInfo) );
		}
	
		return nSelected + nDeselected;
	}
	
	//____ _renderHeader() ________________________________________________________
	
	void WgList::_renderHeader( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip, 
										const WgSkin_p& pSkin, WgTextField * pText, WgIconField * pLabelIcon, 
										WgIconField * pSortIcon, WgState state, bool bShowSortIcon, bool bInvertedSort )
	{
		WgRect canvas( _canvas );
		WgRect clip( _canvas, _clip );
	
		if( pSkin )
		{
			pSkin->render( pDevice, canvas, state, _clip );
			canvas = pSkin->contentRect( canvas, state );
		}
	
		WgRect sortRect = pSortIcon->getIconRect( canvas );
		WgRect labelRect = pSortIcon->getTextRect( canvas, sortRect );
		WgRect iconRect = pLabelIcon->getIconRect( labelRect );
		labelRect = pLabelIcon->getTextRect( labelRect, iconRect );
	
		if( bShowSortIcon && !pSortIcon->isEmpty() )
		{
			WgState iconState = state;
			iconState.setSelected( bInvertedSort );
			pSortIcon->skin()->render( pDevice, sortRect, iconState, _clip );
		}
	
		if( !pLabelIcon->isEmpty() )
			pLabelIcon->skin()->render( pDevice, iconRect, state, _clip );
	
		if( !pText->isEmpty() )
			pText->onRender( pDevice, labelRect, _clip );
	}

} // namespace wg
