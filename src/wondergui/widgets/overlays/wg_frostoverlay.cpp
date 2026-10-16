/*=========================================================================

                             >>> WonderGUI <<<

  This file is part of Tord Bärnfors' WonderGUI UI Toolkit and copyright
  Tord Bärnfors, Sweden [mail: first name AT barnfors DOT c_o_m].

                                -----------

  The WonderGUI UI Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                                -----------

  The WonderGUI UI Toolkit is also available for use in commercial
  closed source projects under a separate license. Interested parties
  should contact Bärnfors Technology AB [www.barnfors.com] for details.

=========================================================================*/
#include <wg_frostoverlay.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_gfxdevice.h>
#include <wg_base.h>

#include <wg_dynamicslotvector.impl.h>


namespace wg
{
	using namespace Util;

	template class DynamicSlotVector<FrostOverlay::Slot>;

	const TypeInfo FrostOverlay::TYPEINFO = { "FrostOverlay", &Overlay::TYPEINFO };
	const TypeInfo FrostOverlay::Slot::TYPEINFO = { "FrostOverlay::Slot", &Overlay::Slot::TYPEINFO };

	//____ Slot::_setBlueprint() _________________________________________________

	bool FrostOverlay::Slot::_setBlueprint( const Blueprint& bp )
	{
		m_bFrost = bp.frost;
		m_placementGeo = Rect( bp.pos, bp.size );
		return true;
	}

	//____ Slot::setGeo() ____________________________________________________

	void FrostOverlay::Slot::setGeo(const Rect& geometry)
	{
		m_placementGeo = geometry;
		_holder()->_refreshRealGeo(this);
	}

	//____ Slot::setPos() ____________________________________________________

	void FrostOverlay::Slot::setPos(const Coord& pos)
	{
		m_placementGeo.setPos(pos);
		_holder()->_refreshRealGeo(this);
	}

	//____ Slot::setSize() ____________________________________________________

	void FrostOverlay::Slot::setSize(const Size& size)
	{
		m_placementGeo.setSize(size);
		_holder()->_refreshRealGeo(this);
	}

	//____ Slot::move() ____________________________________________________

	void FrostOverlay::Slot::move(const Coord& ofs)
	{
		m_placementGeo += ofs;
		_holder()->_refreshRealGeo(this);
	}

	//____ Slot::setFrost() ____________________________________________________

	void FrostOverlay::Slot::setFrost(bool bFrost)
	{
		if (bFrost == m_bFrost)
			return;

		m_bFrost = bFrost;

		if (!bFrost)
			m_pane.clear();

		_holder()->_onRequestRender(m_geo, this);
	}

	//____ constructor ____________________________________________________________

	FrostOverlay::FrostOverlay() : frostSlots(this), frost(this)
	{
	}

	//____ Destructor _____________________________________________________________

	FrostOverlay::~FrostOverlay()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& FrostOverlay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _refreshRealGeo() __________________________________________________

	void FrostOverlay::_refreshRealGeo( Slot * pSlot, bool bForceResize )
	{
		RectSPX placementGeo = align(ptsToSpx(pSlot->m_placementGeo,m_scale));

		SizeSPX sz = placementGeo.size();

		if( sz.w == 0 && sz.h == 0 )
			sz = pSlot->_widget()->_defaultSize(m_scale);
		else if( sz.w == 0 )
			sz.w = pSlot->_widget()->_matchingWidth(sz.h,m_scale);
		else if( sz.h == 0 )
			sz.h = pSlot->_widget()->_matchingHeight(sz.w,m_scale);

		if( sz.w <= 0 )
			sz.w = 64;
		if( sz.h <= 0 )
			sz.h = 64;

		RectSPX geo( placementGeo.pos(), sz );

		// The frost of the pane is recalculated when it is rendered with its new geometry.

		if (geo != pSlot->m_geo)
		{
			_onRequestRender(pSlot->m_geo + pSlot->_widget()->_overflow(), pSlot);
			pSlot->m_geo = geo;
			_onRequestRender(pSlot->m_geo + pSlot->_widget()->_overflow(), pSlot);
		}

		if (bForceResize || pSlot->_widget()->_size() != geo.size())
			pSlot->_setSize(geo, m_scale);
	}

	//____ _resize() ___________________________________________________________

	void FrostOverlay::_resize( const SizeSPX& sz, int scale )
	{
		Overlay::_resize(sz,scale);

		// The backdrop is recreated, and all frost with it, when next rendered.

		frost._setBackdropSize(sz, scale);

		for( auto pSlot = frostSlots.begin() ; pSlot != frostSlots.end() ; pSlot++ )
			_refreshRealGeo( pSlot );
	}

	//____ _render() ___________________________________________________________

	void FrostOverlay::_render(GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window)
	{
		// Render our own skin, it is not part of the backdrop.

		m_skin.render(pDevice, _canvas, m_scale, m_state);

		// Bring the cached backdrop up to date. This renders everything that the main child
		// has requested, whether it is visible on our canvas or not, since panes
		// might need it for their frost. Then copy it to our canvas.

		frost._updateBackdrop(pDevice, [this](GfxDevice * pBackdropDevice, const RectSPX& backdrop)
		{
			if (mainSlot._widget())
				mainSlot._widget()->_render(pBackdropDevice, backdrop, backdrop);
		});

		frost._renderBackdrop(pDevice, _canvas);

		// Render panes that might be dirty, bottom one first.

		RectSPX	dirtBounds = pDevice->clipBounds();

		for( auto pSlot = frostSlots.end() ; pSlot != frostSlots.begin() ; )
		{
			pSlot--;

			if( !pSlot->m_bVisible )
				continue;

			RectSPX geo = pSlot->m_geo + _canvas.pos();
			RectSPX renderBounds = pSlot->_widget()->_renderBounds() + geo.pos();

			if( !renderBounds.isOverlapping(dirtBounds) )
				continue;

			ClipPopData clipPop = limitClipList(pDevice, renderBounds);

			if( pDevice->clipListSize() > 0 )
			{
				if( pSlot->m_bFrost )
					frost._renderPane(pDevice, pSlot->m_pane, pSlot->m_geo, geo, pSlot->_widget()->state());

				pSlot->_widget()->_render(pDevice, geo, geo);
			}

			popClipList(pDevice, clipPop);
		}
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	FrostOverlay::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		if (pSlot == &mainSlot)
			return DynamicSlot::TYPEINFO;

		return Slot::TYPEINFO;
	}

	//____ _childRequestRender() _______________________________________________

	void FrostOverlay::_childRequestRender( StaticSlot * _pSlot, const RectSPX& rect )
	{
		if( _pSlot != &mainSlot )
		{
			// Content of a pane has changed, its frost is still valid.

			Overlay::_childRequestRender(_pSlot, rect);
			return;
		}

		// The main child takes up all of us, so its coordinates are ours.

		frost._invalidateBackdrop(rect);

		// Frost of panes behind which the backdrop has changed needs to be recalculated,
		// and the part of the pane where it changes rendered again.

		for( auto pSlot = frostSlots.begin() ; pSlot != frostSlots.end() ; pSlot++ )
		{
			if( !pSlot->m_bVisible || !pSlot->m_bFrost )
				continue;

			RectSPX damage = frost._paneDamage(rect, pSlot->m_geo);
			if( !damage.isEmpty() )
			{
				pSlot->m_pane.invalidate();
				_onRequestRender(damage, pSlot);
			}
		}

		_onRequestRender(rect, nullptr);
	}

	//____ _childRequestResize() ______________________________________________

	void FrostOverlay::_childRequestResize( StaticSlot * pSlot )
	{
		if( pSlot == &mainSlot )
			_requestResize();
		else
			_refreshRealGeo( static_cast<Slot*>(pSlot), true );
	}

	//____ _releaseChild() ____________________________________________________

	void FrostOverlay::_releaseChild(StaticSlot * pSlot)
	{
		if (pSlot == &mainSlot)
			Overlay::_releaseChild(pSlot);
		else
		{
			_willEraseSlots(pSlot, 1);
			frostSlots.erase(static_cast<Slot*>(pSlot));
		}
	}

	//____ _replaceChild() ____________________________________________________

	void FrostOverlay::_replaceChild(StaticSlot * _pSlot, Widget * pNewChild)
	{
		if (_pSlot == &mainSlot)
		{
			Overlay::_replaceChild(_pSlot, pNewChild);
			return;
		}

		if( pNewChild == nullptr )
		{
			Base::throwError(ErrorLevel::Error, ErrorCode::InvalidParam, "Slot in FrostOverlay must contain pointer to widget and not nullptr.", this, &TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		// The new child might be released from one of our other slots, which moves this one.

		int index = frostSlots._releaseUpdateIndex(pNewChild, int(static_cast<Slot*>(_pSlot) - frostSlots.begin()));
		Slot * pSlot = &frostSlots.at(index);

		_onRequestRender(pSlot->m_geo + pSlot->_widget()->_overflow(), pSlot);
		pSlot->_setWidget(pNewChild);
		_refreshRealGeo(pSlot, true);
		_onRequestRender(pSlot->m_geo + pSlot->_widget()->_overflow(), pSlot);
	}

	//____ _didAddSlots() __________________________________________________

	void FrostOverlay::_didAddSlots(StaticSlot * _pSlot, int nb)
	{
		Slot * pSlot = static_cast<Slot*>(_pSlot);
		for( int i = 0 ; i < nb ; i++ )
			_refreshRealGeo(&pSlot[i], true);
	}

	//____ _didMoveSlots() ___________________________________________________________

	void FrostOverlay::_didMoveSlots(StaticSlot * _pFrom, StaticSlot * _pTo, int nb)
	{
		// Frost of a pane only depends on the backdrop, so moving panes up and down
		// only affects what needs to be rendered.

		if (nb > 1)
		{
			_requestRender();
			return;
		}

		auto pFrom = static_cast<Slot*>(_pFrom);
		auto pTo = static_cast<Slot*>(_pTo);

		// Now we have switched places, pTo contains the widget that was moved

		if (pTo < pFrom)			// We were moved up
		{
			// Request render on all areas covered by siblings we have moved in front of.

			for( Slot * p = pTo+1 ; p <= pFrom ; p++ )
			{
				RectSPX cover = RectSPX::overlap(pTo->_widget()->_renderBounds() + pTo->m_geo.pos(), p->_widget()->_renderBounds() + p->m_geo.pos());

				if (!cover.isEmpty())
					_onRequestRender(cover, pTo);
			}
		}
		else						// We were moved down
		{
			// Request render on our siblings for the area we previously have covered.

			for( Slot * p = pFrom ; p < pTo ; p++ )
			{
				RectSPX cover = RectSPX::overlap(pTo->_widget()->_renderBounds() + pTo->m_geo.pos(), p->_widget()->_renderBounds() + p->m_geo.pos());

				if (!cover.isEmpty())
					_onRequestRender(cover, p);
			}
		}
	}

	//____ _willEraseSlots() __________________________________________________

	void FrostOverlay::_willEraseSlots(StaticSlot * _pSlot, int nb)
	{
		Slot * pSlot = static_cast<Slot*>(_pSlot);
		for( int i = 0 ; i < nb ; i++ )
		{
			_childRequestRender(pSlot+i, pSlot[i]._widget()->_renderBounds() );
			pSlot[i].m_geo.clear();
			pSlot[i].m_pane.clear();
		}
	}

	//____ _beginOverlaySlots() __________________________________________________

	const Overlay::Slot * FrostOverlay::_beginOverlaySlots() const
	{
		return frostSlots.begin();
	}

	//____ _endOverlaySlots() ____________________________________________________

	const Overlay::Slot * FrostOverlay::_endOverlaySlots() const
	{
		return frostSlots.end();
	}

	//____ _sizeOfOverlaySlot() __________________________________________________

	int FrostOverlay::_sizeOfOverlaySlot() const
	{
		return sizeof(Slot);
	}

} // namespace wg
