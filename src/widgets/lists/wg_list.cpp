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
#include <wg_iconitem.h>
#include <wg_textitem.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	
	const char List::CLASSNAME[] = {"List"};
	
		
	
	//____ Constructor ____________________________________________________________
	
	List::List()
	{
		m_selectMode = SelectMode::SingleEntry;
	}
	
	//____ Destructor _____________________________________________________________
	
	List::~List()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool List::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Container::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * List::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	List_p List::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return List_p( static_cast<List*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setEntrySkin() _________________________________________________________
	
	void List::setEntrySkin( const Skin_p& pSkin )
	{
		Size oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->contentPadding() : Size();
	
		m_pEntrySkin[0] = pSkin;
		m_pEntrySkin[1] = pSkin;
		m_bOpaqueEntries = pSkin ? pSkin->isOpaque() : false;
	
		_onEntrySkinChanged( oldPadding, pSkin ? pSkin->contentPadding() : Size() );
	}
	
	bool List::setEntrySkin( const Skin_p& pOddEntrySkin, const Skin_p& pEvenEntrySkin )
	{
		Size oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->contentPadding() : Size();
		Size padding[2];
	
		if( pOddEntrySkin )
			padding[0] = pOddEntrySkin->contentPadding();
	
		if( pEvenEntrySkin )
			padding[1] = pEvenEntrySkin->contentPadding();
	
		if( (padding[0].w != padding[1].w) || (padding[0].h != padding[1].h) )
			return false;
	
		m_pEntrySkin[0] = pOddEntrySkin;
		m_pEntrySkin[1] = pEvenEntrySkin;
		m_bOpaqueEntries = (pOddEntrySkin->isOpaque() && pEvenEntrySkin->isOpaque());
	
		_onEntrySkinChanged( padding[0], pOddEntrySkin ? pOddEntrySkin->contentPadding() : Size() );
		return true;
	}
	
	//____ setSelectMode() ________________________________________________________
	
	bool List::setSelectMode( SelectMode mode )
	{
		if( mode != m_selectMode )
		{
			m_selectMode = mode;
	
			//TODO: Unselect all.
		}
		return true;
	}
	
	//____ setLassoSkin() _________________________________________________________
	
	void List::setLassoSkin( const Skin_p& pSkin )
	{
		m_pLassoSkin = pSkin;
	}


	//____ _cloneContent() _______________________________________________________
	
	void List::_cloneContent( const Widget * _pOrg )
	{
		Container::_cloneContent( _pOrg );
	}

	
	//____ _receive() _____________________________________________________________
	
	void List::_receive( const Msg_p& _pMsg )
	{
		Container::_receive(_pMsg);
	
		State state = m_state;
	
		switch( _pMsg->type() )
		{
			case MsgType::MouseMove:
			{	
				MouseMoveMsg_p pMsg = MouseMoveMsg::cast(_pMsg);
				ListSlot * pEntry = _findEntry(toLocal(pMsg->pointerPos()));
				if( pEntry && pEntry != m_pHoveredEntry.rawPtr() )
				{
					Rect geo;
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
			case MsgType::MouseLeave:
			{
				MouseLeaveMsg_p pMsg = MouseLeaveMsg::cast(_pMsg);
				ListSlot * pEntry = _findEntry(toLocal(pMsg->pointerPos()));
				if( m_pHoveredEntry && !pEntry )
				{
					Rect geo;
					_getEntryGeo( geo, m_pHoveredEntry.rawPtr() ); 
					_requestRender(geo);
					m_pHoveredEntry = 0;
				}
				break;
			}
			case MsgType::MousePress:
			{
				MouseButtonMsg_p pMsg = MouseButtonMsg::cast(_pMsg);
				if( m_selectMode != SelectMode::Unselectable && pMsg->button() == MouseButton::Left )
				{
					Coord ofs = toLocal(pMsg->pointerPos());
					if( !_listWindow().contains(ofs) )
						break;								// Click on header or somewhere else outside the real list.
					
					Rect listArea = _listArea();
					ListSlot * pEntry = _findEntry(ofs);
	
					ofs = listArea.limit(ofs);
					m_lassoBegin = ofs;
					m_lassoEnd = ofs;
	
					if( pEntry )
					{
						switch( m_selectMode )
						{
							case SelectMode::Unselectable:
								break;
							case SelectMode::SingleEntry:
								if( !pEntry->pWidget->state.isSelected() )
								{
									_clearSelected( true );
									_selectEntry( pEntry, true, true );
									m_pFocusedEntry = pEntry;	
								}
								break;
							case SelectMode::FlipOnSelect:
								_selectEntry( pEntry, !pEntry->isSelected(), true );
								m_pFocusedEntry = pEntry;
								break;
							case SelectMode::MultiEntries:
								if( pMsg->modKeys() & MODKEY_SHIFT && m_pFocusedEntry )
								{
									// Select range from focused to clicked entry.
	
									ListHook * pFocused = m_pFocusedEntry.rawPtr();
									ListHook * pFirstSel = wg::min( pEntry, pFocused );
									ListHook * pLastSel = wg::max( pEntry, pFocused );
									_selectRange( pFirstSel, pLastSel, true, true );
	
									// Unselect the rest if not CTRL-click.
	
									if( !(pMsg->modKeys() & MODKEY_CTRL) )
									{
										ListHook * pFirst = _firstHook();
										ListHook * pLast = _lastHook();
										if( pFirst < pFirstSel )
											_selectRange( pFirst, static_cast<ListHook*>(pFirstSel->_prevHook()), false, true );
										if( pLast > pLastSel )
											_selectRange( static_cast<ListHook*>(pLastSel->_nextHook()), pLast, false, true );
									}
								}
								else
								{
									if( pMsg->modKeys() & MODKEY_CTRL )
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
					else if( m_selectMode == SelectMode::SingleEntry || m_selectMode == SelectMode::MultiEntries )
					{
						_clearSelected(true);
						m_pFocusedEntry = 0;
					}
	
					_pMsg->swallow();
				}
				break;
			}
			case MsgType::MouseRelease:
				if( m_selectMode != SelectMode::Unselectable && MouseReleaseMsg::cast(_pMsg)->button() == MouseButton::Left )
				{
					Rect dirtyRect( m_lassoBegin, m_lassoEnd );
					_requestRender(dirtyRect);
	
					m_lassoBegin = m_lassoEnd;
					_pMsg->swallow();
				}
				break;
			case MsgType::MouseClick:
				if( m_selectMode != SelectMode::Unselectable && MouseClickMsg::cast(_pMsg)->button() == MouseButton::Left )
					_pMsg->swallow();
				break;
			case MsgType::MouseDoubleClick:
			case MsgType::MouseRepeat:
				if( m_selectMode != SelectMode::Unselectable && MouseButtonMsg::cast(_pMsg)->button() == MouseButton::Left )
					_pMsg->swallow();
				break;
			case MsgType::MouseDrag:
			{
				MouseDragMsg_p pMsg = MouseDragMsg::cast(_pMsg);
				if( (m_selectMode == SelectMode::FlipOnSelect || m_selectMode == SelectMode::MultiEntries) && pMsg->button() == MouseButton::Left )
				{
					Coord ofs = _listArea().limit(toLocal(pMsg->pointerPos()));
					ofs = _listWindow().limit(ofs);
	
					Rect oldLasso( m_lassoBegin, m_lassoEnd );
					Rect newLasso( m_lassoBegin, ofs );
	
					_onLassoUpdated( oldLasso, newLasso );
	
					Rect dirtyRect = oldLasso;
					dirtyRect.growToContain( ofs );
					_requestRender( dirtyRect );
					m_lassoEnd = ofs;
					_pMsg->swallow();
				}
				break;
			}
            
            default:
                break;
                
		}
	
		if( state != m_state )
			_setState(state);
	}



	int List::_selectSlots(ListSlot * pSlot, int nb, bool bPostMsg)
	{

	}

	int List::_unselectSlots(ListSlot * pSlot, int nb, bool bPostMsg)
	{

	}

	int List::_flipSelection(ListSlot * pSlot, int nb, bool bPostMsg)
	{
	
	}


	//____ _selectEntry() _________________________________________________________
	
	bool List::_selectEntry( ListHook * pHook, bool bSelected, bool bPostMsg )
	{
		State	state = pHook->m_pWidget->state();
	
		if( bSelected != state.isSelected() )
		{
	
			state.setSelected(bSelected);
			pHook->m_pWidget->_setState( state );
	
			if( bPostMsg )
			{
				ItemInfo * pItemInfo	= new ItemInfo[1];
				pItemInfo->pObject	= pHook->_widget();
				pItemInfo->id		= pHook->_widget()->id();
	
				Msg * pMsg;
				if( bSelected )
					pMsg = new ItemsSelectMsg(this, 1, pItemInfo);
				else
					pMsg = new ItemsUnselectMsg(this, 1, pItemInfo);
				Base::msgRouter()->post( pMsg );
			}
		}
	
		return true;
	}
	
	//____ _selectRange() _________________________________________________________
	
	int List::_selectRange( ListHook * pFirst, ListHook * pLast, bool bSelected, bool bPostMsg )
	{
		int	nModified = 0;
		ListHook * pEnd = static_cast<ListHook*>(pLast->_nextHook());
	
		// Request render for the range (not necessary, can be faster to take them one by one depending on circumstances).
	
		Rect geoFirst;
		Rect geoLast;
		_getEntryGeo( geoFirst, pFirst );
		_getEntryGeo( geoLast, pLast );
		_requestRender( Rect::getUnion(geoFirst,geoLast) );
	
		// Reserve ItemInfo array of right size if we are going to post message
	
		ItemInfo * pItemInfo = 0;
		if( bPostMsg )
		{
			int size = 0;
			for( ListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<ListHook*>(pHook->_nextHook()) )
			{
				if( bSelected != pHook->_widget()->state().isSelected() )
					size++;
			}
	
			if( size > 0 )
				pItemInfo = new ItemInfo[size];
		}
	
		// Loop through entries
	
		for( ListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<ListHook*>(pHook->_nextHook()) )
		{
			State	state = pHook->m_pWidget->state();
			if( bSelected != state.isSelected() )
			{
				state.setSelected(bSelected);
				pHook->m_pWidget->_setState( state );
	
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
			Msg * pMsg;
			if( bSelected )
				pMsg = new ItemsSelectMsg(this, 1, pItemInfo);
			else
				pMsg = new ItemsUnselectMsg(this, 1, pItemInfo);
			Base::msgRouter()->post( pMsg );
		}
	
		return nModified;
	}
	
	//____ _flipRange() _________________________________________________________
	
	int List::_flipRange( ListHook * pFirst, ListHook * pLast, bool bPostMsg )
	{
		int nSelected = 0;
		int nDeselected = 0;
		ListHook * pEnd = static_cast<ListHook*>(pLast->_nextHook());
	
		// Request render for the range
	
		Rect geoFirst;
		Rect geoLast;
		_getEntryGeo( geoFirst, pFirst );
		_getEntryGeo( geoLast, pLast );
		_requestRender( Rect::getUnion(geoFirst,geoLast) );
	
		// Reserve ItemInfo array of right size if we are going to post message
	
		ItemInfo * pSelectedItemsInfo = 0;
		ItemInfo * pDeselectedItemsInfo = 0;
		if( bPostMsg )
		{
			int nToSelect = 0;
			int nToDeselect = 0;
	
			for( ListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<ListHook*>(pHook->_nextHook()) )
			{
				if( pHook->_widget()->state().isSelected() )
					nToDeselect++;
				else
					nToSelect++;
			}
	
			if( nToSelect > 0 )
				pSelectedItemsInfo = new ItemInfo[nToSelect];
			if( nToDeselect > 0 )
				pDeselectedItemsInfo = new ItemInfo[nToDeselect];
		}
	
		// Loop through entries
	
		for( ListHook * pHook = pFirst ; pHook != pEnd ; pHook = static_cast<ListHook*>(pHook->_nextHook()) )
		{
			State	state = pHook->m_pWidget->state();
	
			state.setSelected(!state.isSelected());
			pHook->m_pWidget->_setState( state );
	
			if( bPostMsg )
			{
				ItemInfo * p;
				if( !state.isSelected() )
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
				Base::msgRouter()->post( new ItemsSelectMsg(this, 1, pSelectedItemsInfo) );
	
			if( nDeselected > 0 )
				Base::msgRouter()->post( new ItemsUnselectMsg(this, 1, pDeselectedItemsInfo) );
		}
	
		return nSelected + nDeselected;
	}
	
} // namespace wg
