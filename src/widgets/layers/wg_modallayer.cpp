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
#include <wg_inputhandler.h>
#include <wg_internal.h>

#include <wg_cdynamicslotvector.impl.h>


namespace wg
{
	using namespace Util;

	template class CDynamicSlotVector<ModalLayer::Slot>;

	const TypeInfo ModalLayer::TYPEINFO = { "ModalLayer", &Layer::TYPEINFO };
	const TypeInfo ModalLayer::Slot::TYPEINFO = { "ModalLayer::Slot", &Layer::Slot::TYPEINFO };

	//TODO: Improve Modal geometry handling, should be able to run on PreferredSize by default, answering to resize-requests.


	//____ Slot::setOrigo() ____________________________________________________

	void ModalLayer::Slot::setOrigo(const Origo origo)
	{
		m_origo = origo;
		_holder()->_refreshRealGeo(this);
	}

	//____ Slot::setGeo() ____________________________________________________

	void ModalLayer::Slot::setGeo(const Rect& geometry)
	{
		m_placementGeo = geometry;
		_holder()->_refreshRealGeo(this);
	}

	//____ Slot::setOffset() ____________________________________________________

	void ModalLayer::Slot::setOffset(const Coord& ofs)
	{
		m_placementGeo.setPos(ofs);
		_holder()->_refreshRealGeo(this);
	}

	//____ Slot::setSize() ____________________________________________________

	void ModalLayer::Slot::setSize(const Size& size)
	{
		m_placementGeo.setSize(size);
		_holder()->_refreshRealGeo(this);
	}

	//____ Slot::move() ____________________________________________________

	void ModalLayer::Slot::move(const Coord& ofs)
	{
		m_placementGeo += ofs;
		_holder()->_refreshRealGeo(this);
	}


	//____ pushFront() _________________________________________________________________

	ModalLayer::CSlots::iterator ModalLayer::CSlots::pushFront(const Widget_p& pWidget, const Rect& geometry, Origo origo)
	{
		//TODO: Assert

		pWidget->releaseFromParent();								// Always release first, in case widget already was in our array.

		Slot * pSlot = _pushFrontEmpty();
		pSlot->m_geo = geometry;
		pSlot->m_origo = origo;

		pSlot->_setWidget(pWidget);
		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ pushBack() _________________________________________________________________

	ModalLayer::CSlots::iterator ModalLayer::CSlots::pushBack(const Widget_p& pWidget, const Rect& geometry, Origo origo)
	{
		//TODO: Assert

		pWidget->releaseFromParent();								// Always release first, in case widget already was in our array.

		Slot * pSlot = _pushBackEmpty();
		pSlot->m_geo = geometry;
		pSlot->m_origo = origo;

		pSlot->_setWidget(pWidget);
		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ _refreshRealGeo() __________________________________________________

	void ModalLayer::_refreshRealGeo( Slot * pSlot, bool bForceResize )	// Return false if we couldn't get exactly the requested (floating) geometry.
	{
		Size sz = pSlot->m_placementGeo.size();

		if( sz.w == 0 && sz.h == 0 )
			sz = pSlot->_widget()->preferredSize();
		else if( sz.w == 0 )
			sz.w = pSlot->_widget()->matchingWidth(sz.h);
		else if( sz.h == 0 )
			sz.h = pSlot->_widget()->matchingHeight(sz.w);

		if( sz.w <= 0 )
			sz.w = 1;
		if( sz.h <= 0 )
			sz.h = 1;

		Coord ofs = Util::origoToOfs( pSlot->m_origo, m_size ) - Util::origoToOfs( pSlot->m_origo, sz );
		ofs += pSlot->m_placementGeo.pos();

		Rect geo( ofs, sz );

		if (geo != pSlot->m_geo)
		{
			_onRequestRender(pSlot->m_geo, pSlot);
			pSlot->m_geo = geo;
			_onRequestRender(pSlot->m_geo, pSlot);
		}

		if (bForceResize || pSlot->size() != geo.size())
			pSlot->_setSize(geo);
	}

	//____ _childRequestResize() ______________________________________________

	void ModalLayer::_childRequestResize( StaticSlot * pSlot )
	{
		if( pSlot == &mainSlot )
			_requestResize();
		else
		{
			auto p = static_cast<Slot*>(pSlot);
			_refreshRealGeo( p, true );
		}
	}

	//____ _releaseChild() ____________________________________________________

	void ModalLayer::_releaseChild(StaticSlot * pSlot)
	{
		if (pSlot == &mainSlot)
			Layer::_releaseChild(pSlot);
		else
		{
			_willEraseSlots(pSlot, 1);
			modalSlots._erase((Slot*)pSlot);
		}
	}

	//____ constructor ____________________________________________________________

	ModalLayer::ModalLayer() : modalSlots(this)
	{
	}

	//____ Destructor _____________________________________________________________

	ModalLayer::~ModalLayer()
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& ModalLayer::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ matchingHeight() _______________________________________________________

	MU ModalLayer::matchingHeight( MU width ) const
	{
		if( mainSlot._widget() )
			return mainSlot._widget()->matchingHeight( width );
		else
			return Widget::matchingHeight(width);
	}

	//____ matchingWidth() _______________________________________________________

	MU ModalLayer::matchingWidth( MU height ) const
	{
		if( mainSlot._widget() )
			return mainSlot._widget()->matchingWidth( height );
		else
			return Widget::matchingWidth(height);
	}

	//____ preferredSize() _____________________________________________________________

	Size ModalLayer::preferredSize() const
	{
		if( mainSlot._widget() )
			return mainSlot._widget()->preferredSize();
		else
			return Size(1,1);
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	ModalLayer::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		if (pSlot == &mainSlot)
			return DynamicSlot::TYPEINFO;

		return Slot::TYPEINFO;
	}

	//____ _findWidget() ____________________________________________________________

	Widget *  ModalLayer::_findWidget( const Coord& ofs, SearchMode mode )
	{
		// In search mode ACTION_TARGET we always return either the topmost non-hidden modal Widget (or its children),
		// or us.

		if( mode == SearchMode::ActionTarget )
		{
			if( !modalSlots.isEmpty() )
			{
				Slot * pSlot = modalSlots._last();

				if( pSlot->_widget()->isContainer() )
				{
					Widget * pResult = static_cast<OContainer*>(pSlot->_widget())->_findWidget( ofs - pSlot->m_geo.pos(), mode );
					if( pResult )
						return pResult;
				}
				else
				{
					if( pSlot->_widget()->markTest(ofs - pSlot->m_geo.pos()) )
						return pSlot->_widget();
					else
						return this;
				}
			}
			else if( mainSlot._widget() )
			{
				Widget * pWidget = mainSlot._widget();

				if( pWidget->isContainer() )
				{
					Widget * pResult = static_cast<OContainer*>(pWidget)->_findWidget( ofs, mode );
					if( pResult )
						return pResult;
				}
				else
					return pWidget;
			}

			return nullptr;
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
			p = OO(p)->_parent();

		if( p && OO(p)->_parent() != this )
			return;								// Focus belongs to a Widget that is not a descendant to us,
												// so we can't save and shouldn't steal focus.

		// Save old focus so we can return it properly in the future.
		if( p )
		{
			if( p == mainSlot._widget() )
				m_pBaseKeyFocus = pFocused;
			else
			{
				Slot * pSlot = static_cast<Slot*>(OO(p)->_slot());
				pSlot->m_pKeyFocus = pFocused;
			}
		}

		// Find which child-branch to focus and switch to our previously saved focus

		Slot * pSlot = modalSlots._last();

		Widget * 	pSavedFocus = nullptr;
		StaticSlot *		pBranch	= nullptr;

		while( pSlot >= modalSlots._first() && pSlot->m_geo.isEmpty() )
			pSlot--;

		if( pSlot >= modalSlots._first() )
		{
			pSavedFocus = pSlot->m_pKeyFocus.rawPtr();
			pSlot->m_pKeyFocus = nullptr;								// Needs to be cleared for the future.
			pBranch = pSlot;
		}
		else if( mainSlot._widget() )
		{
			pSavedFocus = m_pBaseKeyFocus.rawPtr();
			m_pBaseKeyFocus = nullptr;								// Needs to be cleared for the future.
			pBranch = &mainSlot;
		}

		// Verify that saved focus still is within branch and is not hidden

		Widget * pW = pSavedFocus;
		while( pW && OO(pW)->_parent() != this )
			pW = OO(pW)->_parent();

		if( pW )
		{
			StaticSlot * pSlot = (StaticSlot*) OO(pW)->_slot();
			if( pSlot != pBranch )
				pSavedFocus = 0;				// Previously focused Widget is no longer a child of focused branch.
		}

		// Switch to previously saved focus, or null if not applicable

		_holder()->_childRequestFocus( m_pSlot, pSavedFocus );
	}

	//____ _didAddSlots() __________________________________________________

	void ModalLayer::_didAddSlots(StaticSlot * _pSlot, int nb)
	{
		Slot * pSlot = (Slot*) _pSlot;
		for( int i = 0 ; i < nb ; i++ )
			_refreshRealGeo(&pSlot[i]);

		_updateKeyboardFocus();
	}

	//____ _didMoveSlots() ___________________________________________________________

	void ModalLayer::_didMoveSlots(StaticSlot * _pFrom, StaticSlot * _pTo, int nb)
	{
		if (nb > 1)
		{
			_requestRender();	//TODO: Optimize! Correctly calculate what is dirty even if more than one is moved.
			return;
		}

		auto pFrom = static_cast<Slot*>(_pFrom);
		auto pTo = static_cast<Slot*>(_pTo);


		// Now we have switched places, pTo contains the widget that was moved

		if (pTo < pFrom)			// We were moved forward
		{
			// Request render on all areas covered by siblings we have skipped in front of.

			Slot * p = pTo+1;
			while (p <= pFrom)
			{
				Rect cover(pTo->m_geo, p->m_geo);

				if (!cover.isEmpty())
					_onRequestRender(cover, pTo);
				p++;
			}
		}
		else							// Move us backward
		{
			// Request render on our siblings for the area we previously have covered.

			Slot * p = pFrom;
			while (p < pTo)
			{
				Rect cover(pTo->m_geo, p->m_geo);

				if (!cover.isEmpty())
					_onRequestRender(cover, p);
				p++;
			}
		}
	}


	//____ _willEraseSlots() __________________________________________________

	void ModalLayer::_willEraseSlots(StaticSlot * _pSlot, int nb)
	{
		Slot * pSlot = (Slot*) _pSlot;
		for( int i = 0 ; i < nb ; i++ )
		{
			_childRequestRender(pSlot+i);
			pSlot[i].m_geo.clear();
		}

		_updateKeyboardFocus();
	}

	//____ _hideSlots() _____________________________________________________________

	void ModalLayer::_hideSlots(StaticSlot * _pSlot, int nb)
	{
		// TODO: Implement!!!
	}

	//____ _unhideSlots() _____________________________________________________________

	void ModalLayer::_unhideSlots(StaticSlot * _pSlot, int nb)
	{
		// TODO: Implement!!!
	}


	//____ _beginLayerSlots() __________________________________________________

	const Layer::Layer::Slot * ModalLayer::_beginLayerSlots() const
	{
		return modalSlots._begin();
	}

	//____ _endLayerSlots() ____________________________________________________

	const Layer::Layer::Slot *  ModalLayer::_endLayerSlots() const
	{
		return modalSlots._end();
	}

	//____ _sizeOfLayerSlot() __________________________________________________

	int ModalLayer::_sizeOfLayerSlot() const
	{
		return sizeof(Slot);
	}


	//____ _resize() ___________________________________________________________

	void ModalLayer::_resize( const Size& sz )
	{
		Layer::_resize(sz);

		// Refresh modal widgets geometry, their positions might have changed.

		for( auto pSlot = modalSlots._begin() ; pSlot != modalSlots._end() ; pSlot++ )
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

		if( _pMsg->isInstanceOf( InputMsg::TYPEINFO ) )
		{
			InputMsg * pMsg = static_cast<InputMsg*>(_pMsg);

			if( !modalSlots.isEmpty() && _findWidget( pMsg->pointerPos(), SearchMode::ActionTarget ) == this )
			{
				switch( pMsg->type() )
				{
					case MsgType::MousePress:
					{
						MouseButtonMsg_p pMsg = static_cast<MouseButtonMsg*>(_pMsg);
						Base::msgRouter()->post( new ModalBlockedPressMsg( pMsg->inputId(), pMsg->button(), this, pMsg->modKeys(), pMsg->pointerPos(), pMsg->timestamp()) );
					}
					break;

					case MsgType::MouseRelease:
					{
						MouseButtonMsg_p pMsg = static_cast<MouseButtonMsg*>(_pMsg);
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
