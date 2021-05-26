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
#include <wg_internal.h>

#include <wg_cdynamicslotvector.impl.h>

#include <assert.h>

namespace wg
{
	using namespace Util;

	template class CDynamicSlotVector<FlexPanel::Slot>;

	const TypeInfo FlexPanel::TYPEINFO = { "FlexPanel", &Panel::TYPEINFO };
	const TypeInfo FlexPanel::Slot::TYPEINFO = { "FlexPanel::Slot", &DynamicSlot::TYPEINFO };

	float	FlexPos::s_origoTab[9][2] = {	0.f, 0.f,
												0.5f, 0.f,
												1.f, 0.f,
												1.f, 0.5f,
												1.f, 1.f,
												0.5f, 1.f,
												0.f, 1.f,
												0.f, 0.5f,
												0.5f, 0.5f };




	//____ Slot::setPinned() _______________________________________________

	void FlexPanel::Slot::setPinned()
	{
		if (!m_bPinned)
		{
			m_bPinned = true;

			SizeSPX sz = _holder()->_size();

			m_topLeftPin = FlexPos(m_realGeo.x / (float)sz.w, m_realGeo.y / (float)sz.h);
			m_bottomRightPin = FlexPos(m_realGeo.right() / (float)sz.w, m_realGeo.bottom() / (float)sz.h);

			_holder()->_refreshRealGeo(this);
		}
	}

	void FlexPanel::Slot::setPinned(const FlexPos& topLeft, const FlexPos& bottomRight)
	{
		m_bPinned = true;
		m_topLeftPin = topLeft;
		m_bottomRightPin = bottomRight;

		_holder()->_refreshRealGeo(this);
	}

	//____ Slot::setMovable() _______________________________________________

	void FlexPanel::Slot::setMovable(const FlexPos& origo, const FlexPos& hotspot)
	{
		if (m_bPinned)
		{
			int scale = _holder()->_scale();
			Rect geo = spxToPts(m_realGeo, scale);


			m_bPinned = false;
			m_origo = origo;
			m_hotspot = hotspot;
			m_placementGeo = geo - origo.pos(_holder()->size()) + hotspot.pos(geo);

			_holder()->_refreshRealGeo(this);
		}
	}

	void FlexPanel::Slot::setMovable(const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot)
	{
		m_bPinned = false;
		m_origo = origo;
		m_hotspot = hotspot;
		m_placementGeo = geometry;

		_holder()->_refreshRealGeo(this);
	}

	//____ Slot::setOrigo() __________________________________________________

	bool FlexPanel::Slot::setOrigo(const FlexPos& origo)
	{
		if (m_bPinned)
			return false;

		m_origo = origo;
		_holder()->_refreshRealGeo(this);
		return true;
	}

	//____ Slot::setHotspot() _________________________________________________

	bool FlexPanel::Slot::setHotspot(const FlexPos& hotspot)
	{
		if (m_bPinned)
			return false;

		m_hotspot = hotspot;
		_holder()->_refreshRealGeo(this);
		return true;
	}

	//____ Slot::setGeo() ____________________________________________________

	bool FlexPanel::Slot::setGeo(const Rect& geometry)
	{
		if (m_bPinned)
			return false;

		m_placementGeo = geometry;
		_holder()->_refreshRealGeo(this);
		return true;
	}

	//____ Slot::setOffset() __________________________________________________

	bool FlexPanel::Slot::setOffset(const Coord& ofs)
	{
		if (m_bPinned)
			return false;

		m_placementGeo.setPos(ofs);
		_holder()->_refreshRealGeo(this);
		return true;
	}

	//____ Slot::setSize() _____________________________________________________

	bool FlexPanel::Slot::setSize(const Size& size)
	{
		if (m_bPinned)
			return false;

		m_placementGeo.setSize(size);
		_holder()->_refreshRealGeo(this);
		return true;
	}

	//____ Slot::move() _______________________________________________________

	bool FlexPanel::Slot::move(const Coord& ofs)
	{
		if (m_bPinned)
			return false;

		m_placementGeo += ofs;
		_holder()->_refreshRealGeo(this);
		return true;
	}





	//____ pushFrontPinned() _________________________________________________________

	FlexPanel::CSlots::iterator FlexPanel::CSlots::pushFrontPinned( const Widget_p& pWidget, const FlexPos& topLeft, const FlexPos& bottomRight )
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		Slot * pSlot = _pushFrontEmpty();
		pSlot->_setWidget(pWidget);

		pSlot->m_bPinned = true;
		pSlot->m_topLeftPin = topLeft;
		pSlot->m_bottomRightPin = bottomRight;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ pushFrontMovable() ________________________________________________________

	FlexPanel::CSlots::iterator FlexPanel::CSlots::pushFrontMovable( const Widget_p& pWidget, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot )
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		Slot * pSlot = _pushFrontEmpty();
		pSlot->_setWidget(pWidget);

		pSlot->m_placementGeo = geometry;
		pSlot->m_origo = origo;
		pSlot->m_hotspot = hotspot;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ pushBackPinned() _________________________________________________________

	FlexPanel::CSlots::iterator FlexPanel::CSlots::pushBackPinned(const Widget_p& pWidget, const FlexPos& topLeft, const FlexPos& bottomRight)
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		Slot * pSlot = _pushBackEmpty();
		pSlot->_setWidget(pWidget);

		pSlot->m_bPinned = true;
		pSlot->m_topLeftPin = topLeft;
		pSlot->m_bottomRightPin = bottomRight;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ pushBackMovable() ________________________________________________________

	FlexPanel::CSlots::iterator FlexPanel::CSlots::pushBackMovable(const Widget_p& pWidget, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot)
	{
		//TODO: Assert

		pWidget->releaseFromParent();
		Slot * pSlot = _pushBackEmpty();
		pSlot->_setWidget(pWidget);

		pSlot->m_placementGeo = geometry;
		pSlot->m_origo = origo;
		pSlot->m_hotspot = hotspot;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}


	//____ insertPinned() ________________________________________________________

	FlexPanel::CSlots::iterator FlexPanel::CSlots::insertPinned( int index, const Widget_p& pWidget, const FlexPos& topLeft, const FlexPos& bottomRight )
	{
		//TODO: Assert

		Slot * pSlot = _insertEmpty(index);
		_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);

		pSlot->m_bPinned = true;
		pSlot->m_topLeftPin = topLeft;
		pSlot->m_bottomRightPin = bottomRight;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ insertMovable() ________________________________________________________

	FlexPanel::CSlots::iterator FlexPanel::CSlots::insertMovable( int index, const Widget_p& pWidget, const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot )
	{
		//TODO: Assert

		Slot * pSlot = _insertEmpty(index);
		_releaseGuardPointer(pWidget, &pSlot);
		pSlot->_setWidget(pWidget);

		pSlot->m_placementGeo = geometry;
		pSlot->m_origo = origo;
		pSlot->m_hotspot = hotspot;

		_holder()->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	//____ constructor ____________________________________________________________

	FlexPanel::FlexPanel() : slots(this)
	{
		m_bSiblingsOverlap = true;
	}

	//____ Destructor _____________________________________________________________

	FlexPanel::~FlexPanel()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& FlexPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setConfineWidgets() ___________________________________________________

	void FlexPanel::setConfineWidgets( bool bConfineWidgets )
	{
		if( bConfineWidgets != m_bConfineWidgets )
		{
			m_bConfineWidgets = bConfineWidgets;

			Slot * p = slots._begin();

			while( p < slots._end() )
			{
				_refreshRealGeo( p );
				p++;
			}
		}
	}

	//____ _preferredSize() _____________________________________________________________

	SizeSPX FlexPanel::_preferredSize(int scale) const
	{
		SizeSPX minSize;

		Slot * p = slots._begin();
		while( p < slots._end() )
		{
			minSize = SizeSPX::max(minSize,_sizeNeededForGeo(p));
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

		Slot * pFrom = static_cast<Slot*>(_pFrom);
		Slot * pTo = static_cast<Slot*>(_pTo);

		if (pTo->m_bVisible)		// This is correct, we have already switched places...
		{
			if (pTo < pFrom)			// We were moved forward
			{
				// Request render on all areas covered by siblings we have skipped in front of.

				Slot * p = pTo+1;
				while (p <= pFrom)
				{
					RectSPX cover(pTo->m_realGeo, p->m_realGeo);

					if (p->m_bVisible && !cover.isEmpty())
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
					RectSPX cover(pTo->m_realGeo, p->m_realGeo);

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
		Slot * pSlot = static_cast<Slot*>(_pSlot);
		_unhideSlots(pSlot,nb);
	}

	//____ _willEraseSlots() _____________________________________________________________

	void FlexPanel::_willEraseSlots( StaticSlot * _pSlot, int nb )
	{
		Slot * pSlot = static_cast<Slot*>(_pSlot);
		_hideSlots(pSlot,nb);
	}

	//____ _hideSlots() _____________________________________________________________

	void FlexPanel::_hideSlots( StaticSlot * _pSlot, int nb )
	{
		Slot * pSlot = static_cast<Slot*>(_pSlot);

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
		Slot * pSlot = static_cast<Slot*>(_pSlot);

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

	void FlexPanel::_onRequestRender( const RectSPX& rect, const Slot * pSlot )
	{
		if( !pSlot->m_bVisible )
			return;

		// Clip our geometry and put it in a dirtyrect-list

		Patches patches;
		patches.add( RectSPX( rect, RectSPX(0,0,m_size)) );

		// Remove portions of patches that are covered by opaque upper siblings

		for(Slot * pCover = slots._begin() ; pCover < pSlot ; pCover++ )
		{
			if( pCover->m_bVisible && pCover->m_realGeo.intersectsWith( rect ) )
				OO(pCover->_widget())->_maskPatches( patches, pCover->m_realGeo, RectSPX(0,0,65536,65536 ), _getBlendMode() );
		}

		// Make request render calls

		for( const RectSPX * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}

	//____ _cloneContent() _______________________________________________________

	void FlexPanel::_cloneContent( const Widget * _pOrg )
	{
		Panel::_cloneContent( _pOrg );

		//TODO: Implement
	}

	//____ _resize() ____________________________________________________________

	void FlexPanel::_resize( const SizeSPX& size, int scale )
	{
		Panel::_resize(size,scale);

		Slot * p = slots._begin();
		while( p < slots._end() )
		{
			_refreshRealGeo(p);
			p++;
		}
	}

	//____ _childPos() ________________________________________________________

	CoordSPX FlexPanel::_childPos( const StaticSlot * pSlot ) const
	{
		return ((Slot*)pSlot)->m_realGeo.pos();
	}

	//____ _childRequestRender() _________________________________________________

	void FlexPanel::_childRequestRender( StaticSlot * _pSlot )
	{
		Slot * pSlot = static_cast<Slot*>(_pSlot);
		_onRequestRender( pSlot->m_realGeo, pSlot );
	}

	void FlexPanel::_childRequestRender( StaticSlot * _pSlot, const RectSPX& rect )
	{
		Slot * pSlot = static_cast<Slot*>(_pSlot);
		_onRequestRender( rect + pSlot->m_realGeo.pos(), pSlot );
	}

	//____ _childRequestResize() _________________________________________________

	void FlexPanel::_childRequestResize( StaticSlot * _pSlot )
	{
		Slot * pSlot = static_cast<Slot*>(_pSlot);
		_refreshRealGeo(pSlot, true);
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	FlexPanel::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return Slot::TYPEINFO;
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
		auto pSlot = static_cast<const Slot*>(_pSlot);

		if (pSlot > slots._begin())
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * FlexPanel::_nextChild( const StaticSlot * _pSlot ) const
	{
		auto pSlot = static_cast<const Slot*>(_pSlot);

		if (pSlot < slots._last())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void FlexPanel::_releaseChild(StaticSlot * pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots._erase(static_cast<Slot*>(pSlot));
	}

	//____ _replaceChild() _____________________________________________________

	void FlexPanel::_replaceChild(StaticSlot * _pSlot, Widget * pNewWidget)
	{
		auto pSlot = static_cast<Slot*>(_pSlot);
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
			Slot * pSlot = slots._first();
			package.pSlot = pSlot;
			package.geo = pSlot->m_realGeo;
		}
	}

	//____ _nextSlotWithGeo() ______________________________________________________

	void FlexPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		Slot * pSlot = (Slot*) package.pSlot;

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

	void FlexPanel::_refreshRealGeo( Slot * pSlot, bool bForceResize )
	{
		RectSPX	geo;

		if( pSlot->m_bPinned )
		{
			CoordSPX topLeft = pSlot->m_topLeftPin._pos( m_size );
			CoordSPX bottomRight = pSlot->m_bottomRightPin._pos( m_size );

			geo = RectSPX(topLeft,bottomRight);

			// Respect widgets limits, apply in such a way that rectangle centers in specified rectangle

			SizeSPX sz = geo.size();
			sz.limit( pSlot->_widget()->_minSize(m_scale), pSlot->_widget()->_maxSize(m_scale) );
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

			SizeSPX sz = pSlot->m_placementGeo.isEmpty() ? pSlot->_widget()->_preferredSize(m_scale) : align(ptsToSpx(pSlot->m_placementGeo.size(),m_scale));
			sz.limit( pSlot->_widget()->_minSize(m_scale), pSlot->_widget()->_maxSize(m_scale) );		// Respect widgets limits.

			// Calculate position

			CoordSPX pos = pSlot->m_origo._pos( m_size );		// Placement,
			pos -= pSlot->m_hotspot._pos(sz);				// hotspot
			pos += ptsToSpx(pSlot->m_placementGeo.pos(),m_scale);				// and Offset.

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

			geo = RectSPX( pos, sz );
		}

		geo = align(geo);

		// Request render and update positions.

		if (geo != pSlot->m_realGeo)
		{
			_onRequestRender(pSlot->m_realGeo, pSlot);
			pSlot->m_realGeo = geo;
			_onRequestRender(pSlot->m_realGeo, pSlot);
		}

		if (bForceResize || pSlot->_widget()->_size() != geo.size())
			pSlot->_setSize(geo);
	}

	//____ _sizeNeededForGeo() ________________________________________

	SizeSPX FlexPanel::_sizeNeededForGeo( Slot * pSlot ) const
	{
		SizeSPX sz;

		if( pSlot->m_bPinned )
		{
			sz = pSlot->_widget()->_preferredSize(m_scale);

			sz += SizeSPX( pSlot->m_topLeftPin.offset.x, pSlot->m_topLeftPin.offset.y );
			sz -= SizeSPX( pSlot->m_bottomRightPin.offset.x, pSlot->m_bottomRightPin.offset.y );

			sz.w /= (pSlot->m_bottomRightPin.origo.x - pSlot->m_topLeftPin.origo.x);
			sz.h /= (pSlot->m_bottomRightPin.origo.y - pSlot->m_topLeftPin.origo.y);
		}
		else
		{
			RectSPX geo = ptsToSpx(pSlot->m_placementGeo,m_scale);

			CoordSPX hotspot = pSlot->m_hotspot._pos(geo.size());
			CoordSPX offset = geo.pos() + ptsToSpx(pSlot->m_origo.offset,m_scale) - hotspot;

			spx leftOfOrigo = 0 - offset.x;
			spx rightOfOrigo = offset.x + geo.w;
			spx aboveOrigo = 0 - offset.y;
			spx belowOrigo = offset.y + geo.h;

			if( leftOfOrigo > 0 )
				sz.w = leftOfOrigo / pSlot->m_origo.origo.x;

			if( rightOfOrigo > 0 )
			{
				int w = rightOfOrigo / (1.f - pSlot->m_origo.origo.x);
				if( sz.w < w )
					sz.w = w;
			}

			if( aboveOrigo > 0 )
				sz.h = aboveOrigo / pSlot->m_origo.origo.y;

			if( belowOrigo > 0 )
			{
				int h = belowOrigo / (1.f - pSlot->m_origo.origo.y);
				if( sz.h < h )
					sz.h = h;
			}
		}

		return align(sz);
	}



} // namespace wg
