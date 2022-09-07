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
#ifndef WG_PACKPANEL_DOT_H
#define WG_PACKPANEL_DOT_H
#pragma once

#include <wg_sizebroker.h>
#include <wg_panel.h>
#include <wg_slot.h>
#include <wg_slotextras.h>
#include <wg_dynamicslotvector.h>


namespace wg
{

	class PackPanel;
	typedef	StrongPtr<PackPanel>		PackPanel_p;
	typedef	WeakPtr<PackPanel>			PackPanel_wp;

	//____ PackPanel ________________________________________________________________
	/**
	 * @brief	A widget for arranging children horizontally or vertically.
	 *
	 * A widget for arranging children horizontally or vertically.
	 */

	class PackPanel : public Panel
	{

	public:

		//____ Slot ____________________________________________________________

		class Slot : public DynamicSlot
		{
			friend class PackPanel;
			friend class CSlots;
			template<class S> friend class SlotVector;
			friend class DynamicSlotVector<Slot>;

		public:
			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Geometry _________________________________________________

			void			setWeight(float weight);
			inline float	weight() const { return m_weight; }

			inline Coord	pos() const { return Util::spxToPts(m_geo.pos(),_holder()->_scale()); }
			inline Size		size() const { return Util::spxToPts(m_geo.size(), _holder()->_scale()); }
			inline Rect		geo() const { return Util::spxToPts(m_geo, _holder()->_scale()); }

			SLOT_PADDING_METHODS(PackPanel)

			//.____ Appearance ________________________________________________

			SLOT_HIDING_METHODS(PackPanel)

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:

			Slot(SlotHolder *pHolder) : DynamicSlot(pHolder) {}
			Slot(Slot&& o) = default;
			Slot& operator=(Slot&& o) = default;

			SLOT_PADDING_HELPER_METHODS

			Border		m_padding;
			bool		m_bVisible = true;

			bool		m_bResizeRequired = false;
			float		m_weight = 1.f;				// Weight for space allocation.
			RectSPX		m_geo;						// Real geo of child (no padding included).
			SizeSPX		m_defaultSize;			// Cached padded default size from the child.
		};

		using		iterator = SlotArrayIterator<Slot>;

		//____ CSlots ________________________________________________________


		class CSlots : public DynamicSlotVector<Slot>, 
			public PaddedSlotCollectionMethods<Slot, iterator, PackPanel>,
			public HideableSlotCollectionMethods<Slot, iterator, PackPanel>
		{
			friend class PackPanel;
		public:

			//.____ Geometry _______________________________________________________

			bool		setWeight(int index, int amount, float weight);
			bool		setWeight(const SlotIterator&  beg, const SlotIterator&  end, float weight);
			bool		setWeight(int index, int amount, std::initializer_list<float> weights);
			bool		setWeight(const SlotIterator&  beg, const SlotIterator&  end, std::initializer_list<float> weights);

		protected:
			CSlots(SlotHolder * pHolder) : DynamicSlotVector<Slot>(pHolder) {}

			inline PackPanel *		_holder() { return static_cast<PackPanel*>(DynamicSlotVector<Slot>::_holder()); }
			inline const PackPanel *	_holder() const { return static_cast<const PackPanel*>(DynamicSlotVector<Slot>::_holder()); }

			Slot* _slot(int index) { return DynamicSlotVector<Slot>::_slot(index); }

			inline int _size() const override { return DynamicSlotVector<Slot>::size(); }
		};

		friend class PaddedSlotCollectionMethods<PackPanel::Slot, iterator, PackPanel>;
		friend class HideableSlotCollectionMethods<PackPanel::Slot, iterator, PackPanel>;

		//.____ Creation __________________________________________

		static PackPanel_p	create() { return PackPanel_p(new PackPanel()); }

		//.____ Components _______________________________________

		CSlots		slots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		void			setAxis( Axis orientaiton );
		Axis			axis() const { return m_bHorizontal?Axis::X:Axis::Y; }

		//.____ Behavior ________________________________________________________

		void			setSizeBroker( SizeBroker * pBroker );
		SizeBroker_p	sizeBroker() const { return m_pSizeBroker; }

		//.____ Internal ______________________________________________________

		spx				_matchingHeight(spx width, int scale = -1) const override;
		spx				_matchingWidth(spx height, int scale = -1) const override;

		SizeSPX			_defaultSize(int scale = -1) const override;


	protected:
		PackPanel();
		virtual ~PackPanel();

		// Overloaded from Widget

		void		_resize( const SizeSPX& size, int scale = -1 ) override;


		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		CoordSPX	_childPos(const StaticSlot * pSlot) const override;

		void		_childRequestRender(StaticSlot * pSlot) override;
		void		_childRequestRender(StaticSlot * pSlot, const RectSPX& rect) override;
		void		_childRequestResize(StaticSlot * pSlot) override;

		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewWidget) override;

		void		_didAddSlots(StaticSlot * pSlot, int nb) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots(StaticSlot * pSlot, int nb) override;

		void		_hideSlots(StaticSlot *, int nb);
		void		_unhideSlots(StaticSlot *, int nb);
		void		_repadSlots(StaticSlot *, int nb, Border padding);
		void		_repadSlots(StaticSlot *, int nb, const Border * pPaddings);

		// Needed by CSlots

		void		_reweightSlots(Slot * pSlot, int nb, float weight);
		void		_reweightSlots(Slot * pSlot, int nb, const float * pWeights);
		void		_refreshChildGeo() { _refreshChildGeo(true); }

		//

		void		_refreshChildGeo(bool bRequestRender);

		void		_hideChildren(Slot * pSlot, int nb);
		void		_unhideChildren(Slot * pSlot, int nb);

		void		_refreshGeometries();
		SizeSPX		_calcDefaultSize( int scale ) const;
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray ) const;
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray, spx forcedBreadth ) const;

		spx			_setItemLengths(SizeBrokerItem * pItems, int nItems, spx totalLength) const;
		spx			_setDefaultLengths(SizeBrokerItem * pItems, int nItems) const;

		bool			m_bHorizontal;
		SizeBroker_p	m_pSizeBroker;
		SizeSPX			m_defaultContentSize;

	};



} // namespace wg
#endif //WG_PACKPANEL_DOT_H
