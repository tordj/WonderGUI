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

#include <wg_flexpanel.h>
#include <wg_patches.h>
#include <wg_util.h>

#include <wg_slotarray.impl.h>
#include <wg_islotarray.impl.h>

#include <assert.h>

namespace wg
{
	using namespace Util;

	template class ISlotArray<FlexSlot>;

	template class SlotArray<FlexSlot>;

	const char FlexPanel::CLASSNAME[] = {"FlexPanel"};

	float	FlexPos::s_origoTab[9][2] = {	0.f, 0.f,
												0.5f, 0.f,
												1.f, 0.f,
												1.f, 0.5f,
												1.f, 1.f,
												0.5f, 1.f,
												0.f, 1.f,
												0.f, 0.5f,
												0.5f, 0.5f };


	//____ addPinned() _________________________________________________________

	IFlexSlots::iterator IFlexSlots::addPinned( Widget * pWidget, const FlexPos& topLeft, const FlexPos& bottomRight )
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		FlexSlot * pSlot = m_pSlotArray->add();
		pSlot->_setWidget(pWidget);

		pSlot->bPinned = true;
		pSlot->topLeftPin = topLeft;
		pSlot->bottomRightPin = bottomRight;

		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ addMovable() ________________________________________________________

	IFlexSlots::iterator IFlexSlots::addMovable( Widget * pWidget, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot )
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		FlexSlot * pSlot = m_pSlotArray->add();
		pSlot->_setWidget(pWidget);

		pSlot->placementGeo = qpixToRaw(geometry);
		pSlot->origo = origo;
		pSlot->hotspot = hotspot;

		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ insertPinned() ________________________________________________________

	IFlexSlots::iterator IFlexSlots::insertPinned( int index, Widget * pWidget, const FlexPos& topLeft, const FlexPos& bottomRight )
	{
		//TODO: Assert

		FlexSlot * pSlot = m_pSlotArray->insert(index);
		_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);

		pSlot->bPinned = true;
		pSlot->topLeftPin = topLeft;
		pSlot->bottomRightPin = bottomRight;

		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ insertMovable() ________________________________________________________

	IFlexSlots::iterator IFlexSlots::insertMovable( int index, Widget * pWidget, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot )
	{
		//TODO: Assert

		FlexSlot * pSlot = m_pSlotArray->insert(index);
		_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);

		pSlot->placementGeo = qpixToRaw(geometry);
		pSlot->origo = origo;
		pSlot->hotspot = hotspot;

		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ setPinned() ________________________________________________________

	void IFlexSlots::setPinned( int index )
	{
		//TODO: Assert

		_setPinned(m_pSlotArray->slot(index));
	}

	void IFlexSlots::setPinned(iterator it)
	{
		//TODO: Assert

		_setPinned(it._slot());
	}

	void IFlexSlots::setPinned(int index, const FlexPos& topLeft, const FlexPos& bottomRight)
	{
		//TODO: Assert

		_setPinned(m_pSlotArray->slot(index), topLeft, bottomRight);
	}

	void IFlexSlots::setPinned( iterator it, const FlexPos& topLeft, const FlexPos& bottomRight )
	{
		//TODO: Assert

		_setPinned(it._slot(), topLeft, bottomRight);
	}

	//____ setMovable() ________________________________________________________

	void IFlexSlots::setMovable( int index, const FlexPos& origo, const FlexPos& hotspot )
	{
		//TODO: Assert

		_setMovable(m_pSlotArray->slot(index), origo, hotspot);
	}

	void IFlexSlots::setMovable(iterator it, const FlexPos& origo, const FlexPos& hotspot)
	{
		//TODO: Assert

		_setMovable(it._slot(), origo, hotspot);
	}

	void IFlexSlots::setMovable(int index, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot)
	{
		//TODO: Assert

		_setMovable(m_pSlotArray->slot(index), qpixToRaw(geometry), origo, hotspot);
	}

	void IFlexSlots::setMovable( iterator it, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot )
	{
		//TODO: Assert

		_setMovable(it._slot(), qpixToRaw(geometry), origo, hotspot);
	}
/*
	//____ moveToBack() ________________________________________________________

	void IFlexSlots::moveToBack( int index )
	{
		//TODO: Assert

		_moveBelow( m_pSlotArray->slot(index),m_pSlotArray->first() );
	}

	IFlexSlots::iterator IFlexSlots::moveToBack(iterator it)
	{
		//TODO: Assert

		return iterator( _moveBelow(it._slot(), m_pSlotArray->first()) );
	}

	//____ moveToFront() ________________________________________________________

	void IFlexSlots::moveToFront( int index )
	{
		//TODO: Assert

		_moveAbove( m_pSlotArray->slot(index), m_pSlotArray->last() );
	}

	IFlexSlots::iterator IFlexSlots::moveToFront(iterator it)
	{
		//TODO: Assert

		return iterator( _moveAbove(it._slot(), m_pSlotArray->last()) );
	}

	//____ moveAbove() ________________________________________________________

	void IFlexSlots::moveAbove( int index, int sibling )
	{
		//TODO: Assert

		_moveAbove(m_pSlotArray->slot(index), m_pSlotArray->slot(sibling));
	}

	IFlexSlots::iterator IFlexSlots::moveAbove(iterator it, iterator sibling)
	{
		//TODO: Assert

		return iterator( _moveAbove(it._slot(), sibling._slot()) );
	}


	//____ moveBelow() ________________________________________________________

	void IFlexSlots::moveBelow(int index, int sibling)
	{
		//TODO: Assert!

		_moveBelow(m_pSlotArray->slot(index), m_pSlotArray->slot(sibling));
	}

	IFlexSlots::iterator IFlexSlots::moveBelow(iterator it, iterator sibling)
	{
		//TODO: Assert

		return iterator(_moveBelow(it._slot(), sibling._slot()));
	}
*/


	//____ isMovable() ________________________________________________________

	bool IFlexSlots::isMovable( int index ) const
	{
		//TODO: Assert!

		return !m_pSlotArray->slot(index)->bPinned;
	}

	bool IFlexSlots::isMovable(iterator it) const
	{
		//TODO: Assert!

		return !it._slot()->bPinned;
	}


	//____ isPinned() ________________________________________________________

	bool IFlexSlots::isPinned(int index) const
	{
		//TODO: Assert!

		return m_pSlotArray->slot(index)->bPinned;
	}

	bool IFlexSlots::isPinned( iterator it ) const
	{
		//TODO: Assert!

		return it._slot()->bPinned;
	}

	//____ setOrigo() ________________________________________________________

	bool IFlexSlots::setOrigo( int index, const FlexPos& origo )
	{
		//TODO: Assert!

		return _setOrigo(m_pSlotArray->slot(index), origo);
	}

	bool IFlexSlots::setOrigo( iterator it, const FlexPos& origo)
	{
		//TODO: Assert!

		return _setOrigo(it._slot(), origo);
	}


	//____ origo() ________________________________________________________

	FlexPos IFlexSlots::origo( int index ) const
	{
		//TODO: Assert!

		return _origo(m_pSlotArray->slot(index));
	}

	FlexPos IFlexSlots::origo( iterator it ) const
	{
		//TODO: Assert!

		return _origo(it._slot());
	}


	//____ setHotspot() ________________________________________________________

	bool IFlexSlots::setHotspot( int index, const FlexPos& hotspot )
	{
		//TODO: Assert!

		return _setHotspot(m_pSlotArray->slot(index), hotspot);
	}

	bool IFlexSlots::setHotspot(iterator it, const FlexPos& hotspot)
	{
		//TODO: Assert!

		return _setHotspot(it._slot(), hotspot);
	}

	//____ hotspot() ________________________________________________________

	FlexPos IFlexSlots::hotspot( int index ) const
	{
		//TODO: Assert!

		return _hotspot( m_pSlotArray->slot(index) );
	}

	FlexPos IFlexSlots::hotspot(iterator it) const
	{
		//TODO: Assert!

		return _hotspot(it._slot());
	}


	//____ setGeo() ________________________________________________________

	bool IFlexSlots::setGeo( int index, const Rect& geometry )
	{
		//TODO: Assert!

		return _setGeo(m_pSlotArray->slot(index), qpixToRaw(geometry));
	}

	bool IFlexSlots::setGeo(iterator it, const Rect& geometry)
	{
		//TODO: Assert!

		return _setGeo(it._slot(), qpixToRaw(geometry));
	}

	//____ geo() ________________________________________________________

	Rect IFlexSlots::geo( int index ) const
	{
		//TODO: Assert

		return rawToQpix(_geo(m_pSlotArray->slot(index)));
	}

	Rect IFlexSlots::geo( iterator it ) const
	{
		//TODO: Assert

		return rawToQpix(_geo(it._slot()));
	}


	//____ setOfs() ________________________________________________________

	bool IFlexSlots::setOfs( int index, const Coord& ofs )
	{
		//TODO: Assert

		return _setOfs(m_pSlotArray->slot(index), qpixToRaw(ofs));
	}

	bool IFlexSlots::setOfs( iterator it, const Coord& ofs )
	{
		//TODO: Assert

		return _setOfs(it._slot(), qpixToRaw(ofs));
	}

	//____ ofs() ________________________________________________________

	Coord IFlexSlots::ofs( int index ) const
	{
		//TODO: Assert

		return rawToQpix(_ofs(m_pSlotArray->slot(index)));
	}

	Coord IFlexSlots::ofs( iterator it ) const
	{
		//TODO: Assert

		return rawToQpix(_ofs(it._slot()));
	}

	//____ setSize() ________________________________________________________

	bool IFlexSlots::setSize( int index, const Size& size )
	{
		//TODO: Assert

		return _setSize(m_pSlotArray->slot(index), qpixToRaw(size) );
	}

	bool IFlexSlots::setSize( iterator it, const Size& size )
	{
		//TODO: Assert

		return _setSize(it._slot(), qpixToRaw(size) );
	}

	//____ size() ________________________________________________________

	Rect IFlexSlots::size( int index ) const
	{
		//TODO: Assert

		return rawToQpix(_size(m_pSlotArray->slot(index)));
	}

	Rect IFlexSlots::size( iterator it ) const
	{
		//TODO: Assert

		return rawToQpix(_size(it._slot()));
	}

	//____ move() ________________________________________________________

	bool IFlexSlots::move( int index, const Coord& ofs )
	{
		//TODO: Assert

		return _move(m_pSlotArray->slot(index), qpixToRaw(ofs));
	}

	bool IFlexSlots::move( iterator it, const Coord& ofs )
	{
		//TODO: Assert

		return _move(it._slot(), qpixToRaw(ofs));
	}

	//____ topLeftCorner() ________________________________________________________

	FlexPos IFlexSlots::topLeftCorner( int index ) const
	{
		//TODO: Assert

		return _topLeftCorner(m_pSlotArray->slot(index));
	}

	FlexPos IFlexSlots::topLeftCorner( iterator it ) const
	{
		//TODO: Assert

		return _topLeftCorner(it._slot());
	}

	//____ bottomRightCorner() ________________________________________________________

	FlexPos IFlexSlots::bottomRightCorner( int index ) const
	{
		//TODO: Assert

		return _bottomRightCorner(m_pSlotArray->slot(index));
	}

	FlexPos IFlexSlots::bottomRightCorner( iterator it ) const
	{
		//TODO: Assert

		return _bottomRightCorner(it._slot());
	}

	//____ _setPinned() ________________________________________________________

	void IFlexSlots::_setPinned(FlexSlot * p)
	{
		if (!p->bPinned)
		{
			p->bPinned = true;

			SizeI sz = _holder()->_size();

			p->topLeftPin = FlexPos(p->realGeo.x / (float)sz.w, p->realGeo.y / (float)sz.h);
			p->bottomRightPin = FlexPos(p->realGeo.right() / (float)sz.w, p->realGeo.bottom() / (float)sz.h);

			_holder()->_refreshRealGeo(p);
		}
	}

	void IFlexSlots::_setPinned(FlexSlot * p, const FlexPos& topLeft, const FlexPos& bottomRight)
	{
		p->bPinned = true;
		p->topLeftPin = topLeft;
		p->bottomRightPin = bottomRight;

		_holder()->_refreshRealGeo(p);
	}

	//____ _setMovable() ________________________________________________________

	void IFlexSlots::_setMovable(FlexSlot * p, const FlexPos& origo, const FlexPos& hotspot)
	{
		if (p->bPinned)
		{
			p->bPinned = false;
			p->origo = origo;
			p->hotspot = hotspot;
			p->placementGeo = p->realGeo - origo.rawPos(_holder()->_size()) + hotspot.rawPos(p->realGeo);

			_holder()->_refreshRealGeo(p);
		}
	}

	void IFlexSlots::_setMovable(FlexSlot * p, const RectI& geometry, const FlexPos& origo, const FlexPos& hotspot)
	{
		p->bPinned = false;
		p->origo = origo;
		p->hotspot = hotspot;
		p->placementGeo = geometry;

		_holder()->_refreshRealGeo(p);
	}

	//____ _setOrigo() ________________________________________________________

	bool IFlexSlots::_setOrigo(FlexSlot * p, const FlexPos& origo)
	{
		if (p->bPinned)
			return false;

		p->origo = origo;
		_holder()->_refreshRealGeo(p);

		return true;
	}

	//____ _origo() ________________________________________________________

	FlexPos IFlexSlots::_origo(FlexSlot * p) const
	{
		if (p->bPinned)
			return FlexPos();

		return p->origo;
	}

	//____ _setHotspot() ________________________________________________________

	bool IFlexSlots::_setHotspot(FlexSlot * p, const FlexPos& hotspot)
	{
		if (p->bPinned)
			return false;

		p->hotspot = hotspot;
		_holder()->_refreshRealGeo(p);

		return true;
	}

	//____ _hotspot() ________________________________________________________

	FlexPos IFlexSlots::_hotspot(FlexSlot * p) const
	{
		if (p->bPinned)
			return FlexPos();

		return p->hotspot;
	}

	//____ _setGeo() ________________________________________________________

	bool IFlexSlots::_setGeo(FlexSlot * p, const RectI& geometry)
	{
		if (p->bPinned)
			return false;

		p->placementGeo = geometry;
		_holder()->_refreshRealGeo(p);

		return true;
	}

	//____ _geo() ________________________________________________________

	RectI IFlexSlots::_geo(FlexSlot * p) const
	{
		if (p->bPinned)
			return RectI();

		return p->placementGeo;
	}

	//____ _setOfs() ________________________________________________________

	bool IFlexSlots::_setOfs( FlexSlot * p, const CoordI& ofs )
	{
		if( p->bPinned )
			return false;

		p->placementGeo.setPos(ofs);
		_holder()->_refreshRealGeo( p );

		return true;
	}

	//____ _ofs() ________________________________________________________

	CoordI IFlexSlots::_ofs( FlexSlot * p ) const
	{
		if( p->bPinned )
			return CoordI();

		return p->placementGeo.pos();
	}

	//____ _setSize() ________________________________________________________

	bool IFlexSlots::_setSize( FlexSlot * p, const SizeI& size )
	{
		if( p->bPinned )
			return false;

		p->placementGeo.setSize(size);
		_holder()->_refreshRealGeo( p );

		return true;
	}

	//____ _size() ________________________________________________________

	RectI IFlexSlots::_size( FlexSlot * p ) const
	{
		if( p->bPinned )
			return SizeI();

		return p->placementGeo.size();
	}

	//____ _move() ________________________________________________________

	bool IFlexSlots::_move( FlexSlot * p, const CoordI& ofs )
	{
		if( p->bPinned )
			return false;

		p->placementGeo += ofs;
		_holder()->_refreshRealGeo( p );

		return true;
	}

	//____ _topLeftCorner() ________________________________________________________

	FlexPos IFlexSlots::_topLeftCorner( FlexSlot * p ) const
	{
		if( !p->bPinned )
			return FlexPos();

		return p->topLeftPin;
	}

	//____ _bottomRightCorner() ________________________________________________________

	FlexPos IFlexSlots::_bottomRightCorner( FlexSlot * p ) const
	{
		if( !p->bPinned )
			return FlexPos();

		return p->bottomRightPin;
	}


	//____ Constructor ____________________________________________________________

	FlexPanel::FlexPanel() : m_children(this), children(&m_children,this)
	{
		m_bSiblingsOverlap = true;
	}

	//____ Destructor _____________________________________________________________

	FlexPanel::~FlexPanel()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool FlexPanel::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Panel::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * FlexPanel::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	FlexPanel_p FlexPanel::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return FlexPanel_p( static_cast<FlexPanel*>(pObject) );

		return 0;
	}

	//____ setConfineWidgets() ___________________________________________________

	void FlexPanel::setConfineWidgets( bool bConfineWidgets )
	{
		if( bConfineWidgets != m_bConfineWidgets )
		{
			m_bConfineWidgets = bConfineWidgets;

			FlexSlot * p = m_children.begin();

			while( p < m_children.end() )
			{
				_refreshRealGeo( p );
				p++;
			}
		}
	}

	//____ _preferredSize() _____________________________________________________________

	SizeI FlexPanel::_preferredSize() const
	{
		SizeI minSize;

		FlexSlot * p = m_children.begin();
		while( p < m_children.end() )
		{
			minSize = SizeI::max(minSize,_sizeNeededForGeo(p));
			p++;
		}
		return minSize;
	}


	//____ _didMoveSlots() ___________________________________________________________

	void FlexPanel::_didMoveSlots(BasicSlot * _pFrom, BasicSlot * _pTo, int nb)
	{
		if (nb > 1)
		{
			_requestRender();	//TODO: Optimize! Correctly calculate what is dirty even if more than one is moved.
			return;
		}

		FlexSlot * pFrom = static_cast<FlexSlot*>(_pFrom);
		FlexSlot * pTo = static_cast<FlexSlot*>(_pTo);

		if (pTo->bVisible)		// This is correct, we have already switched places...
		{
			if (pTo < pFrom)			// We were moved forward
			{
				// Request render on all areas covered by siblings we have skipped in front of.

				FlexSlot * p = pTo+1;
				while (p <= pFrom)
				{
					RectI cover(pTo->realGeo, p->realGeo);

					if (p->bVisible && !cover.isEmpty())
						_onRequestRender(cover, pTo);
					p++;
				}
			}
			else							// Move us backward
			{
				// Request render on our siblings for the area we previously have covered.

				FlexSlot * p = pFrom;
				while (p < pTo)
				{
					RectI cover(pTo->realGeo, p->realGeo);

					if (p->bVisible && !cover.isEmpty())
						_onRequestRender(cover, p);
					p++;
				}
			}
		}
	}


	//____ _didAddSlots() _____________________________________________________________

	void FlexPanel::_didAddSlots( BasicSlot * _pSlot, int nb )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);
		_unhideSlots(pSlot,nb);
	}

	//____ _willRemoveSlots() _____________________________________________________________

	void FlexPanel::_willRemoveSlots( BasicSlot * _pSlot, int nb )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);
		_hideSlots(pSlot,nb);
	}

	//____ _hideSlots() _____________________________________________________________

	void FlexPanel::_hideSlots( BasicSlot * _pSlot, int nb )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].bVisible == true )
			{
				_onRequestRender(pSlot[i].realGeo, &pSlot[i]);
				pSlot[i].bVisible = false;					// Needs to be done AFTER _onRequestRender()!
			}
		}
	}

	//____ _unhideSlots() _____________________________________________________________

	void FlexPanel::_unhideSlots( BasicSlot * _pSlot, int nb )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].bVisible == false )
			{
				pSlot[i].bVisible = true;
				_refreshRealGeo(&pSlot[i]);
				_onRequestRender(pSlot[i].realGeo, &pSlot[i]);
			}
		}
	}


	//____ _onRequestRender() ______________________________________________________

	void FlexPanel::_onRequestRender( const RectI& rect, const FlexSlot * pSlot )
	{
		if( !pSlot->bVisible )
			return;

		// Clip our geometry and put it in a dirtyrect-list

		Patches patches;
		patches.add( RectI( rect, RectI(0,0,m_size)) );

		// Remove portions of patches that are covered by opaque upper siblings

		for(FlexSlot * pCover = m_children.begin() ; pCover < pSlot ; pCover++ )
		{
			if( pCover->bVisible && pCover->realGeo.intersectsWith( rect ) )
				pCover->_widget()->_maskPatches( patches, pCover->realGeo, RectI(0,0,65536,65536 ), _getBlendMode() );
		}

		// Make request render calls

		for( const RectI * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}

	//____ _cloneContent() _______________________________________________________

	void FlexPanel::_cloneContent( const Widget * _pOrg )
	{
		Panel::_cloneContent( _pOrg );

		//TODO: Implement
	}

	//____ _resize() ____________________________________________________________

	void FlexPanel::_resize( const SizeI& size )
	{
		Panel::_resize(size);

		FlexSlot * p = m_children.begin();
		while( p < m_children.end() )
		{
			_refreshRealGeo(p);
			p++;
		}
	}

	//____ _childPos() ________________________________________________________

	CoordI FlexPanel::_childPos( BasicSlot * pSlot ) const
	{
		return ((FlexSlot*)pSlot)->realGeo.pos();
	}

	//____ _childRequestRender() _________________________________________________

	void FlexPanel::_childRequestRender( BasicSlot * _pSlot )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);
		_onRequestRender( pSlot->realGeo, pSlot );
	}

	void FlexPanel::_childRequestRender( BasicSlot * _pSlot, const RectI& rect )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);
		_onRequestRender( rect + pSlot->realGeo.pos(), pSlot );
	}

	//____ _childRequestResize() _________________________________________________

	void FlexPanel::_childRequestResize( BasicSlot * _pSlot )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);
		_refreshRealGeo(pSlot, true);
	}


	//____ _firstChild() __________________________________________________________

	Widget * FlexPanel::_firstChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.first()->_widget();
	}

	//____ _lastChild() __________________________________________________________

	Widget * FlexPanel::_lastChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.last()->_widget();
	}


	//____ _prevChild() __________________________________________________________

	Widget * FlexPanel::_prevChild( const BasicSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const FlexSlot*>(_pSlot);

		if (pSlot > m_children.begin())
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * FlexPanel::_nextChild( const BasicSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const FlexSlot*>(_pSlot);

		if (pSlot < m_children.last())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void FlexPanel::_releaseChild(BasicSlot * pSlot)
	{
		_willRemoveSlots(pSlot, 1);
		m_children.remove(static_cast<FlexSlot*>(pSlot));
	}

	//____ _replaceChild() _____________________________________________________

	void FlexPanel::_replaceChild(BasicSlot * _pSlot, Widget * pNewWidget)
	{
		auto pSlot = static_cast<FlexSlot*>(_pSlot);
		children._releaseGuardPointer(pNewWidget, &pSlot);

		pSlot->_setWidget(pNewWidget);

		if (pSlot->bVisible )
		{
			_refreshRealGeo(pSlot, true);
			_onRequestRender(pSlot->realGeo, pSlot);
		}
	}


	//____ _firstSlotWithGeo() _____________________________________________________

	void FlexPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( m_children.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			FlexSlot * pSlot = m_children.first();
			package.pSlot = pSlot;
			package.geo = pSlot->realGeo;
		}
	}

	//____ _nextSlotWithGeo() ______________________________________________________

	void FlexPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		FlexSlot * pSlot = (FlexSlot*) package.pSlot;

		if( pSlot == m_children.last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			package.geo = pSlot->realGeo;
		}
	}


	//____ _refreshRealGeo() ___________________________________________

	void FlexPanel::_refreshRealGeo( FlexSlot * pSlot, bool bForceResize )
	{
		RectI	geo;

		if( pSlot->bPinned )
		{
			CoordI topLeft = pSlot->topLeftPin.rawPos( m_size );
			CoordI bottomRight = pSlot->bottomRightPin.rawPos( m_size );

			geo = RectI(topLeft,bottomRight);

			// Respect widgets limits, apply in such a way that rectangle centers in specified rectangle

			SizeI sz = geo.size();
			sz.limit( pSlot->_minSize(), pSlot->_maxSize() );
			if( sz != geo.size() )
			{
				geo.x += geo.w - sz.w / 2;
				geo.y += geo.h - sz.h / 2;
				geo.w = sz.w;
				geo.h = sz.h;
			}
		}
		else
		{
			// Calculate size

			SizeI sz = pSlot->placementGeo.isEmpty() ? pSlot->_preferredSize() : pSlot->placementGeo.size();
			sz.limit( pSlot->_minSize(), pSlot->_maxSize() );		// Respect widgets limits.

			// Calculate position

			CoordI pos = pSlot->origo.rawPos( m_size );			// Origo,
			pos -= pSlot->hotspot.rawPos(sz);					// hotspot
			pos += pSlot->placementGeo.pos();				// and Offset.

			// Limit size/pos according to parent

			if( isConfiningWidgets() )
			{
				if( sz.w > m_size.w )
					sz.w = m_size.w;
				if( sz.h > m_size.h )

				if( pos.x + sz.w > m_size.w )
					pos.x = m_size.w - sz.w;

				if( pos.y + sz.h > m_size.h )
					pos.y = m_size.h - sz.h;
			}

			geo = RectI( pos, sz );
		}

		// Request render and update positions.

		if (geo != pSlot->realGeo)
		{
			_onRequestRender(pSlot->realGeo, pSlot);
			pSlot->realGeo = geo;
			_onRequestRender(pSlot->realGeo, pSlot);
		}

		if (bForceResize || pSlot->_size() != geo.size())
			pSlot->_setSize(geo);
	}

	//____ _sizeNeededForGeo() ________________________________________

	SizeI FlexPanel::_sizeNeededForGeo( FlexSlot * pSlot ) const
	{
		SizeI sz;

		if( pSlot->bPinned )
		{
			sz = pSlot->_preferredSize();

			sz += SizeI( pSlot->topLeftPin.offset.x, pSlot->topLeftPin.offset.y );
			sz -= SizeI( pSlot->bottomRightPin.offset.x, pSlot->bottomRightPin.offset.y );

			sz.w = (int) (sz.w / (float) (pSlot->bottomRightPin.origo.x - pSlot->topLeftPin.origo.x));
			sz.h = (int) (sz.w / (float) (pSlot->bottomRightPin.origo.y - pSlot->topLeftPin.origo.y));
		}
		else
		{
			RectI geo = pSlot->placementGeo;

			CoordI hotspot = pSlot->hotspot.rawPos(geo.size());
			CoordI offset = geo.pos() + pSlot->origo.offset - hotspot;

			int leftOfOrigo = 0 - offset.x;
			int rightOfOrigo = offset.x + geo.w;
			int aboveOrigo = 0 - offset.y;
			int belowOrigo = offset.y + geo.h;

			if( leftOfOrigo > 0 )
				sz.w = (int) (leftOfOrigo / pSlot->origo.origo.x);

			if( rightOfOrigo > 0 )
			{
				int w = (int) (rightOfOrigo / (1.f - pSlot->origo.origo.x) );
				if( sz.w < w )
					sz.w = w;
			}

			if( aboveOrigo > 0 )
				sz.h = (int) (aboveOrigo / pSlot->origo.origo.y);

			if( belowOrigo > 0 )
			{
				int h = (int) (belowOrigo / (1.f - pSlot->origo.origo.y) );
				if( sz.h < h )
					sz.h = h;
			}
		}

		return sz;
	}



} // namespace wg
