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

#ifndef	WG_TWOSLOTPANEL_DOT_H
#define	WG_TWOSLOTPANEL_DOT_H
#pragma once



#include <wg_panel.h>
#include <wg_slot.h>
#include <wg_slotarray2.h>
#include <wg_sizebroker.h>


namespace wg
{

	//____ TwoSlotPanel ___________________________________________________________

	class TwoSlotPanel;
	typedef	StrongPtr<TwoSlotPanel>	TwoSlotPanel_p;
	typedef	WeakPtr<TwoSlotPanel>	TwoSlotPanel_wp;

	class TwoSlotPanel : public Panel
	{

	public:

		//____ Slot _________________________________________________________

		class Slot : public DynamicSlot
		{
			friend class TwoSlotPanel;
			template<class S, int X> friend class SlotArray2;

		public: 

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Geometry _________________________________________________

			void			setWeight(float weight);
			inline float	weight() const { return m_weight; }


			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:
			Slot() : DynamicSlot(nullptr) {}

			float	m_weight = 1.f;
			int		m_length = 0;
		};

		//.____ Creation __________________________________________

		static TwoSlotPanel_p	create() { return TwoSlotPanel_p(new TwoSlotPanel()); }

		//.____ Components _______________________________________

		SlotArray2<Slot, 2>		slots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		void			setAxis(Axis orientaiton);
		Axis			axis() const { return m_bHorizontal ? Axis::X : Axis::Y; }

		//.____ Behavior _______________________________________________________

		void			setSizeBroker(SizeBroker* pBroker);
		SizeBroker_p	sizeBroker() const { return m_pSizeBroker; }

		//.____ Internal _______________________________________________________

		SizeSPX			_defaultSize(int scale = -1) const override;


	protected:
		TwoSlotPanel();
		virtual ~TwoSlotPanel();

		void		_updateDefaultSize();
		SizeSPX		_calcDefaultSize(int scale) const;
		bool		_updateGeo();

		// Overloaded from Widget

		void		_resize(const SizeSPX& size, int scale = -1) override;

		void		_collectPatches(PatchesSPX& container, const RectSPX& geo, const RectSPX& clip) override;
		void		_maskPatches(PatchesSPX& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode) override;

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo(SlotWithGeo& package) const override;
		void		_nextSlotWithGeo(SlotWithGeo& package) const override;

		CoordSPX	_childPos( const StaticSlot * pSlot) const override;

		void		_childRequestRender(StaticSlot * pSlot) override;
		void		_childRequestRender(StaticSlot * pSlot, const RectSPX& rect) override;
		void		_childRequestResize(StaticSlot * pSlot) override;

		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override;

		//

		inline RectSPX	_slotOneRect(const RectSPX& contentRect) const
		{
			if (m_bHorizontal)
				return { contentRect.x, contentRect.y, slots[0].m_length, contentRect.h };
			else
				return { contentRect.x, contentRect.y, contentRect.w, slots[0].m_length };
		}

		inline RectSPX	_slotTwoRect(const RectSPX& contentRect) const
		{
			if (m_bHorizontal)
				return { contentRect.x + slots[0].m_length, contentRect.y, slots[1].m_length, contentRect.h };
			else
				return { contentRect.x, contentRect.y + slots[0].m_length, contentRect.w, slots[1].m_length };
		}


		bool			m_bHorizontal;
		SizeSPX			m_defaultSize;
		SizeBroker_p	m_pSizeBroker;
	};

}

#endif //WG_TWOSLOTPANEL_DOT_H
