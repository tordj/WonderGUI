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

#ifndef	WG_PANEL_IMPL_DOT_H
#define	WG_PANEL_IMPL_DOT_H
#pragma once

#include <vector>
#include <wg_panel.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_gfxdevice.h>
#include <wg_slot.h>

namespace wg
{
	template < class SlotType>
	const TypeInfo Panel<SlotType>::TYPEINFO = { "Panel<SlotType>", &Container::TYPEINFO };

	//____ constructor _____________________________________________________________

	template < class SlotType>
	Panel<SlotType>::Panel() : m_maskOp(MaskOp::Recurse), slots(this)
	{
	}

	//____ typeInfo() _________________________________________________________

	template < class SlotType>
	const TypeInfo&  Panel<SlotType>::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setMaskOp() _____________________________________________________________

	template < class SlotType>
	void Panel<SlotType>::setMaskOp( MaskOp operation )
	{
		if( operation != m_maskOp )
		{
			m_maskOp = operation;
			_requestRender();
		}
	}

	//____ hideSlots() ___________________________________________________________

	template < class SlotType>
	void Panel<SlotType>::hideSlots(int index, int amount)
	{
		_hideSlots( &slots[index], amount);
	}

	template < class SlotType>
	void Panel<SlotType>::hideSlots(iterator beg, iterator end)
	{
		_hideSlots( beg, int(end - beg));
	}

	//____ unhideSlots() ___________________________________________________________

	template < class SlotType>
	void Panel<SlotType>::unhideSlots(int index, int amount)
	{
		_unhideSlots( &slots[index], amount);
	}

	template < class SlotType>
	void Panel<SlotType>::unhideSlots(iterator beg, iterator end)
	{
		_unhideSlots( beg, int(end - beg));
	}


	//____ _maskPatches() __________________________________________________________

	template < class SlotType>
	void Panel<SlotType>::_maskPatches( PatchesSPX& patches, const RectSPX& geo, const RectSPX& clip )
	{
		RectSPX coverage = m_skin.contentRect(geo, m_scale, m_state);
		
		patches.sub( RectSPX::overlap(coverage,clip) );

		if( coverage.contains(_contentRect(geo)) );
			return;										// No need to loop through children, skins coverage contains them all.


		switch( m_maskOp )
		{
			case MaskOp::Recurse:
			{
				RectSPX	myClip = RectSPX::overlap(geo, clip);				// Need to limit clip to our geo. Otherwise children outside might mask what they shouldn't (for panels where children can go outside parent).
				SlotWithGeo child;
				_firstSlotWithGeo( child );

				while(child.pSlot)
				{
					child.pSlot->_widget()->_maskPatches( patches, child.geo + geo.pos(), myClip );
					_nextSlotWithGeo( child );
				}
				break;
			}
			case MaskOp::Skip:
				break;
			case MaskOp::Mask:
				patches.sub( RectSPX::overlap(geo,clip) );
				break;
		}
	}

//____ _slotGeo() _______________________________________________________

	template < class SlotType>
	RectSPX Panel<SlotType>::_slotGeo(const StaticSlot * pSlot) const
	{
		return ((PanelSlot*)pSlot)->m_geo;
	}


	//____ _firstChild() __________________________________________________________

	template < class SlotType>
	Widget * Panel<SlotType>::_firstChild() const
	{
		if (slots.isEmpty())
			return nullptr;

		return slots.front()._widget();
	}

	//____ _lastChild() __________________________________________________________

	template < class SlotType>
	Widget * Panel<SlotType>::_lastChild() const
	{
		if (slots.isEmpty())
			return nullptr;

		return slots.back()._widget();
	}


	//____ _firstSlotWithGeo() _____________________________________________________

	template < class SlotType>
	void Panel<SlotType>::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( slots.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			auto * pSlot = slots.begin();

			while (!static_cast<PanelSlot*>(pSlot)->m_bVisible)
			{
				if (pSlot == &slots.back())
				{
					package.pSlot = nullptr;
					return;
				}
				pSlot++;
			}

			package.pSlot = pSlot;
			package.geo = pSlot->m_geo;
		}
	}

	//____ _nextSlotWithGeo() _____________________________________________________

	template < class SlotType>
	void Panel<SlotType>::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		auto pSlot = (SlotType*) package.pSlot;

		if( pSlot == &slots.back() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;

			while (!static_cast<PanelSlot*>(pSlot)->m_bVisible)
			{
				if (pSlot == &slots.back())
				{
					package.pSlot = nullptr;
					return;
				}
				pSlot++;
			}

			package.pSlot = pSlot;
			package.geo = pSlot->m_geo;
		}
	}

	//____ _prevChild() _______________________________________________________

	template < class SlotType>
	Widget * Panel<SlotType>::_prevChild(const StaticSlot * _pSlot) const
	{
		auto pSlot = static_cast<const SlotType*>(_pSlot);

		if (pSlot > slots.begin())
			return pSlot[-1]._widget();

		return nullptr;
	}

	//____ _nextChild() _______________________________________________________

	template < class SlotType>
	Widget * Panel<SlotType>::_nextChild(const StaticSlot * _pSlot) const
	{
		auto pSlot = static_cast<const SlotType*>(_pSlot);

		if (pSlot < &slots.back())
			return pSlot[1]._widget();

		return nullptr;
	}

	//____ _childRequestRender() ______________________________________________

	template < class SlotType>
	void Panel<SlotType>::_childRequestRender(StaticSlot * pSlot, const RectSPX& rect)
	{
		_requestRender(rect + ((PanelSlot*)pSlot)->m_geo.pos());
	}


} // namespace wg

#endif //WG_PANEL_IMPL_DOT_H
