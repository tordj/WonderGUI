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
#include <wg_cicondisplay.h>
#include <wg_ctextdisplay.h>
#include <wg_gfxdevice.h>
#include <wg_internal.h>

namespace wg
{


	const TypeInfo List::TYPEINFO = { "List", &Container::TYPEINFO };
	const TypeInfo List::Slot::TYPEINFO = { "List::Slot", &SelectableSlot::TYPEINFO };



	//____ constructor ____________________________________________________________

	List::List() : lasso(this), m_selectMode(SelectMode::SingleEntry)
	{
	}

	//____ Destructor _____________________________________________________________

	List::~List()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& List::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setEntrySkin() _________________________________________________________

	void List::setEntrySkin( Skin * pSkin )
	{
		Size oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->contentPaddingSize() : Size();

		m_pEntrySkin[0] = pSkin;
		m_pEntrySkin[1] = pSkin;
		m_bOpaqueEntries = pSkin ? pSkin->isOpaque() : false;

		_onEntrySkinChanged( oldPadding, pSkin ? pSkin->contentPaddingSize() : Size() );
	}

	bool List::setEntrySkin( Skin * pOddEntrySkin, Skin * pEvenEntrySkin )
	{
//		SizeI oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->_contentPaddingSize() : SizeI();
		Size padding[2];

		if( pOddEntrySkin )
			padding[0] = pOddEntrySkin->contentPaddingSize();

		if( pEvenEntrySkin )
			padding[1] = pEvenEntrySkin->contentPaddingSize();

		if( (padding[0].w != padding[1].w) || (padding[0].h != padding[1].h) )
			return false;

		m_pEntrySkin[0] = pOddEntrySkin;
		m_pEntrySkin[1] = pEvenEntrySkin;
		m_bOpaqueEntries = (pOddEntrySkin->isOpaque() && pEvenEntrySkin->isOpaque());

		_onEntrySkinChanged( padding[0], pOddEntrySkin ? pOddEntrySkin->contentPaddingSize() : Size() );
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
				MouseMoveMsg * pMsg = static_cast<MouseMoveMsg*>(_pMsg);
				Slot * pEntry = _findEntry(toLocal(pMsg->pointerPos()));
				if( pEntry && pEntry->_widget() != m_pHoveredChild.rawPtr() )
				{
					Rect geo;
					if( m_pHoveredChild )
					{
						_getEntryGeo( geo, (Slot*) OO(m_pHoveredChild)->_slot() );
						_requestRender(geo);
					}

					_getEntryGeo( geo, pEntry );
					_requestRender(geo);
					m_pHoveredChild = pEntry->_widget();
				}
				break;
			}
			case MsgType::MouseLeave:
			{
				MouseLeaveMsg_p pMsg = static_cast<MouseLeaveMsg*>(_pMsg);
				Slot * pEntry = _findEntry(toLocal(pMsg->pointerPos()));
				if( m_pHoveredChild && !pEntry )
				{
					Rect geo;
					_getEntryGeo( geo, (Slot*) OO(m_pHoveredChild)->_slot() );
					_requestRender(geo);
					m_pHoveredChild = nullptr;
				}
				break;
			}
			case MsgType::MousePress:
			{
				grabFocus();

				MouseButtonMsg_p pMsg = static_cast<MouseButtonMsg*>(_pMsg);
				if( m_selectMode != SelectMode::Unselectable && pMsg->button() == MouseButton::Left )
				{
					Coord ofs = toLocal(pMsg->pointerPos());
					if( !_listWindow().contains(ofs) )
						break;								// Click on header or somewhere else outside the real list.

					Rect listArea = _listArea();
					Slot * pEntry = _findEntry(ofs);

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
								if( !pEntry->_widget()->state().isSelected() )
								{
									_unselectSlots( _beginSlots(), _endSlots(), true );
									_selectSlot( pEntry, true );
									m_pFocusedChild = pEntry->_widget();
								}
								break;
							case SelectMode::FlipOnSelect:
								_setSlotSelection( pEntry, _nextSlot(pEntry), !pEntry->m_bSelected, true );
								m_pFocusedChild = pEntry->_widget();
								break;
							case SelectMode::MultiEntries:
								if( pMsg->modKeys() & MODKEY_SHIFT && m_pFocusedChild )
								{
									// Select range from focused to clicked entry.

									Slot * pFocused = (Slot*) OO(m_pFocusedChild)->_slot();
									Slot * pBeginSel = wg::min( pEntry, pFocused );
									Slot * pEndSel = _nextSlot(wg::max( pEntry, pFocused ));
									_selectSlots( pBeginSel, pEndSel, true );

									// Unselect the rest if not CTRL-click.

									if( !(pMsg->modKeys() & MODKEY_CTRL) )
									{
										Slot * pBegin = _beginSlots();
										Slot * pEnd = _endSlots();
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
										_setSlotSelection( pEntry, _nextSlot(pEntry), !pEntry->m_bSelected, true );
									}
									else
									{
										//TODO: Not post unselected/selected for already selected pressed entry.
										_unselectSlots(_beginSlots(), _endSlots(), true);
										_selectSlot( pEntry, true );
									}
									m_pFocusedChild = pEntry->_widget();
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
				if( m_selectMode != SelectMode::Unselectable &&  static_cast<MouseReleaseMsg*>(_pMsg)->button() == MouseButton::Left )
				{
					Rect dirtyRect( m_lassoBegin, m_lassoEnd );
					_requestRender(dirtyRect);

					m_lassoBegin = m_lassoEnd;
					_pMsg->swallow();
				}
				break;
			case MsgType::MouseClick:
				if( m_selectMode != SelectMode::Unselectable &&  static_cast<MouseClickMsg*>(_pMsg)->button() == MouseButton::Left )
					_pMsg->swallow();
				break;
			case MsgType::MouseDoubleClick:
			case MsgType::MouseRepeat:
				if( m_selectMode != SelectMode::Unselectable &&  static_cast<MouseButtonMsg*>(_pMsg)->button() == MouseButton::Left )
					_pMsg->swallow();
				break;
			case MsgType::MouseDrag:
			{
				auto pMsg = static_cast<MouseDragMsg*>(_pMsg);
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
		Slot * p = _beginSlots();
		if (p == _endSlots())
			return nullptr;

		return p->_widget();
	}

	//____ _lastChild() __________________________________________________________

	Widget * List::_lastChild() const
	{
		Slot * p = _endSlots();
		if (p == _beginSlots())
			return nullptr;

		return _prevSlot(p)->_widget();
	}

	//____ _setSlotSelection() _________________________________________________________

	int List::_setSlotSelection(Slot * pBegin, Slot * pEnd, bool bSelected, bool bPostMsg)
	{
		// Count slots to be selected (if we need to post a message)

		int nbChanges = 0;
		for (Slot * p = pBegin; p < pEnd; p = _nextSlot(p))
		{
			if (p->m_bVisible && bSelected != p->m_bSelected && (p->_widget()->isSelectable() || bSelected == false) )
				nbChanges++;
		}

		if (nbChanges == 0)
			return 0;

		ItemInfo * pItemInfo = 0;
		if( bPostMsg )
			pItemInfo = new ItemInfo[nbChanges];

		//

		int nbItems = 0;
		for (Slot * p = pBegin; p < pEnd; p = _nextSlot(p))
		{
			if( p->m_bVisible && bSelected != p->m_bSelected && (p->_widget()->isSelectable() || bSelected == false))
			{
				p->m_bSelected = bSelected;

				State	state = p->_widget()->state();
				state.setSelected(bSelected);
				OO(p->_widget())->_setState( state );

				if( bPostMsg )
				{
					pItemInfo[nbItems].pObject	= p->_widget();
					pItemInfo[nbItems].id		= p->_widget()->id();
					nbItems++;
				}
			}
		}

		//

		if (bPostMsg)
		{
			if (bSelected)
				Base::msgRouter()->post(ItemsSelectMsg::create(this, nbItems, pItemInfo));
			else
				Base::msgRouter()->post(ItemsUnselectMsg::create(this, nbItems, pItemInfo));
		}

		return nbChanges;
	}





	//____ _flipSelection() _________________________________________________________

	int List::_flipSelection( Slot * pBegin, Slot * pEnd, bool bPostMsg )
	{
		int nSelected = 0;
		int nDeselected = 0;
		Slot * pLast = _prevSlot(pEnd);

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

			for( Slot * pSlot = pBegin ; pSlot != pEnd ; pSlot = _nextSlot(pSlot) )
			{
				if (pSlot->m_bVisible)
				{
					if( pSlot->m_bSelected )
						nToDeselect++;
					else if( pSlot->_widget()->isSelectable() )
						nToSelect++;
				}
			}

			if( nToSelect > 0 )
				pSelectedItemsInfo = new ItemInfo[nToSelect];
			if( nToDeselect > 0 )
				pDeselectedItemsInfo = new ItemInfo[nToDeselect];
		}

		// Loop through entries

		for( Slot * pSlot = pBegin ; pSlot != pEnd ; pSlot = _nextSlot(pSlot) )
		{

			if (pSlot->m_bVisible && (pSlot->_widget()->isSelectable() || pSlot->m_bSelected))
			{
				pSlot->m_bSelected = !pSlot->m_bSelected;
				State	state = pSlot->_widget()->state();
				state.setSelected(pSlot->m_bSelected);
				OO(pSlot->_widget())->_setState(state);

				if (bPostMsg)
				{
					ItemInfo * p;
					if (pSlot->m_bSelected)
						p = &pSelectedItemsInfo[nSelected++];
					else
						p = &pDeselectedItemsInfo[nDeselected++];

					p->pObject = pSlot->_widget();
					p->id = pSlot->_widget()->id();
				}
			}
		}

		// Post message

		if( bPostMsg )
		{
			if( nSelected > 0 )
				Base::msgRouter()->post( ItemsSelectMsg::create(this, nSelected, pSelectedItemsInfo) );

			if( nDeselected > 0 )
				Base::msgRouter()->post( ItemsUnselectMsg::create(this, nDeselected, pDeselectedItemsInfo) );
		}

		return nSelected + nDeselected;
	}

	//____ _componentState() __________________________________________________

	State List::_componentState(const GeoComponent* pComponent) const
	{
		if (pComponent == &lasso)
			return StateEnum::Normal;
		else
			return m_state;
	}

	//____ _componentPos() ____________________________________________________

	Coord List::_componentPos(const GeoComponent* pComponent) const
	{
		if (pComponent == &lasso)
			return m_lassoBegin;
		else
			return Coord();
	}

	//____ _componentSize() ___________________________________________________

	Size List::_componentSize(const GeoComponent* pComponent) const
	{
		if (pComponent == &lasso)
			return Size(m_lassoEnd - m_lassoBegin);
		else
			return m_size;

	}

	//____ _componentGeo() ____________________________________________________

	Rect List::_componentGeo(const GeoComponent* pComponent) const
	{
		if (pComponent == &lasso)
			return Rect(m_lassoBegin, m_lassoEnd);
		else
			return m_size;
	}

	//____ _componentRequestRender() __________________________________________

	void List::_componentRequestRender(const GeoComponent* pComponent)
	{
		if (pComponent == &lasso)
			return _requestRender( Rect(m_lassoBegin, m_lassoEnd) );
		else
			return _requestRender();

	}

	void List::_componentRequestRender(const GeoComponent* pComponent, const Rect& rect)
	{
		if (pComponent == &lasso)
			return _requestRender( rect + m_lassoBegin );
		else
			return _requestRender(rect);
	}

} // namespace wg
