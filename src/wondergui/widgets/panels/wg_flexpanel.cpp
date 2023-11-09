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

#include <wg_dynamicslotvector.impl.h>
#include <wg_panel.impl.h>

#include <assert.h>

namespace wg
{
	using namespace Util;

	template class DynamicSlotVector<FlexPanelSlot>;
	template class Panel<FlexPanelSlot>;

	const TypeInfo FlexPanel::TYPEINFO = { "FlexPanel", &Panel::TYPEINFO };
	const TypeInfo FlexPanelSlot::TYPEINFO = { "FlexPanelSlot", &DynamicSlot::TYPEINFO };

	float	FlexPos::s_origoTab[Placement_size][2] = {	-1.f, -1.f,			// Signals undefined.
														0.f, 0.f,
														0.5f, 0.f,
														1.f, 0.f,
														1.f, 0.5f,
														1.f, 1.f,
														0.5f, 1.f,
														0.f, 1.f,
														0.f, 0.5f,
														0.5f, 0.5f };



	//____ FlexPanelSlot::_setBlueprint() ________________________________________

	bool FlexPanelSlot::_setBlueprint( const Blueprint& bp )
	{
		if(bp.pin1.origo.x >= 0.f && bp.pin2.origo.x >= 0.f )
		{
			m_bPinned = true;
			m_topLeftPin = bp.pin1;
			m_bottomRightPin = bp.pin2;
		}
		else
		{
			m_placementGeo = Rect( bp.pos, bp.size );
			m_origo = bp.origo;
			m_hotspot = bp.hotspot.origo.x < 0 ? bp.origo : bp.hotspot;

		}
		
		m_bVisible = bp.visible;
		return true;
	}


	//____ FlexPanelSlot::setPinned() _______________________________________________

	void FlexPanelSlot::setPinned()
	{
		if (!m_bPinned)
		{
			m_bPinned = true;

			SizeSPX sz = static_cast<FlexPanel*>(_holder())->_size();

			m_topLeftPin = FlexPos(m_geo.x / (float)sz.w, m_geo.y / (float)sz.h);
			m_bottomRightPin = FlexPos(m_geo.right() / (float)sz.w, m_geo.bottom() / (float)sz.h);

			static_cast<FlexPanel*>(_holder())->_refreshRealGeo(this);
		}
	}

	void FlexPanelSlot::setPinned(const FlexPos& topLeft, const FlexPos& bottomRight)
	{
		m_bPinned = true;
		m_topLeftPin = topLeft;
		m_bottomRightPin = bottomRight;

		static_cast<FlexPanel*>(_holder())->_refreshRealGeo(this);
	}

	//____ FlexPanelSlot::setMovable() _______________________________________________

	void FlexPanelSlot::setMovable(const FlexPos& origo, const FlexPos& hotspot)
	{
		if (m_bPinned)
		{
			int scale = _holder()->_scale();
			Rect geo = spxToPts(m_geo, scale);


			m_bPinned = false;
			m_origo = origo;
			m_hotspot = hotspot.origo.x < 0 ? origo : hotspot;
			m_placementGeo = geo - origo.pos(static_cast<FlexPanel*>(_holder())->size()) + hotspot.pos(geo);

			static_cast<FlexPanel*>(_holder())->_refreshRealGeo(this);
		}
	}

	void FlexPanelSlot::setMovable(const Rect& geometry, const FlexPos& origo, const FlexPos& hotspot)
	{
		m_bPinned = false;
		m_origo = origo;
		m_hotspot = hotspot.origo.x < 0 ? origo : hotspot;
		m_placementGeo = geometry;

		static_cast<FlexPanel*>(_holder())->_refreshRealGeo(this);
	}

	//____ FlexPanelSlot::setOrigo() __________________________________________________

	bool FlexPanelSlot::setOrigo(const FlexPos& origo)
	{
		if (m_bPinned)
			return false;

		m_origo = origo;
		static_cast<FlexPanel*>(_holder())->_refreshRealGeo(this);
		return true;
	}

	//____ FlexPanelSlot::setHotspot() _________________________________________________

	bool FlexPanelSlot::setHotspot(const FlexPos& hotspot)
	{
		if (m_bPinned)
			return false;

		m_hotspot = hotspot;
		static_cast<FlexPanel*>(_holder())->_refreshRealGeo(this);
		return true;
	}

	//____ FlexPanelSlot::setGeo() ____________________________________________________

	bool FlexPanelSlot::setGeo(const Rect& geometry)
	{
		if (m_bPinned)
			return false;

		m_placementGeo = geometry;
		static_cast<FlexPanel*>(_holder())->_refreshRealGeo(this);
		return true;
	}

	//____ FlexPanelSlot::setOffset() __________________________________________________

	bool FlexPanelSlot::setOffset(const Coord& ofs)
	{
		if (m_bPinned)
			return false;

		m_placementGeo.setPos(ofs);
		static_cast<FlexPanel*>(_holder())->_refreshRealGeo(this);
		return true;
	}

	//____ FlexPanelSlot::setSize() _____________________________________________________

	bool FlexPanelSlot::setSize(const Size& size)
	{
		if (m_bPinned)
			return false;

		m_placementGeo.setSize(size);
		static_cast<FlexPanel*>(_holder())->_refreshRealGeo(this);
		return true;
	}

	//____ FlexPanelSlot::move() _______________________________________________________

	bool FlexPanelSlot::move(const Coord& ofs)
	{
		if (m_bPinned)
			return false;

		m_placementGeo += ofs;
		static_cast<FlexPanel*>(_holder())->_refreshRealGeo(this);
		return true;
	}

	//____ constructor ____________________________________________________________

	FlexPanel::FlexPanel()
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

			auto p = slots.begin();

			while( p < slots.end() )
			{
				_refreshRealGeo( p );
				p++;
			}
		}
	}

	//____ _defaultSize() _____________________________________________________________

	SizeSPX FlexPanel::_defaultSize(int scale) const
	{
		SizeSPX minSize;

		auto p = slots.begin();
		while( p < slots.end() )
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

		auto pFrom = static_cast<FlexPanelSlot*>(_pFrom);
		auto pTo = static_cast<FlexPanelSlot*>(_pTo);

		if (pTo->m_bVisible)		// This is correct, we have already switched places...
		{
			if (pTo < pFrom)			// We were moved forward
			{
				// Request render on all areas covered by siblings we have skipped in front of.

				auto p = pTo+1;
				while (p <= pFrom)
				{
					RectSPX cover = RectSPX::overlap(pTo->m_geo, p->m_geo);

					if (p->m_bVisible && !cover.isEmpty())
						_onRequestRender(cover, pTo);
					p++;
				}
			}
			else							// Move us backward
			{
				// Request render on our siblings for the area we previously have covered.

				auto p = pFrom;
				while (p < pTo)
				{
					RectSPX cover = RectSPX::overlap(pTo->m_geo, p->m_geo);

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
		auto pSlot = static_cast<FlexPanelSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].m_bVisible == true )
			{
				_refreshRealGeo(&pSlot[i]);
				_onRequestRender(pSlot[i].m_geo, &pSlot[i]);
			}
		}
	}

	//____ _willEraseSlots() _____________________________________________________________

	void FlexPanel::_willEraseSlots( StaticSlot * _pSlot, int nb )
	{
		auto pSlot = static_cast<FlexPanelSlot*>(_pSlot);
		_hideSlots(pSlot,nb);
	}

	//____ _hideSlots() _____________________________________________________________

	void FlexPanel::_hideSlots( StaticSlot * _pSlot, int nb )
	{
		auto pSlot = static_cast<FlexPanelSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].m_bVisible == true )
			{
				_onRequestRender(pSlot[i].m_geo, &pSlot[i]);
				pSlot[i].m_bVisible = false;					// Needs to be done AFTER _onRequestRender()!
			}
		}
	}

	//____ _unhideSlots() _____________________________________________________________

	void FlexPanel::_unhideSlots( StaticSlot * _pSlot, int nb )
	{
		auto pSlot = static_cast<FlexPanelSlot*>(_pSlot);

		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].m_bVisible == false )
			{
				pSlot[i].m_bVisible = true;
				_refreshRealGeo(&pSlot[i]);
				_onRequestRender(pSlot[i].m_geo, &pSlot[i]);
			}
		}
	}


	//____ _onRequestRender() ______________________________________________________

	void FlexPanel::_onRequestRender( const RectSPX& rect, const FlexPanelSlot * pSlot )
	{
		if( !pSlot->m_bVisible )
			return;

		// Clip our geometry and put it in a dirtyrect-list

		PatchesSPX patches;
		patches.add( RectSPX::overlap( rect, RectSPX(0,0,m_size)) );

		// Remove portions of patches that are covered by opaque upper siblings

		for(auto pCover = slots.begin() ; pCover < pSlot ; pCover++ )
		{
			if( pCover->m_bVisible && pCover->m_geo.isOverlapping( rect ) )
				pCover->_widget()->_maskPatches( patches, pCover->m_geo, RectSPX(0,0,65536,65536 ), _getBlendMode() );
		}

		// Make request render calls

		for( const RectSPX * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}

	//____ _resize() ____________________________________________________________

	void FlexPanel::_resize( const SizeSPX& size, int scale )
	{
		Panel::_resize(size,scale);

		auto p = slots.begin();
		while( p < slots.end() )
		{
			_refreshRealGeo(p);
			p++;
		}
	}

	//____ _childRequestRender() _________________________________________________

	void FlexPanel::_childRequestRender( StaticSlot * _pSlot )
	{
		auto pSlot = static_cast<FlexPanelSlot*>(_pSlot);
		_onRequestRender( pSlot->m_geo, pSlot );
	}

	void FlexPanel::_childRequestRender( StaticSlot * _pSlot, const RectSPX& rect )
	{
		auto pSlot = static_cast<FlexPanelSlot*>(_pSlot);
		_onRequestRender( rect + pSlot->m_geo.pos(), pSlot );
	}

	//____ _childRequestResize() _________________________________________________

	void FlexPanel::_childRequestResize( StaticSlot * _pSlot )
	{
		auto pSlot = static_cast<FlexPanelSlot*>(_pSlot);
		_refreshRealGeo(pSlot, true);
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	FlexPanel::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return FlexPanelSlot::TYPEINFO;
	}

	//____ _releaseChild() ____________________________________________________

	void FlexPanel::_releaseChild(StaticSlot * pSlot)
	{
		slots.erase(static_cast<FlexPanelSlot*>(pSlot));
	}

	//____ _replaceChild() _____________________________________________________

	void FlexPanel::_replaceChild(StaticSlot * _pSlot, Widget * pNewWidget)
	{
		auto pSlot = static_cast<FlexPanelSlot*>(_pSlot);
		slots._releaseGuardPointer(pNewWidget, &pSlot);

		pSlot->_setWidget(pNewWidget);

		if (pSlot->m_bVisible )
		{
			_refreshRealGeo(pSlot, true);
			_onRequestRender(pSlot->m_geo, pSlot);
		}
	}

	//____ _refreshRealGeo() ___________________________________________

	void FlexPanel::_refreshRealGeo( FlexPanelSlot * pSlot, bool bForceResize )
	{
		RectSPX	geo;

		if( pSlot->m_bPinned )
		{
			CoordSPX topLeft = pSlot->m_topLeftPin._pos( m_size, m_scale );
			CoordSPX bottomRight = pSlot->m_bottomRightPin._pos( m_size, m_scale );

			geo = RectSPX(topLeft,bottomRight);

			// Respect widgets limits, apply in such a way that rectangle centers in specified rectangle

			SizeSPX sz = geo.size();
			sz.limit( pSlot->_widget()->_minSize(m_scale), pSlot->_widget()->_maxSize(m_scale) );
			if( sz != geo.size() )
			{
				geo.x += (geo.w - sz.w) / 2;
				geo.y += (geo.h - sz.h) / 2;
				geo.w = sz.w;
				geo.h = sz.h;
			}
		}
		else
		{
			// Calculate size

			SizeSPX sz = pSlot->m_placementGeo.isEmpty() ? pSlot->_widget()->_defaultSize(m_scale) : align(ptsToSpx(pSlot->m_placementGeo.size(),m_scale));
			sz.limit( pSlot->_widget()->_minSize(m_scale), pSlot->_widget()->_maxSize(m_scale) );		// Respect widgets limits.

			// Calculate position

			CoordSPX pos = pSlot->m_origo._pos( m_size, m_scale );		// Placement,
			pos -= pSlot->m_hotspot._pos(sz, m_scale);				// hotspot
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

		if (geo != pSlot->m_geo)
		{
			_onRequestRender(pSlot->m_geo, pSlot);
			pSlot->m_geo = geo;
			_onRequestRender(pSlot->m_geo, pSlot);
		}

		if (bForceResize || pSlot->_widget()->_size() != geo.size())
			pSlot->_setSize(geo, m_scale);
	}

	//____ _sizeNeededForGeo() ________________________________________

	SizeSPX FlexPanel::_sizeNeededForGeo( FlexPanelSlot * pSlot ) const
	{
		SizeSPX sz;

		if( pSlot->m_bPinned )
		{
			sz = pSlot->_widget()->_defaultSize(m_scale);

			sz += SizeSPX( pSlot->m_topLeftPin.offset.x, pSlot->m_topLeftPin.offset.y );
			sz -= SizeSPX( pSlot->m_bottomRightPin.offset.x, pSlot->m_bottomRightPin.offset.y );

			sz.w /= (pSlot->m_bottomRightPin.origo.x - pSlot->m_topLeftPin.origo.x);
			sz.h /= (pSlot->m_bottomRightPin.origo.y - pSlot->m_topLeftPin.origo.y);
		}
		else
		{
			RectSPX geo = ptsToSpx(pSlot->m_placementGeo,m_scale);

			CoordSPX hotspot = pSlot->m_hotspot._pos(geo.size(), m_scale);
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
