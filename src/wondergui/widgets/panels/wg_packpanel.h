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

#include <wg_packlayout.h>
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
			template<class S> friend class SlotVector;
			friend class DynamicSlotVector<Slot>;

		public:

			//.____ Blueprint _______________________________________________________

			struct Blueprint
			{
				Border	padding = 0;
				bool	visible = true;
				float	weight = 1;
			};
			

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

			bool _setBlueprint( const Blueprint& bp );

			SLOT_PADDING_HELPER_METHODS

			Border		m_padding;
			bool		m_bVisible = true;

			bool		m_bResizeRequired = false;
			float		m_weight = 1.f;				// Weight for space allocation.
			RectSPX		m_geo;						// Real geo of child (no padding included).
			SizeSPX		m_defaultSize;				// Cached padded default size from the child.
		};

		using		iterator = DynamicSlotVector<Slot>::iterator;

		//.____ Blueprint _____________________________________________________
		
		struct Blueprint
		{
			Axis			axis = Axis::X;
			Object_p		baggage;
			bool			dropTarget = false;
			bool			enabled = true;
			Finalizer_p		finalizer = nullptr;
			int				id = 0;
			PackLayout_p		layout;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
		};
		
		//.____ Creation __________________________________________

		static PackPanel_p	create() { return PackPanel_p(new PackPanel()); }
		static PackPanel_p	create(const Blueprint& blueprint) { return PackPanel_p(new PackPanel(blueprint)); }

		//.____ Components _______________________________________

		DynamicSlotVector<Slot>	slots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		void			setAxis( Axis orientaiton );
		Axis			axis() const { return m_bHorizontal?Axis::X:Axis::Y; }

		//.____ Behavior ________________________________________________________

		void			setLayout(PackLayout* pLayout);
		PackLayout_p	layout() const { return m_pLayout; }

		//.____ Misc ________________________________________________________________
		
		bool			setSlotWeight(int index, int amount, float weight);
		bool			setSlotWeight(iterator  beg, iterator end, float weight);
		bool			setSlotWeight(int index, int amount, std::initializer_list<float> weights);
		bool			setSlotWeight(iterator beg, iterator end, std::initializer_list<float> weights);

		bool			setSlotPadding(int index, int amount, Border padding);
		bool			setSlotPadding(iterator beg, iterator end, Border padding);
		bool			setSlotPadding(int index, int amount, std::initializer_list<Border> padding);
		bool			setSlotPadding(iterator beg, iterator end, std::initializer_list<Border> padding);

		void			hideSlots(int index, int amount);
		void			hideSlots(iterator beg, iterator end);

		void			unhideSlots(int index, int amount);
		void			unhideSlots(iterator beg, iterator end);


		//.____ Internal ______________________________________________________

		spx				_matchingHeight(spx width, int scale) const override;
		spx				_matchingWidth(spx height, int scale) const override;

		SizeSPX			_defaultSize(int scale) const override;


	protected:
		PackPanel();
		PackPanel(const Blueprint& bp);
		virtual ~PackPanel();

		// Overloaded from Widget

		void		_resize( const SizeSPX& size, int scale ) override;


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

		void		_reweightSlots(Slot * pSlot, int nb, float weight);
		void		_reweightSlots(Slot * pSlot, int nb, const float * pWeights);
//		void		_refreshChildGeo() { _refreshChildGeo(true); }

		//

		void		_refreshChildGeo(bool bRequestRender);

		void		_hideChildren(Slot * pSlot, int nb);
		void		_unhideChildren(Slot * pSlot, int nb);

		void		_refreshGeometries();
		SizeSPX		_calcDefaultSize( int scale ) const;
		int			_populateLayoutArray( PackLayout::Item * pArray ) const;
		int			_populateLayoutArray(PackLayout::Item* pArray, spx forcedBreadth ) const;

		bool			m_bHorizontal;
		PackLayout_p	m_pLayout;
		SizeSPX			m_defaultContentSize;

	};



} // namespace wg
#endif //WG_PACKPANEL_DOT_H
