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
#include <wg_paddedslot.h>
#include <wg_cpaddedslotvector.h>

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

		class Slot : public PaddedSlot
		{
			friend class PackPanel;
			friend class CSlots;
			template<class S> friend class SlotVector;
			friend class CDynamicSlotVector<Slot>;

		public:
			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Properties _________________________________________________

			inline float	weight() const { return m_weight; }
			void			setWeight(float weight);

			//.____ Geometry _________________________________________________

			inline Coord	pos() const { return m_geo.pos(); }
			inline Size		size() const { return m_geo.size(); }
			inline Rect		geo() const { return m_geo; }

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:

			Slot(SlotHolder *pHolder) : PaddedSlot(pHolder) {}
			Slot(Slot&& o) = default;
			Slot& operator=(Slot&& o) = default;

			bool			m_bResizeRequired = false;
			float			m_weight = 1.f;				// Weight for space allocation.
			Rect			m_geo;						// Real geo of child (no padding included).
			Size			m_preferredSize;				// Cached padded preferred size from the child.
		};


		//____ CSlots ________________________________________________________

		class CSlots;
		typedef	StrongComponentPtr<CSlots>	CSlots_p;
		typedef	WeakComponentPtr<CSlots>	CSlots_wp;

		class CSlots : public CPaddedSlotVector<Slot>
		{ 
			friend class PackPanel;
		public:

			//.____ Geometry _______________________________________________________

			bool		setWeight(int index, int amount, float weight);
			bool		setWeight(const SlotIterator&  beg, const SlotIterator&  end, float weight);
			bool		setWeight(int index, int amount, const std::initializer_list<float> weights);
			bool		setWeight(const SlotIterator&  beg, const SlotIterator&  end, const std::initializer_list<float> weights);

			//.____ Misc __________________________________________________________

			inline CSlots_p	ptr() { return CSlots_p(this); }

		protected:
			CSlots(SlotHolder * pHolder) : CPaddedSlotVector<Slot>(pHolder) {}

			inline PackPanel *		_holder() { return static_cast<PackPanel*>(CPaddedSlotVector<Slot>::_holder()); }
			inline const PackPanel *	_holder() const { return static_cast<const PackPanel*>(CPaddedSlotVector<Slot>::_holder()); }
		};


		//.____ Creation __________________________________________

		static PackPanel_p	create() { return PackPanel_p(new PackPanel()); }

		//.____ Components _______________________________________

		CSlots		slots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		void			setAxis( Axis orientaiton );
		Axis		axis() const { return m_bHorizontal?Axis::X:Axis::Y; }

		MU				matchingHeight(MU width) const override;
		MU				matchingWidth(MU height) const override;

		Size			preferredSize() const override;


		//.____ Behavior ________________________________________________________

		void			setSizeBroker( SizeBroker * pBroker );
		SizeBroker_p	sizeBroker() const { return m_pSizeBroker; }

	protected:
		PackPanel();
		virtual ~PackPanel();
		virtual Widget* _newOfMyType() const  override { return new PackPanel(); };

		// Overloaded from Widget

		void			_resize( const Size& size ) override;


		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		Coord		_childPos(const StaticSlot * pSlot) const override;

		void		_childRequestRender(StaticSlot * pSlot) override;
		void		_childRequestRender(StaticSlot * pSlot, const Rect& rect) override;
		void		_childRequestResize(StaticSlot * pSlot) override;

		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewWidget) override;

		void		_didAddSlots(StaticSlot * pSlot, int nb) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots(StaticSlot * pSlot, int nb) override;
		void		_hideSlots(StaticSlot *, int nb) override;
		void		_unhideSlots(StaticSlot *, int nb) override;
		void		_repadSlots(StaticSlot *, int nb, Border padding) override;
		void		_repadSlots(StaticSlot *, int nb, const Border * pPaddings) override;

		// Needed by CSlots

		void		_reweightSlots(Slot * pSlot, int nb, float weight);
		void		_reweightSlots(Slot * pSlot, int nb, const float * pWeights);
		void		_refreshChildGeo() { _refreshChildGeo(true); }

		//

		void		_refreshChildGeo(bool bRequestRender);

		void		_hideChildren(Slot * pSlot, int nb);
		void		_unhideChildren(Slot * pSlot, int nb);

		void		_refreshGeometries();
		Size		_calcPreferredSize();
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray ) const;
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray, MU forcedBreadth ) const;

		MU			_setItemLengths(SizeBrokerItem * pItems, int nItems, MU totalLength) const;
		MU			_setPreferredLengths(SizeBrokerItem * pItems, int nItems) const;

		bool			m_bHorizontal;
		SizeBroker_p	m_pSizeBroker;
		Size			m_preferredContentSize;

	};



} // namespace wg
#endif //WG_PACKPANEL_DOT_H
