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

#ifndef	WG_PANEL_DOT_H
#define	WG_PANEL_DOT_H
#pragma once

#include <wg_container.h>
#include <wg_slot.h>
#include <wg_dynamicslotvector.h>
#include <wg_util.h>

namespace wg
{

	/**
	 * @brief	Base class for layout widgets.
	 *
	 * Base class for layout widgets.
	 */

	//____ Panel ________________________________________________________________

	class PanelBase : public Container
	{
		friend class PanelSlot;
	protected:
		PanelBase() {}
		template<class BP> PanelBase(const BP& bp) : Container(bp) {}

		virtual void _hideSlots(StaticSlot *, int nb) = 0;
		virtual void _unhideSlots(StaticSlot *, int nb) = 0;

	};


	//____ PanelSlot ____________________________________________________________

	class PanelSlot : public DynamicSlot
	{
		template<typename T>
		friend class Panel;

	public:

		//.____ Blueprint _______________________________________________________

		struct Blueprint
		{
			bool	visible = true;
		};


		PanelSlot(SlotHolder* pHolder) : DynamicSlot(pHolder) {}

		//.____ Appearance ________________________________________________

		inline void		hide() { static_cast<PanelBase*>(_holder())->_hideSlots(this, 1); }
		inline void		unhide() { static_cast<PanelBase*>(_holder())->_unhideSlots(this, 1); }

		inline void		setVisible(bool bVisible)
						{
							if (bVisible)
								static_cast<PanelBase*>(_holder())->_unhideSlots(this, 1);
							else
								static_cast<PanelBase*>(_holder())->_hideSlots(this, 1);
						}
		
		inline bool		isVisible() const { return m_bVisible; }

		//.____ Geometry _________________________________________________
		
		inline Coord	pos() const { return Util::spxToPts(m_geo.pos(),_holder()->_scale()); }
		inline Size		size() const { return Util::spxToPts(m_geo.size(), _holder()->_scale()); }
		inline Rect		geo() const { return Util::spxToPts(m_geo, _holder()->_scale()); }

	protected:
				
		RectSPX		m_geo;
		bool		m_bVisible = true;
	};





	template<class SlotType>
	class Panel : public PanelBase
	{
	public:
		
		using		iterator = typename DynamicSlotVector<SlotType>::iterator;

		//.____ Components _______________________________________

		DynamicSlotVector<SlotType>		slots;
		
		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _____________________________________________________
		
		void			hideSlots(int index, int amount);
		void			hideSlots(iterator beg, iterator end);

		void			unhideSlots(int index, int amount);
		void			unhideSlots(iterator beg, iterator end);

		
		//.____ Behavior _______________________________________________________

		void		setMaskOp( MaskOp operation );
		MaskOp		maskOp() const { return m_maskOp; }

		

	protected:
		Panel();
		template<class BP> Panel(const BP& bp) : PanelBase(bp), slots(this)
		{
			m_maskOp = bp.maskOp;
		}
		
		virtual ~Panel() {};

		virtual void	_maskPatches( PatchesSPX& patches, const RectSPX& geo, const RectSPX& clip ) override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		RectSPX		_slotGeo(const StaticSlot * pSlot) const override;

		void		_childRequestRender(StaticSlot * pSlot, const RectSPX& rect) override;

		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		
		MaskOp		m_maskOp;			// Specifies how container masks background.

	};




} // namespace wg
#endif //WG_PANEL_DOT_H
