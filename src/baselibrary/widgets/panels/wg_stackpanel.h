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

#ifndef WG_STACKPANEL_DOT_H
#define WG_STACKPANEL_DOT_H
#pragma once

#include <wg_panel.h>
#include <wg_slot.h>
#include <wg_slotextras.h>
#include <wg_cdynamicslotvector.h>

namespace wg
{

	class StackPanel;
	typedef	StrongPtr<StackPanel>	StackPanel_p;
	typedef	WeakPtr<StackPanel>		StackPanel_wp;


	//____ StackPanel ___________________________________________________________

	/**
	*/

	class StackPanel : public Panel
	{ 

	public:

		//____ Slot ____________________________________________________________

		class Slot : public DynamicSlot
		{
			friend class StackPanel;
			friend class CSlots;
			friend class CDynamicSlotVector<Slot>;

		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Geometry ______________________________________________________

			SLOT_PADDING_METHODS(StackPanel)

			//.____ Appearance ________________________________________________

			SLOT_HIDING_METHODS(StackPanel)

			//.____ Properties _________________________________________________

			void				setSizePolicy(SizePolicy2D policy) { _holder()->_setSizePolicy(this, policy); }
			inline SizePolicy2D sizePolicy() const { return m_sizePolicy; }

			void				setPlacement(Placement placement) { _holder()->_setPlacement(this, placement); }
			inline Placement	placement() const { return m_placement; }

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:

			Slot(SlotHolder * pHolder) : DynamicSlot(pHolder) {}
			Slot(Slot&& o) = default;
			Slot& operator=(Slot&& o) = default;

			StackPanel* _holder() { return static_cast<StackPanel*>(DynamicSlot::_holder()); }
			const StackPanel* _holder() const { return static_cast<const StackPanel*>(DynamicSlot::_holder()); }

			SLOT_PADDING_HELPER_METHODS

			Border				m_padding;
			bool				m_bVisible;

			Placement			m_placement = Placement::Center;
			SizePolicy2D		m_sizePolicy = SizePolicy2D::Original;
			CoordSPX			m_position;
		};

		using		iterator = SlotArrayIterator<Slot>;

		//____ CSlots ________________________________________________________

		class CSlots;
		typedef	StrongComponentPtr<CSlots>	CSlots_p;
		typedef	WeakComponentPtr<CSlots>	CSlots_wp;

		class CSlots : public CDynamicSlotVector<Slot>, 
			public PaddedSlotCollectionMethods<Slot,iterator,StackPanel>,
			public HideableSlotCollectionMethods<Slot, iterator, StackPanel>
		{
			friend class StackPanel;
		public:

			//.____ Misc __________________________________________________________

			inline CSlots_p	ptr() { return CSlots_p(this); }

		protected:
			
			inline StackPanel* _holder() { return static_cast<StackPanel*>(CDynamicSlotVector<Slot>::_holder()); }
			inline const StackPanel* _holder() const { return static_cast<const StackPanel*>(CDynamicSlotVector<Slot>::_holder()); }

			Slot* _slot(int index) { return CDynamicSlotVector<Slot>::_slot(index); }
			inline int _size() const override { return CDynamicSlotVector<Slot>::size(); }

			CSlots(SlotHolder * pHolder) : CDynamicSlotVector<Slot>(pHolder) {}
		};

		friend class Slot;
		friend class CSlots;
		friend class PaddedSlotCollectionMethods<StackPanel::Slot, iterator, StackPanel>;
		friend class HideableSlotCollectionMethods<StackPanel::Slot, iterator, StackPanel>;

		//.____ Creation __________________________________________

		static StackPanel_p	create() { return StackPanel_p(new StackPanel()); }

		//.____ Components _______________________________________

		CSlots		slots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal _________________________________________________

		spx			_matchingHeight(spx width, int scale = -1) const override;
		spx			_matchingWidth(spx height, int scale = -1) const override;

		SizeSPX		_defaultSize(int scale = -1) const override;


	protected:
		StackPanel();
		virtual ~StackPanel();

		// Overloaded from Widget

		void		_resize( const SizeSPX& size, int scale = -1 ) override;

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		CoordSPX	_childPos(const StaticSlot * pSlot) const override;

		void		_childRequestRender( StaticSlot * pSlot ) override;
		void		_childRequestRender( StaticSlot * pSlot, const RectSPX& rect ) override;
		void		_childRequestResize( StaticSlot * pSlot ) override;

		Widget *	_prevChild( const StaticSlot * pSlot ) const override;
		Widget *	_nextChild( const StaticSlot * pSlot ) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild ) override;

		void		_didAddSlots(StaticSlot * pSlot, int nb) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots(StaticSlot * pSlot, int nb) override;
		void		_hideSlots(StaticSlot *, int nb);
		void		_unhideSlots(StaticSlot *, int nb);
		void		_repadSlots(StaticSlot *, int nb, Border padding);
		void		_repadSlots(StaticSlot *, int nb, const Border * pPaddings);

		//

		void		_setSizePolicy(Slot * pSlot, SizePolicy2D policy);
		void		_setPlacement(Slot * pSlot, Placement placement);


		// Internal to StackPanel

		SizeSPX _calcDefaultSize(int scale) const;
		void	_updateChildGeo(Slot * pBegin, Slot * pEnd);

		void	_hideChildren( Slot * pBegin, Slot * pEnd );
		void	_unhideChildren( Slot * pBegin, Slot * pEnd );


		RectSPX	_childGeo( const Slot * pSlot ) const;

		SizeSPX	m_defaultSize;
	};


} // namespace wg
#endif //WG_STACKPANEL_DOT_H
