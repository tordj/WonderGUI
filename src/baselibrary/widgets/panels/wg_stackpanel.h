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
#include <wg_paddedslot.h>
#include <wg_cpaddedslotvector.h>


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
		friend class Slot;
		friend class CSlots;

	public:

		//____ Slot ____________________________________________________________

		class Slot : public PaddedSlot
		{
			friend class StackPanel;
			friend class CSlots;
			friend class CDynamicSlotVector<Slot>;

		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Properties _________________________________________________

			void				setSizePolicy(SizePolicy2D policy) { _holder()->_setSizePolicy(this, policy); }
			inline SizePolicy2D sizePolicy() const { return m_sizePolicy; }

			void				setPlacement(Placement placement) { _holder()->_setPlacement(this, placement); }
			inline Placement	placement() const { return m_placement; }

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:

			Slot(SlotHolder * pHolder) : PaddedSlot(pHolder) {}
			Slot(Slot&& o) = default;
			Slot& operator=(Slot&& o) = default;


			StackPanel *		_holder() { return static_cast<StackPanel*>(PaddedSlot::_holder()); }
			const StackPanel *	_holder() const { return static_cast<const StackPanel*>(PaddedSlot::_holder()); }

			Placement			m_placement = Placement::Center;
			SizePolicy2D	m_sizePolicy = SizePolicy2D::Original;
		};


		//____ CSlots ________________________________________________________

		class CSlots;
		typedef	StrongComponentPtr<CSlots>	CSlots_p;
		typedef	WeakComponentPtr<CSlots>	CSlots_wp;

		class CSlots : public CPaddedSlotVector<Slot>
		{
			friend class StackPanel;
		public:

			//.____ Misc __________________________________________________________

			inline CSlots_p	ptr() { return CSlots_p(this); }

		protected:

			CSlots(SlotHolder * pHolder) : CPaddedSlotVector<Slot>(pHolder) {}
		};


		//.____ Creation __________________________________________

		static StackPanel_p	create() { return StackPanel_p(new StackPanel()); }

		//.____ Components _______________________________________

		CSlots		slots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		MU			matchingHeight(MU width) const override;
		MU			matchingWidth(MU height) const override;

		Size		preferredSize() const override;


	protected:
		StackPanel();
		virtual ~StackPanel();
		virtual Widget* _newOfMyType() const override { return new StackPanel(); };

		// Overloaded from Widget

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_resize( const Size& size ) override;

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		Coord		_childPos(const StaticSlot * pSlot) const override;

		void		_childRequestRender( StaticSlot * pSlot ) override;
		void		_childRequestRender( StaticSlot * pSlot, const Rect& rect ) override;
		void		_childRequestResize( StaticSlot * pSlot ) override;

		Widget *	_prevChild( const StaticSlot * pSlot ) const override;
		Widget *	_nextChild( const StaticSlot * pSlot ) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild ) override;

		void		_didAddSlots(StaticSlot * pSlot, int nb) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots(StaticSlot * pSlot, int nb) override;
		void		_hideSlots(StaticSlot *, int nb) override;
		void		_unhideSlots(StaticSlot *, int nb) override;
		void		_repadSlots(StaticSlot *, int nb, Border padding) override;
		void		_repadSlots(StaticSlot *, int nb, const Border * pPaddings) override;

		//

		void		_setSizePolicy(Slot * pSlot, SizePolicy2D policy);
		void		_setPlacement(Slot * pSlot, Placement placement);


		// Internal to StackPanel

		Size 	_calcPreferredSize();
		void	_adaptChildrenToSize();

		void	_hideChildren( Slot * pSlot, int nb );
		void	_unhideChildren( Slot * pSlot, int nb );


		Rect	_childGeo( const Slot * pSlot ) const;

		Size	m_preferredSize;
	};


} // namespace wg
#endif //WG_STACKPANEL_DOT_H
