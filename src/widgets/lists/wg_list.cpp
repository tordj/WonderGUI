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
	
	List::List() : m_selectMode(SelectMode::SingleEntry)
	{
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
	
	List_p List::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return List_p( static_cast<List*>(pObject) );
	
		return 0;
	}
	
	//____ setEntrySkin() _________________________________________________________
	
	void List::setEntrySkin( Skin * pSkin )
	{
		Size oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->contentPadding() : Size();
	
		m_pEntrySkin[0] = pSkin;
		m_pEntrySkin[1] = pSkin;
		m_bOpaqueEntries = pSkin ? pSkin->isOpaque() : false;
	
		_onEntrySkinChanged( oldPadding, pSkin ? pSkin->contentPadding() : Size() );
	}
	
	bool List::setEntrySkin( Skin * pOddEntrySkin, Skin * pEvenEntrySkin )
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
	
	void List::setLassoSkin( Skin * pSkin )
	{
		m_pLassoSkin = pSkin;
	}


	//____ _cloneContent() _______________________________________________________
	
	void List::_cloneContent( const Widget * _pOrg )
	{
		Container::_cloneContent( _pOrg );
	}

	
	//____ _receive() _____________________________________________________________
	
	void List::_receive( Msg * _pMsg )
	{
		Container::_receive(_pMsg);
	
		State state = m_state;
	
		switch( _pMsg->type() )
		{
			case MsgType::MouseMove:
			{	
				MouseMoveMsg_p pMsg = MouseMoveMsg::cast(_pMsg);
				ListSlot * pEntry = _findEntry(toLocal(pMsg->pointerPos()));
				if( pEntry && pEntry->pWidget != m_pHoveredChild.rawPtr() )
				{
					Rect geo;
					if( m_pHoveredChild )
					{
						_getEntryGeo( geo, (ListSlot*) m_pHoveredChild->_slot() ); 
						_requestRender(geo);
					}
	
					_getEntryGeo( geo, pEntry ); 
					_requestRender(geo);
					m_pHoveredChild = pEntry->pWidget;
				}
				break;
			}
			case MsgType::MouseLeave:
			{
				MouseLeaveMsg_p pMsg = MouseLeaveMsg::cast(_pMsg);
				ListSlot * pEntry = _findEntry(toLocal(pMsg->pointerPos()));
				if( m_pHoveredChild && !pEntry )
				{
					Rect geo;
					_getEntryGeo( geo, (ListSlot*) m_pHoveredChild->_slot() ); 
					_requestRender(geo);
					m_pHoveredChild = nullptr;
				}
				break;
			}
			case MsgType::MousePress:
			{
				grabFocus();

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
								if( !pEntry->pWidget->state().isSelected() )
								{
									_unselectSlots( _beginSlots(), _endSlots(), true );
									_selectSlot( pEntry, true );
									m_pFocusedChild = pEntry->pWidget;	
								}
								break;
							case SelectMode::FlipOnSelect:
								_setSlotSelection( pEntry, _nextSlot(pEntry), !pEntry->pWidget->state().isSelected(), true );
								m_pFocusedChild = pEntry->pWidget;
								break;
							case SelectMode::MultiEntries:
								if( pMsg->modKeys() & MODKEY_SHIFT && m_pFocusedChild )
								{
									// Select range from focused to clicked entry.
	
									ListSlot * pFocused = (ListSlot*) m_pFocusedChild->_slot();
									ListSlot * pBeginSel = wg::min( pEntry, pFocused );
									ListSlot * pEndSel = _nextSlot(wg::max( pEntry, pFocused ));
									_selectSlots( pBeginSel, pEndSel, true );
	
									// Unselect the rest if not CTRL-click.
	
									if( !(pMsg->modKeys() & MODKEY_CTRL) )
									{
										ListSlot * pBegin = _beginSlots();
										ListSlot * pEnd = _endSlots();
										if( pBegin < pBeginSel )
											_unselectSlots( pBegin, pBeginSel, true );
										if( pEnd > pEndSel )
											_unselectSlots( pEndSel, pEnd, true );
									}
								}
								else
								{
									if( pMsg->modKeys() & MODKEY_CTRL )
									{
										// CTRL-click: We just flip the entry.
										_setSlotSelection( pEntry, _nextSlot(pEntry), !pEntry->pWidget->state().isSelected(), true );
									}
									else
									{
										//TODO: Not post unselected/selected for already selected pressed entry.
										_unselectSlots(_beginSlots(), _endSlots(), true);
										_selectSlot( pEntry, true );
									}
									m_pFocusedChild = pEntry->pWidget;
								}
								break;
						}
					}
					else if( m_selectMode == SelectMode::SingleEntry || m_selectMode == SelectMode::MultiEntries )
					{
						_unselectSlots(_beginSlots(), _endSlots(), true);
						m_pFocusedChild = nullptr;
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

	//____ _firstChild() __________________________________________________________

	Widget * List::_firstChild() const
	{
		ListSlot * p = _beginSlots();
		if (p == _endSlots())
			return nullptr;

		return p->pWidget;
	}

	//____ _lastChild() __________________________________________________________

	Widget * List::_lastChild() const
	{
		ListSlot * p = _endSlots();
		if (p == _beginSlots())
			return nullptr;

		return _prevSlot(p)->pWidget;
	}

	//____ _didAddSlots() _________________________________________________________

	void List::_didAddSlots(Slot * pSlot, int nb)
	{
		// Do nothing
	}

	//____ _willRemoveSlots() _________________________________________________________

	void List::_willRemoveSlots(Slot * _pSlot, int nb)
	{
		// Unselect slots that will be removed.

		ListSlot * pSlot = (ListSlot*) _pSlot;
		ListSlot * pEnd = (ListSlot*) (((char *)_pSlot) + m_sizeOfSlot*nb);
		_setSlotSelection(pSlot, pEnd, false, true);
	}

	//____ _hideSlots() _________________________________________________________

	void List::_hideSlots(ListSlot * pSlot, int nb)
	{
		for (int i = 0; i < nb; i++)
			pSlot[i].bVisible = false;
	}

	//____ _unhideSlots() _________________________________________________________

	void List::_unhideSlots(ListSlot * pSlot, int nb)
	{
		for (int i = 0; i < nb; i++)
			pSlot[i].bVisible = true;
	}

	//____ _setSlotSelection() _________________________________________________________

	int List::_setSlotSelection(ListSlot * pBegin, ListSlot * pEnd, bool bSelected, bool bPostMsg)
	{
		// Count slots to be selected (if we need to post a message)

		int nbChanges = 0;
		for (ListSlot * p = pBegin; p < pEnd; p = _nextSlot(p))
		{
			if (bSelected != p->pWidget->state().isSelected())
				nbChanges++;
		}

		ItemInfo * pItemInfo = 0;
		if( bPostMsg )
			pItemInfo = new ItemInfo[nbChanges];
		
		//

		int nbItems = 0;
		for (ListSlot * p = pBegin; p < pEnd; p = _nextSlot(p))
		{
			if( bSelected != p->pWidget->state().isSelected() )
			{
				State	state = p->pWidget->state();
				state.setSelected(bSelected);
				p->pWidget->_setState( state );
		
				if( bPostMsg )
				{
					pItemInfo[nbItems].pObject	= p->pWidget;
					pItemInfo[nbItems].id		= p->pWidget->id();		
					nbItems++;
				}
			}			
		}		
		
		// 

		if (bPostMsg)
		{
			if (bSelected)
				Base::msgRouter()->post(new ItemsSelectMsg(this, nbItems, pItemInfo));
			else
				Base::msgRouter()->post(new ItemsUnselectMsg(this, nbItems, pItemInfo));
		}

		return nbChanges;
	}


	
	
	
	//____ _flipSelection() _________________________________________________________

	int List::_flipSelection( ListSlot * pBegin, ListSlot * pEnd, bool bPostMsg )
	{
		int nSelected = 0;
		int nDeselected = 0;
		ListSlot * pLast = _prevSlot(pEnd);
	
		// Request render for the range
	
		Rect geoFirst;
		Rect geoLast;
		_getEntryGeo( geoFirst, pBegin );
		_getEntryGeo( geoLast, pLast );
		_requestRender( Rect::getUnion(geoFirst,geoLast) );
	
		// Reserve ItemInfo array of right size if we are going to post message
	
		ItemInfo * pSelectedItemsInfo = 0;
		ItemInfo * pDeselectedItemsInfo = 0;
		if( bPostMsg )
		{
			int nToSelect = 0;
			int nToDeselect = 0;
	
			for( ListSlot * pSlot = pBegin ; pSlot != pEnd ; pSlot = _nextSlot(pSlot) )
			{
				if( pSlot->pWidget->state().isSelected() )
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
	
		for( ListSlot * pSlot = pBegin ; pSlot != pEnd ; pSlot = _nextSlot(pSlot) )
		{
			State	state = pSlot->pWidget->state();
	
			state.setSelected(!state.isSelected());
			pSlot->pWidget->_setState( state );
	
			if( bPostMsg )
			{
				ItemInfo * p;
				if( !state.isSelected() )
					p = &pDeselectedItemsInfo[nSelected++];
				else
					p = &pSelectedItemsInfo[nDeselected++];
	
				p->pObject	= pSlot->pWidget;
				p->id		= pSlot->pWidget->id();
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
