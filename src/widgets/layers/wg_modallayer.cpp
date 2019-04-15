/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_modallayer.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_msgrouter.h>
#include <wg_base.h>
#include <wg_slotarray.impl.h>
#include <wg_inputhandler.h>

#include <wg_idynamicchildren.h>
#include <wg_idynamicchildren.impl.h>


namespace wg
{
	INSTANTIATE_DYNAMICCHILDREN(ModalSlot, ModalChildrenHolder)

	template class SlotArray<ModalSlot>;

	const char ModalLayer::CLASSNAME[] = {"ModalLayer"};

	//TODO: Improve Modal geometry handling, should be able to run on PreferredSize by default, answering to resize-requests.

	//____ add() _________________________________________________________________

	IModalChildren::iterator IModalChildren::add(Widget * pWidget, const Rect& geometry, Origo origo)
	{
		//TODO: Assert

		ModalSlot * pSlot = m_pSlotArray->add();
		pSlot->geo = geometry;
		pSlot->origo = origo;

		pSlot->replaceWidget(m_pHolder->_widgetHolder(), pWidget);
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ setOrigo() ______________________________________________________________

	void IModalChildren::setOrigo(int index, const Origo origo)
	{
		//TODO: Assert

		_setOrigo( m_pSlotArray->slot(index), origo );
	}

	void IModalChildren::setOrigo(iterator it, const Origo origo)
	{
		//TODO: Assert

		_setOrigo( it._slot(), origo );
	}

	//____ origo() ______________________________________________________________

	Origo IModalChildren::origo(int index) const
	{
		//TODO: Assert

		return m_pSlotArray->slot(index)->origo;
	}

	Origo IModalChildren::origo(iterator it) const
	{
		//TODO: Assert

		return it._slot()->origo;
	}

	//____ setGeo() ______________________________________________________________

	void IModalChildren::setGeo(int index, const Rect& geometry)
	{
		//TODO: Assert

		_setGeo(m_pSlotArray->slot(index), geometry);
	}

	void IModalChildren::setGeo(iterator it, const Rect& geometry)
	{
		//TODO: Assert

		_setGeo(it._slot(), geometry);
	}

	//____ geo() _________________________________________________________________

	Rect IModalChildren::geo(int index) const
	{
		//TODO: Assert

		return m_pSlotArray->slot(index)->geo;
	}

	Rect IModalChildren::geo(iterator it) const
	{
		//TODO: Assert

		return it._slot()->geo;
	}

	//____ setOfs() ______________________________________________________________

	void IModalChildren::setOfs(int index, const Coord& ofs)
	{
		//TODO: Assert

		_setOfs(m_pSlotArray->slot(index), ofs);
	}

	void IModalChildren::setOfs(iterator it, const Coord& ofs)
	{
		//TODO: Assert

		_setOfs(it._slot(), ofs);
	}

	//____ ofs() _________________________________________________________________

	Coord IModalChildren::ofs(int index) const
	{
		//TODO: Assert

		return m_pSlotArray->slot(index)->geo.pos();
	}

	Coord IModalChildren::ofs(iterator it) const
	{
		//TODO: Assert

		return it._slot()->geo.pos();
	}

	//____ setSize() __________________________________________________________

	void IModalChildren::setSize(int index, const Size& size)
	{
		//TODO: Assert

		_setSize(m_pSlotArray->slot(index), size);
	}

	void IModalChildren::setSize(iterator it, const Size& size)
	{
		//TODO: Assert

		_setSize(it._slot(), size);
	}

	//____ size() ______________________________________________________________

	Size IModalChildren::size( int index ) const
	{
		//TODO: Assert

		return m_pSlotArray->slot(index)->geo.size();
	}

	Size IModalChildren::size( iterator it ) const
	{
		//TODO: Assert

		return it._slot()->geo.size();
	}

	//____ move() ______________________________________________________________

	void IModalChildren::move( int index, const Coord& ofs )
	{
		//TODO: Assert

		_move(m_pSlotArray->slot(index), ofs);
	}

	void IModalChildren::move( iterator it, const Coord& ofs )
	{
		//TODO: Assert

		_move(it._slot(), ofs);
	}

	//____ _setOrigo() ______________________________________________________________

	void IModalChildren::_setOrigo(ModalSlot * p, const Origo origo)
	{
		p->origo = origo;
		m_pHolder->_refreshRealGeo(p);
	}

	//____ _setGeo() ______________________________________________________________

	void IModalChildren::_setGeo(ModalSlot * p, const Rect& geometry)
	{
		p->placementGeo = geometry;
		m_pHolder->_refreshRealGeo(p);
	}

	//____ _setOfs() ______________________________________________________________

	void IModalChildren::_setOfs(ModalSlot * p, const Coord& ofs)
	{
		p->placementGeo.setPos(ofs);
		m_pHolder->_refreshRealGeo(p);
	}
	//____ _setSize() __________________________________________________________

	void IModalChildren::_setSize(ModalSlot * p, const Size& size)
	{
		p->placementGeo.setSize(size);
		m_pHolder->_refreshRealGeo(p);
	}

	//____ _move() ______________________________________________________________

	void IModalChildren::_move( ModalSlot * p, const Coord& ofs )
	{
		p->placementGeo += ofs;
		m_pHolder->_refreshRealGeo(p);
	}

	//____ _refreshRealGeo() __________________________________________________

	void ModalLayer::_refreshRealGeo( ModalSlot * pSlot )	// Return false if we couldn't get exactly the requested (floating) geometry.
	{
		Size sz = pSlot->placementGeo.size();

		if( sz.w == 0 && sz.h == 0 )
			sz = pSlot->pWidget->preferredSize();
		else if( sz.w == 0 )
			sz.w = pSlot->pWidget->matchingWidth(sz.h);
		else if( sz.h == 0 )
			sz.h = pSlot->pWidget->matchingHeight(sz.w);

		if( sz.w <= 0 )
			sz.w = 1;
		if( sz.h <= 0 )
			sz.h = 1;

		Coord ofs = Util::origoToOfs( pSlot->origo, m_size ) - Util::origoToOfs( pSlot->origo, sz );
		ofs += pSlot->placementGeo.pos();

		Rect newGeo( ofs, sz );

		if( newGeo != pSlot->geo )
		{
			_onRequestRender(pSlot->geo, pSlot);
			pSlot->geo = Rect( ofs, sz );
			_onRequestRender(pSlot->geo, pSlot);
		}
	}

	//____ _childRequestResize() ______________________________________________

	void ModalLayer::_childRequestResize( Slot * pSlot )
	{
		if( pSlot == &m_baseSlot )
			_requestResize();
		else
			_refreshRealGeo( (ModalSlot *) pSlot );
	}

	//____ _releaseChild() ____________________________________________________

	void ModalLayer::_releaseChild(Slot * pSlot)
	{
		if (pSlot == &m_baseSlot)
			Layer::_releaseChild(pSlot);
		else
			_willRemoveSlots(pSlot, 1);
	}

	//____ Constructor ____________________________________________________________

	ModalLayer::ModalLayer() : modals(&m_modals, this)
	{
	}

	//____ Destructor _____________________________________________________________

	ModalLayer::~ModalLayer()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool ModalLayer::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Layer::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * ModalLayer::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	ModalLayer_p ModalLayer::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ModalLayer_p( static_cast<ModalLayer*>(pObject) );

		return 0;
	}

	//____ matchingHeight() _______________________________________________________

	int ModalLayer::matchingHeight( int width ) const
	{
		if( m_baseSlot.pWidget )
			return m_baseSlot.pWidget->matchingHeight( width );
		else
			return Widget::matchingHeight(width);
	}

	//____ matchingWidth() _______________________________________________________

	int ModalLayer::matchingWidth( int height ) const
	{
		if( m_baseSlot.pWidget )
			return m_baseSlot.pWidget->matchingWidth( height );
		else
			return Widget::matchingWidth(height);
	}

	//____ preferredSize() _____________________________________________________________

	Size ModalLayer::preferredSize() const
	{
		if( m_baseSlot.pWidget )
			return m_baseSlot.pWidget->preferredSize();
		else
			return Size(1,1);
	}

	//____ _findWidget() ____________________________________________________________

	Widget *  ModalLayer::_findWidget( const Coord& ofs, SearchMode mode )
	{
		// In search mode ACTION_TARGET we always return either the topmost non-hidden modal Widget (or its children),
		// or us.

		if( mode == SearchMode::ActionTarget )
		{
			ModalSlot * pSlot = m_modals.last();

			if( pSlot )
			{
				if( pSlot->pWidget->isContainer() )
				{
					Widget * pResult = static_cast<Container*>(pSlot->pWidget)->_findWidget( ofs - pSlot->geo.pos(), mode );
					if( pResult )
						return pResult;
				}
				else
				{
					if( pSlot->pWidget->markTest(ofs - pSlot->geo.pos()) )
						return pSlot->pWidget;
					else
						return this;
				}
			}
			else if( m_baseSlot.pWidget )
			{
				if( m_baseSlot.pWidget->isContainer() )
				{
					Widget * pResult = static_cast<Container*>(m_baseSlot.pWidget)->_findWidget( ofs, mode );
					if( pResult )
						return pResult;
				}
				else
					return m_baseSlot.pWidget;
			}
			else
				return 0;
		}

		// For the rest of the modes we can rely on the default method.

		return Container::_findWidget( ofs, mode );
	}

	//____ _updateKeyboardFocus() _______________________________________________________

	void ModalLayer::_updateKeyboardFocus()
	{
		// Get message handler, verify that we have a root

		if( !_holder() || !_holder()->_root() )
			return;

		// Retrieve focused Widget and verify it being a descendant to us.

		Widget * pFocused = Base::inputHandler()->focusedWidget().rawPtr();

		Widget * p = pFocused;
		while( p && p->parent() && p->parent().rawPtr() != this )
			p = p->_parent();

		if( p && p->_parent() != this )
			return;								// Focus belongs to a Widget that is not a descendant to us,
												// so we can't save and shouldn't steal focus.

		// Save old focus so we can return it properly in the future.
		if( p )
		{
			if( p == m_baseSlot.pWidget )
				m_pBaseKeyFocus = pFocused;
			else
			{
				ModalSlot * pSlot = static_cast<ModalSlot*>(p->_slot());
				pSlot->pKeyFocus = pFocused;
			}
		}

		// Find which child-branch to focus and switch to our previously saved focus

		ModalSlot * pSlot = m_modals.last();

		Widget * 	pSavedFocus = nullptr;
		Slot *		pBranch	= nullptr;

		while( pSlot > m_modals.first() && pSlot->geo.isEmpty() )
			pSlot--;

		if( pSlot > m_modals.first() )
		{
			pSavedFocus = pSlot->pKeyFocus.rawPtr();
			pSlot->pKeyFocus = nullptr;								// Needs to be cleared for the future.
			pBranch = pSlot;
		}
		else if( m_baseSlot.pWidget )
		{
			pSavedFocus = m_pBaseKeyFocus.rawPtr();
			m_pBaseKeyFocus = 0;								// Needs to be cleared for the future.
			pBranch = &m_baseSlot;
		}

		// Verify that saved focus still is within branch and is not hidden

		Widget * pW = pSavedFocus;
		while( pW && pW->_parent() != this )
			pW = pW->_parent();

		if( pW )
		{
			Slot * pSlot = (Slot*) pW->_slot();
			if( pSlot != pBranch )
				pSavedFocus = 0;				// Previously focused Widget is no longer a child of focused branch.
		}

		// Switch to previously saved focus, or null if not applicable

		_holder()->_childRequestFocus( m_pSlot, pSavedFocus );
	}


	//____ _didAddSlots() __________________________________________________

	void ModalLayer::_didAddSlots(Slot * _pSlot, int nb)
	{
		ModalSlot * pSlot = (ModalSlot*) _pSlot;
		for( int i = 0 ; i < nb ; i++ )
			_refreshRealGeo(&pSlot[i]);

		_updateKeyboardFocus();
	}

	//____ _didMoveSlots() ___________________________________________________________

	void ModalLayer::_didMoveSlots(Slot * _pFrom, Slot * _pTo, int nb)
	{
		if (nb > 1)
		{
			_requestRender();	//TODO: Optimize! Correctly calculate what is dirty even if more than one is moved.
			return;
		}

		auto pFrom = static_cast<ModalSlot*>(_pFrom);
		auto pTo = static_cast<ModalSlot*>(_pTo);


		// Now we have switched places, pTo contains the widget that was moved

		if (pTo < pFrom)			// We were moved forward
		{
			// Request render on all areas covered by siblings we have skipped in front of.

			ModalSlot * p = pTo+1;
			while (p <= pFrom)
			{
				Rect cover(pTo->geo, p->geo);

				if (!cover.isEmpty())
					_onRequestRender(cover, pTo);
				p++;
			}
		}
		else							// Move us backward
		{
			// Request render on our siblings for the area we previously have covered.

			ModalSlot * p = pFrom;
			while (p < pTo)
			{
				Rect cover(pTo->geo, p->geo);

				if (!cover.isEmpty())
					_onRequestRender(cover, p);
				p++;
			}
		}
	}


	//____ _willRemoveSlots() __________________________________________________

	void ModalLayer::_willRemoveSlots(Slot * _pSlot, int nb)
	{
		ModalSlot * pSlot = (ModalSlot*) _pSlot;
		for( int i = 0 ; i < nb ; i++ )
		{
			_childRequestRender(pSlot+i);
			pSlot[i].geo = { 0,0,0,0 };
		}

		_updateKeyboardFocus();
	}

	//____ _beginLayerSlots() __________________________________________________

	const LayerSlot * ModalLayer::_beginLayerSlots() const
	{
		return m_modals.begin();
	}

	//____ _endLayerSlots() ____________________________________________________

	const LayerSlot *  ModalLayer::_endLayerSlots() const
	{
		return m_modals.end();
	}

	//____ _sizeOfLayerSlot() __________________________________________________

	int ModalLayer::_sizeOfLayerSlot() const
	{
		return sizeof(ModalSlot);
	}


	//____ _setSize() ___________________________________________________________

	void ModalLayer::_setSize( const Size& sz )
	{
		Layer::_setSize(sz);

		// Refresh modal widgets geometry, their positions might have changed.

		for( auto pSlot = m_modals.begin() ; pSlot != m_modals.end() ; pSlot++ )
			_refreshRealGeo( pSlot );
	}

	//____ _cloneContent() ______________________________________________________

	void ModalLayer::_cloneContent( const Widget * _pOrg )
	{
		Layer::_cloneContent( _pOrg );
	}

	//____ _receive() ______________________________________________________________

	void ModalLayer::_receive( Msg * _pMsg )
	{
		Layer::_receive(_pMsg);

		if( _pMsg->isInstanceOf( InputMsg::CLASSNAME ) )
		{
			InputMsg_p pMsg = InputMsg::cast(_pMsg);

			if( !m_modals.isEmpty() && _findWidget( pMsg->pointerPos(), SearchMode::ActionTarget ) == this )
			{
				switch( pMsg->type() )
				{
					case MsgType::MousePress:
					{
						MouseButtonMsg_p pMsg = MouseButtonMsg::cast(_pMsg);
						Base::msgRouter()->post( new ModalBlockedPressMsg( pMsg->inputId(), pMsg->button(), this, pMsg->modKeys(), pMsg->pointerPos(), pMsg->timestamp()) );
					}
					break;

					case MsgType::MouseRelease:
					{
						MouseButtonMsg_p pMsg = MouseButtonMsg::cast(_pMsg);
						Base::msgRouter()->post( new ModalBlockedPressMsg( pMsg->inputId(), pMsg->button(), this, pMsg->modKeys(), pMsg->pointerPos(), pMsg->timestamp()) );
					}
					break;

					case MsgType::MouseMove:
					{
						Base::msgRouter()->post( new ModalMoveOutsideMsg( pMsg->inputId(), this, pMsg->modKeys(), pMsg->pointerPos(), pMsg->timestamp()) );
					}
					break;

					default:
						break;
				}
			}
		}
	}


} // namespace wg
