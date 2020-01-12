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

#include <wg_cdynamicslotvector.impl.h>

#include <assert.h>

namespace wg
{
	using namespace Util;

	template class CDynamicSlotVector<FlexSlot>;

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




	//____ FlexSlot::setPinned() _______________________________________________

	void FlexSlot::setPinned()
	{
		if (!m_bPinned)
		{
			m_bPinned = true;

			SizeI sz = _holder()->_size();

			m_topLeftPin = FlexPos(m_realGeo.x / (float)sz.w, m_realGeo.y / (float)sz.h);
			m_bottomRightPin = FlexPos(m_realGeo.right() / (float)sz.w, m_realGeo.bottom() / (float)sz.h);

			_holder()->_refreshRealGeo(this);
		}
	}

	void FlexSlot::setPinned(const FlexPos& topLeft, const FlexPos& bottomRight)
	{
		m_bPinned = true;
		m_topLeftPin = topLeft;
		m_bottomRightPin = bottomRight;

		_holder()->_refreshRealGeo(this);
	}

	//____ FlexSlot::setMovable() _______________________________________________

	void FlexSlot::setMovable(const FlexPos& origo, const FlexPos& hotspot)
	{
		if (m_bPinned)
		{
			m_bPinned = false;
			m_origo = origo;
			m_hotspot = hotspot;
			m_placementGeo = m_realGeo - origo.rawPos(_holder()->_size()) + hotspot.rawPos(m_realGeo);

			_holder()->_refreshRealGeo(this);
		}
	}

	void FlexSlot::setMovable(const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot)
	{
		m_bPinned = false;
		m_origo = origo;
		m_hotspot = hotspot;
		m_placementGeo = geometry;

		_holder()->_refreshRealGeo(this);
	}

	//____ FlexSlot::setOrigo() __________________________________________________

	bool FlexSlot::setOrigo(const FlexPos& origo)
	{
		if (m_bPinned)
			return false;

		m_origo = origo;
		_holder()->_refreshRealGeo(this);
		return true;
	}

	//____ FlexSlot::setHotspot() _________________________________________________

	bool FlexSlot::setHotspot(const FlexPos& hotspot)
	{
		if (m_bPinned)
			return false;

		m_hotspot = hotspot;
		_holder()->_refreshRealGeo(this);
		return true;
	}

	//____ FlexSlot::setGeo() ____________________________________________________

	bool FlexSlot::setGeo(const Rect& geometry)
	{
		if (m_bPinned)
			return false;

		m_placementGeo = qpixToRaw(geometry);
		_holder()->_refreshRealGeo(this);
		return true;
	}

	//____ FlexSlot::setOffset() __________________________________________________

	bool FlexSlot::setOffset(const Coord& ofs)
	{
		if (m_bPinned)
			return false;

		m_placementGeo.setPos(qpixToRaw(ofs));
		_holder()->_refreshRealGeo(this);
		return true;
	}

	//____ FlexSlot::setSize() _____________________________________________________

	bool FlexSlot::setSize(const Size& size)
	{
		if (m_bPinned)
			return false;

		m_placementGeo.setSize(qpixToRaw(size));
		_holder()->_refreshRealGeo(this);
		return true;
	}

	//____ FlexSlot::move() _______________________________________________________

	bool FlexSlot::move(const Coord& ofs)
	{
		if (m_bPinned)
			return false;

		m_placementGeo += qpixToRaw(ofs);
		_holder()->_refreshRealGeo(this);
		return true;
	}





	//____ pushFrontPinned() _________________________________________________________

	CFlexSlotVector::iterator CFlexSlotVector::pushFrontPinned( const Widget_p& pWidget, const FlexPos& topLeft, const FlexPos& bottomRight )
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		FlexSlot * pSlot = _pushFrontEmpty();
		pSlot->_setWidget(pWidget);

		pSlot->m_bPinned = true;
		pSlot->m_topLeftPin = topLeft;
		pSlot->m_bottomRightPin = bottomRight;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ pushFrontMovable() ________________________________________________________

	CFlexSlotVector::iterator CFlexSlotVector::pushFrontMovable( const Widget_p& pWidget, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot )
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		FlexSlot * pSlot = _pushFrontEmpty();
		pSlot->_setWidget(pWidget);

		pSlot->m_placementGeo = qpixToRaw(geometry);
		pSlot->m_origo = origo;
		pSlot->m_hotspot = hotspot;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ pushBackPinned() _________________________________________________________

	CFlexSlotVector::iterator CFlexSlotVector::pushBackPinned(const Widget_p& pWidget, const FlexPos& topLeft, const FlexPos& bottomRight)
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		FlexSlot * pSlot = _pushBackEmpty();
		pSlot->_setWidget(pWidget);

		pSlot->m_bPinned = true;
		pSlot->m_topLeftPin = topLeft;
		pSlot->m_bottomRightPin = bottomRight;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ pushBackMovable() ________________________________________________________

	CFlexSlotVector::iterator CFlexSlotVector::pushBackMovable(const Widget_p& pWidget, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot)
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		FlexSlot * pSlot = _pushBackEmpty();
		pSlot->_setWidget(pWidget);

		pSlot->m_placementGeo = qpixToRaw(geometry);
		pSlot->m_origo = origo;
		pSlot->m_hotspot = hotspot;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}


	//____ insertPinned() ________________________________________________________

	CFlexSlotVector::iterator CFlexSlotVector::insertPinned( int index, const Widget_p& pWidget, const FlexPos& topLeft, const FlexPos& bottomRight )
	{
		//TODO: Assert

		FlexSlot * pSlot = _insertEmpty(index);
		_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);

		pSlot->m_bPinned = true;
		pSlot->m_topLeftPin = topLeft;
		pSlot->m_bottomRightPin = bottomRight;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ insertMovable() ________________________________________________________

	CFlexSlotVector::iterator CFlexSlotVector::insertMovable( int index, const Widget_p& pWidget, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot )
	{
		//TODO: Assert

		FlexSlot * pSlot = _insertEmpty(index);
		_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);

		pSlot->m_placementGeo = qpixToRaw(geometry);
		pSlot->m_origo = origo;
		pSlot->m_hotspot = hotspot;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ Constructor ____________________________________________________________

	FlexPanel::FlexPanel() : slots(this)
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

			FlexSlot * p = slots._begin();

			while( p < slots._end() )
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

		FlexSlot * p = slots._begin();
		while( p < slots._end() )
		{
			minSize = SizeI::max(minSize,_sizeNeededForGeo(p));
			p++;
		}
		return minSize;
	}


	//____ _didMoveSlots() ___________________________________________________________

	void FlexPanel::_didMoveSlots(StaticSlot * _pFrom, StaticSlot * _pTo, int nb)
	{
		if (nb > 1)
		{
			_requestRender();	//TODO: Optimize! Correctly calculate what is dirty even if more than one is moved.
			return;
		}

		FlexSlot * pFrom = static_cast<FlexSlot*>(_pFrom);
		FlexSlot * pTo = static_cast<FlexSlot*>(_pTo);

		if (pTo->m_bVisible)		// This is correct, we have already switched places...
		{
			if (pTo < pFrom)			// We were moved forward
			{
				// Request render on all areas covered by siblings we have skipped in front of.

				FlexSlot * p = pTo+1;
				while (p <= pFrom)
				{
					RectI cover(pTo->m_realGeo, p->m_realGeo);

					if (p->m_bVisible && !cover.isEmpty())
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
					RectI cover(pTo->m_realGeo, p->m_realGeo);

					if (p->m_bVisible && !cover.isEmpty())
						_onRequestRender(cover, p);
					p++;
				}
			}
		}
	}


	//____ _didAddSlots() _____________________________________________________________

	void FlexPanel::_didAddSlots( StaticSlot * _pSlot, int nb )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);
		_unhideSlots(pSlot,nb);
	}

	//____ _willEraseSlots() _____________________________________________________________

	void FlexPanel::_willEraseSlots( StaticSlot * _pSlot, int nb )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);
		_hideSlots(pSlot,nb);
	}

	//____ _hideSlots() _____________________________________________________________

	void FlexPanel::_hideSlots( StaticSlot * _pSlot, int nb )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].m_bVisible == true )
			{
				_onRequestRender(pSlot[i].m_realGeo, &pSlot[i]);
				pSlot[i].m_bVisible = false;					// Needs to be done AFTER _onRequestRender()!
			}
		}
	}

	//____ _unhideSlots() _____________________________________________________________

	void FlexPanel::_unhideSlots( StaticSlot * _pSlot, int nb )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].m_bVisible == false )
			{
				pSlot[i].m_bVisible = true;
				_refreshRealGeo(&pSlot[i]);
				_onRequestRender(pSlot[i].m_realGeo, &pSlot[i]);
			}
		}
	}


	//____ _onRequestRender() ______________________________________________________

	void FlexPanel::_onRequestRender( const RectI& rect, const FlexSlot * pSlot )
	{
		if( !pSlot->m_bVisible )
			return;

		// Clip our geometry and put it in a dirtyrect-list

		Patches patches;
		patches.add( RectI( rect, RectI(0,0,m_size)) );

		// Remove portions of patches that are covered by opaque upper siblings

		for(FlexSlot * pCover = slots._begin() ; pCover < pSlot ; pCover++ )
		{
			if( pCover->m_bVisible && pCover->m_realGeo.intersectsWith( rect ) )
				pCover->_widget()->_maskPatches( patches, pCover->m_realGeo, RectI(0,0,65536,65536 ), _getBlendMode() );
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

		FlexSlot * p = slots._begin();
		while( p < slots._end() )
		{
			_refreshRealGeo(p);
			p++;
		}
	}

	//____ _childPos() ________________________________________________________

	CoordI FlexPanel::_childPos( const StaticSlot * pSlot ) const
	{
		return ((FlexSlot*)pSlot)->m_realGeo.pos();
	}

	//____ _childRequestRender() _________________________________________________

	void FlexPanel::_childRequestRender( StaticSlot * _pSlot )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);
		_onRequestRender( pSlot->m_realGeo, pSlot );
	}

	void FlexPanel::_childRequestRender( StaticSlot * _pSlot, const RectI& rect )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);
		_onRequestRender( rect + pSlot->m_realGeo.pos(), pSlot );
	}

	//____ _childRequestResize() _________________________________________________

	void FlexPanel::_childRequestResize( StaticSlot * _pSlot )
	{
		FlexSlot * pSlot = static_cast<FlexSlot*>(_pSlot);
		_refreshRealGeo(pSlot, true);
	}


	//____ _firstChild() __________________________________________________________

	Widget * FlexPanel::_firstChild() const
	{
		if (slots.isEmpty())
			return nullptr;

		return slots._first()->_widget();
	}

	//____ _lastChild() __________________________________________________________

	Widget * FlexPanel::_lastChild() const
	{
		if (slots.isEmpty())
			return nullptr;

		return slots._last()->_widget();
	}


	//____ _prevChild() __________________________________________________________

	Widget * FlexPanel::_prevChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const FlexSlot*>(_pSlot);

		if (pSlot > slots._begin())
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * FlexPanel::_nextChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const FlexSlot*>(_pSlot);

		if (pSlot < slots._last())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void FlexPanel::_releaseChild(StaticSlot * pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots._erase(static_cast<FlexSlot*>(pSlot));
	}

	//____ _replaceChild() _____________________________________________________

	void FlexPanel::_replaceChild(StaticSlot * _pSlot, Widget * pNewWidget)
	{
		auto pSlot = static_cast<FlexSlot*>(_pSlot);
		slots._releaseGuardPointer(pNewWidget, &pSlot);

		pSlot->_setWidget(pNewWidget);

		if (pSlot->m_bVisible )
		{
			_refreshRealGeo(pSlot, true);
			_onRequestRender(pSlot->m_realGeo, pSlot);
		}
	}


	//____ _firstSlotWithGeo() _____________________________________________________

	void FlexPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( slots.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			FlexSlot * pSlot = slots._first();
			package.pSlot = pSlot;
			package.geo = pSlot->m_realGeo;
		}
	}

	//____ _nextSlotWithGeo() ______________________________________________________

	void FlexPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		FlexSlot * pSlot = (FlexSlot*) package.pSlot;

		if( pSlot == slots._last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			package.geo = pSlot->m_realGeo;
		}
	}


	//____ _refreshRealGeo() ___________________________________________

	void FlexPanel::_refreshRealGeo( FlexSlot * pSlot, bool bForceResize )
	{
		RectI	geo;

		if( pSlot->m_bPinned )
		{
			CoordI topLeft = pSlot->m_topLeftPin.rawPos( m_size );
			CoordI bottomRight = pSlot->m_bottomRightPin.rawPos( m_size );

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

			SizeI sz = pSlot->m_placementGeo.isEmpty() ? pSlot->_preferredSize() : pSlot->m_placementGeo.size();
			sz.limit( pSlot->_minSize(), pSlot->_maxSize() );		// Respect widgets limits.

			// Calculate position

			CoordI pos = pSlot->m_origo.rawPos( m_size );			// Origo,
			pos -= pSlot->m_hotspot.rawPos(sz);					// hotspot
			pos += pSlot->m_placementGeo.pos();				// and Offset.

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

		if (geo != pSlot->m_realGeo)
		{
			_onRequestRender(pSlot->m_realGeo, pSlot);
			pSlot->m_realGeo = geo;
			_onRequestRender(pSlot->m_realGeo, pSlot);
		}

		if (bForceResize || pSlot->_size() != geo.size())
			pSlot->_setSize(geo);
	}

	//____ _sizeNeededForGeo() ________________________________________

	SizeI FlexPanel::_sizeNeededForGeo( FlexSlot * pSlot ) const
	{
		SizeI sz;

		if( pSlot->m_bPinned )
		{
			sz = pSlot->_preferredSize();

			sz += SizeI( pSlot->m_topLeftPin.offset.x, pSlot->m_topLeftPin.offset.y );
			sz -= SizeI( pSlot->m_bottomRightPin.offset.x, pSlot->m_bottomRightPin.offset.y );

			sz.w = (int) (sz.w / (float) (pSlot->m_bottomRightPin.origo.x - pSlot->m_topLeftPin.origo.x));
			sz.h = (int) (sz.w / (float) (pSlot->m_bottomRightPin.origo.y - pSlot->m_topLeftPin.origo.y));
		}
		else
		{
			RectI geo = pSlot->m_placementGeo;

			CoordI hotspot = pSlot->m_hotspot.rawPos(geo.size());
			CoordI offset = geo.pos() + pSlot->m_origo.offset - hotspot;

			int leftOfOrigo = 0 - offset.x;
			int rightOfOrigo = offset.x + geo.w;
			int aboveOrigo = 0 - offset.y;
			int belowOrigo = offset.y + geo.h;

			if( leftOfOrigo > 0 )
				sz.w = (int) (leftOfOrigo / pSlot->m_origo.origo.x);

			if( rightOfOrigo > 0 )
			{
				int w = (int) (rightOfOrigo / (1.f - pSlot->m_origo.origo.x) );
				if( sz.w < w )
					sz.w = w;
			}

			if( aboveOrigo > 0 )
				sz.h = (int) (aboveOrigo / pSlot->m_origo.origo.y);

			if( belowOrigo > 0 )
			{
				int h = (int) (belowOrigo / (1.f - pSlot->m_origo.origo.y) );
				if( sz.h < h )
					sz.h = h;
			}
		}

		return sz;
	}



} // namespace wg
