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

#include <wg_cdynamicslotarray.impl.h>


namespace wg
{
	using namespace Util;

	template class CDynamicSlotArray<ModalSlot>;

	const char ModalLayer::CLASSNAME[] = {"ModalLayer"};

	//TODO: Improve Modal geometry handling, should be able to run on PreferredSize by default, answering to resize-requests.

	//____ add() _________________________________________________________________

	CModalSlotArray::iterator CModalSlotArray::add(Widget * pWidget, const Rect& geometry, Origo origo)
	{
		//TODO: Assert

		pWidget->releaseFromParent();								// Always release first, in case widget already was in our array.

		ModalSlot * pSlot = _addEmpty();
		pSlot->geo = qpixToRaw(geometry);
		pSlot->origo = origo;

		pSlot->_setWidget(pWidget);
		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ setOrigo() ______________________________________________________________

	void CModalSlotArray::setOrigo(int index, const Origo origo)
	{
		//TODO: Assert

		_setOrigo( _slot(index), origo );
	}

	void CModalSlotArray::setOrigo(iterator it, const Origo origo)
	{
		//TODO: Assert

		_setOrigo( it._slot(), origo );
	}

	//____ origo() ______________________________________________________________

	Origo CModalSlotArray::origo(int index) const
	{
		//TODO: Assert

		return _slot(index)->origo;
	}

	Origo CModalSlotArray::origo(iterator it) const
	{
		//TODO: Assert

		return it._slot()->origo;
	}

	//____ setGeo() ______________________________________________________________

	void CModalSlotArray::setGeo(int index, const Rect& geometry)
	{
		//TODO: Assert

		_setGeo(_slot(index), geometry);
	}

	void CModalSlotArray::setGeo(iterator it, const Rect& geometry)
	{
		//TODO: Assert

		_setGeo(it._slot(), geometry);
	}

	//____ geo() _________________________________________________________________

	Rect CModalSlotArray::geo(int index) const
	{
		//TODO: Assert

		return rawToQpix( _slot(index)->geo );
	}

	Rect CModalSlotArray::geo(iterator it) const
	{
		//TODO: Assert

		return rawToQpix(it._slot()->geo);
	}

	//____ setOfs() ______________________________________________________________

	void CModalSlotArray::setOfs(int index, const Coord& ofs)
	{
		//TODO: Assert

		_setOfs(_slot(index), ofs);
	}

	void CModalSlotArray::setOfs(iterator it, const Coord& ofs)
	{
		//TODO: Assert

		_setOfs(it._slot(), ofs);
	}

	//____ ofs() _________________________________________________________________

	Coord CModalSlotArray::ofs(int index) const
	{
		//TODO: Assert

		return rawToQpix(_slot(index)->geo.pos());
	}

	Coord CModalSlotArray::ofs(iterator it) const
	{
		//TODO: Assert

		return rawToQpix(it._slot()->geo.pos());
	}

	//____ setSize() __________________________________________________________

	void CModalSlotArray::setSize(int index, const Size& size)
	{
		//TODO: Assert

		_setSize(_slot(index), size);
	}

	void CModalSlotArray::setSize(iterator it, const Size& size)
	{
		//TODO: Assert

		_setSize(it._slot(), size);
	}

	//____ size() ______________________________________________________________

	Size CModalSlotArray::size( int index ) const
	{
		//TODO: Assert

		return rawToQpix(_slot(index)->geo.size());
	}

	Size CModalSlotArray::size( iterator it ) const
	{
		//TODO: Assert

		return rawToQpix(it._slot()->geo.size());
	}

	//____ move() ______________________________________________________________

	void CModalSlotArray::move( int index, const Coord& ofs )
	{
		//TODO: Assert

		_move(_slot(index), ofs);
	}

	void CModalSlotArray::move( iterator it, const Coord& ofs )
	{
		//TODO: Assert

		_move(it._slot(), ofs);
	}

	//____ _setOrigo() ______________________________________________________________

	void CModalSlotArray::_setOrigo(ModalSlot * p, const Origo origo)
	{
		p->origo = origo;
		_holder()->_refreshRealGeo(p);
	}

	//____ _setGeo() ______________________________________________________________

	void CModalSlotArray::_setGeo(ModalSlot * p, const Rect& geometry)
	{
		p->placementGeo = qpixToRaw(geometry);
		_holder()->_refreshRealGeo(p);
	}

	//____ _setOfs() ______________________________________________________________

	void CModalSlotArray::_setOfs(ModalSlot * p, const Coord& ofs)
	{
		p->placementGeo.setPos(qpixToRaw(ofs));
		_holder()->_refreshRealGeo(p);
	}
	//____ _setSize() __________________________________________________________

	void CModalSlotArray::_setSize(ModalSlot * p, const Size& size)
	{
		p->placementGeo.setSize(qpixToRaw(size));
		_holder()->_refreshRealGeo(p);
	}

	//____ _move() ______________________________________________________________

	void CModalSlotArray::_move( ModalSlot * p, const Coord& ofs )
	{
		p->placementGeo += qpixToRaw(ofs);
		_holder()->_refreshRealGeo(p);
	}

	//____ _refreshRealGeo() __________________________________________________

	void ModalLayer::_refreshRealGeo( ModalSlot * pSlot, bool bForceResize )	// Return false if we couldn't get exactly the requested (floating) geometry.
	{
		SizeI sz = pSlot->placementGeo.size();

		if( sz.w == 0 && sz.h == 0 )
			sz = pSlot->_preferredSize();
		else if( sz.w == 0 )
			sz.w = pSlot->_matchingWidth(sz.h);
		else if( sz.h == 0 )
			sz.h = pSlot->_matchingHeight(sz.w);

		if( sz.w <= 0 )
			sz.w = 1;
		if( sz.h <= 0 )
			sz.h = 1;

		CoordI ofs = Util::origoToOfs( pSlot->origo, m_size ) - Util::origoToOfs( pSlot->origo, sz );
		ofs += pSlot->placementGeo.pos();

		RectI geo( ofs, sz );

		if (geo != pSlot->geo)
		{
			_onRequestRender(pSlot->geo, pSlot);
			pSlot->geo = geo;
			_onRequestRender(pSlot->geo, pSlot);
		}

		if (bForceResize || pSlot->_size() != geo.size())
			pSlot->setSize(geo);
	}

	//____ _childRequestResize() ______________________________________________

	void ModalLayer::_childRequestResize( StaticSlot * pSlot )
	{
		if( pSlot == &mainSlot )
			_requestResize();
		else
		{
			auto p = static_cast<ModalSlot*>(pSlot);
			_refreshRealGeo( p, true );
		}
	}

	//____ _releaseChild() ____________________________________________________

	void ModalLayer::_releaseChild(StaticSlot * pSlot)
	{
		if (pSlot == &mainSlot)
			Layer::_releaseChild(pSlot);
		else
			_willEraseSlots(pSlot, 1);
	}

	//____ Constructor ____________________________________________________________

	ModalLayer::ModalLayer() : modalSlots(this)
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

	//____ _matchingHeight() _______________________________________________________

	int ModalLayer::_matchingHeight( int width ) const
	{
		if( _baseSlot()->_widget() )
			return _baseSlot()->_matchingHeight( width );
		else
			return Widget::_matchingHeight(width);
	}

	//____ _matchingWidth() _______________________________________________________

	int ModalLayer::_matchingWidth( int height ) const
	{
		if( _baseSlot()->_widget() )
			return _baseSlot()->_matchingWidth( height );
		else
			return Widget::_matchingWidth(height);
	}

	//____ _preferredSize() _____________________________________________________________

	SizeI ModalLayer::_preferredSize() const
	{
		if( _baseSlot()->_widget() )
			return _baseSlot()->_preferredSize();
		else
			return SizeI(1,1);
	}

	//____ _findWidget() ____________________________________________________________

	Widget *  ModalLayer::_findWidget( const CoordI& ofs, SearchMode mode )
	{
		// In search mode ACTION_TARGET we always return either the topmost non-hidden modal Widget (or its children),
		// or us.

		if( mode == SearchMode::ActionTarget )
		{
			if( !modalSlots.isEmpty() )
			{
				ModalSlot * pSlot = modalSlots._last();

				if( pSlot->_widget()->isContainer() )
				{
					Widget * pResult = static_cast<Container*>(pSlot->_widget())->_findWidget( ofs - pSlot->geo.pos(), mode );
					if( pResult )
						return pResult;
				}
				else
				{
					if( pSlot->_markTest(ofs - pSlot->geo.pos()) )
						return pSlot->_widget();
					else
						return this;
				}
			}
			else if( _baseSlot()->_widget() )
			{
				if( _baseSlot()->_widget()->isContainer() )
				{
					Widget * pResult = static_cast<Container*>(_baseSlot()->_widget())->_findWidget( ofs, mode );
					if( pResult )
						return pResult;
				}
				else
					return _baseSlot()->_widget();
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
			p = p->_parent();

		if( p && p->_parent() != this )
			return;								// Focus belongs to a Widget that is not a descendant to us,
												// so we can't save and shouldn't steal focus.

		// Save old focus so we can return it properly in the future.
		if( p )
		{
			if( p == _baseSlot()->_widget() )
				m_pBaseKeyFocus = pFocused;
			else
			{
				ModalSlot * pSlot = static_cast<ModalSlot*>(p->_slot());
				pSlot->pKeyFocus = pFocused;
			}
		}

		// Find which child-branch to focus and switch to our previously saved focus

		ModalSlot * pSlot = modalSlots._last();

		Widget * 	pSavedFocus = nullptr;
		StaticSlot *		pBranch	= nullptr;

		while( pSlot >= modalSlots._first() && pSlot->geo.isEmpty() )
			pSlot--;

		if( pSlot >= modalSlots._first() )
		{
			pSavedFocus = pSlot->pKeyFocus.rawPtr();
			pSlot->pKeyFocus = nullptr;								// Needs to be cleared for the future.
			pBranch = pSlot;
		}
		else if( _baseSlot()->_widget() )
		{
			pSavedFocus = m_pBaseKeyFocus.rawPtr();
			m_pBaseKeyFocus = nullptr;								// Needs to be cleared for the future.
			pBranch = &mainSlot;
		}

		// Verify that saved focus still is within branch and is not hidden

		Widget * pW = pSavedFocus;
		while( pW && pW->_parent() != this )
			pW = pW->_parent();

		if( pW )
		{
			StaticSlot * pSlot = (StaticSlot*) pW->_slot();
			if( pSlot != pBranch )
				pSavedFocus = 0;				// Previously focused Widget is no longer a child of focused branch.
		}

		// Switch to previously saved focus, or null if not applicable

		_holder()->_childRequestFocus( m_pSlot, pSavedFocus );
	}

	//____ _didAddSlots() __________________________________________________

	void ModalLayer::_didAddSlots(StaticSlot * _pSlot, int nb)
	{
		ModalSlot * pSlot = (ModalSlot*) _pSlot;
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

		auto pFrom = static_cast<ModalSlot*>(_pFrom);
		auto pTo = static_cast<ModalSlot*>(_pTo);


		// Now we have switched places, pTo contains the widget that was moved

		if (pTo < pFrom)			// We were moved forward
		{
			// Request render on all areas covered by siblings we have skipped in front of.

			ModalSlot * p = pTo+1;
			while (p <= pFrom)
			{
				RectI cover(pTo->geo, p->geo);

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
				RectI cover(pTo->geo, p->geo);

				if (!cover.isEmpty())
					_onRequestRender(cover, p);
				p++;
			}
		}
	}


	//____ _willEraseSlots() __________________________________________________

	void ModalLayer::_willEraseSlots(StaticSlot * _pSlot, int nb)
	{
		ModalSlot * pSlot = (ModalSlot*) _pSlot;
		for( int i = 0 ; i < nb ; i++ )
		{
			_childRequestRender(pSlot+i);
			pSlot[i].geo = { 0,0,0,0 };
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

	const LayerSlot * ModalLayer::_beginLayerSlots() const
	{
		return modalSlots._begin();
	}

	//____ _endLayerSlots() ____________________________________________________

	const LayerSlot *  ModalLayer::_endLayerSlots() const
	{
		return modalSlots._end();
	}

	//____ _sizeOfLayerSlot() __________________________________________________

	int ModalLayer::_sizeOfLayerSlot() const
	{
		return sizeof(ModalSlot);
	}


	//____ _setSize() ___________________________________________________________

	void ModalLayer::_resize( const SizeI& sz )
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

		if( _pMsg->isInstanceOf( InputMsg::CLASSNAME ) )
		{
			InputMsg_p pMsg = InputMsg::cast(_pMsg);

			if( !modalSlots.isEmpty() && _findWidget( pMsg->pointerPosRaw(), SearchMode::ActionTarget ) == this )
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
