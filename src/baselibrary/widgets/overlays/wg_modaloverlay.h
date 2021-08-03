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

#ifndef WG_MODALOVERLAY_DOT_H
#define WG_MODALOVERLAY_DOT_H
#pragma once

#include <wg_overlay.h>
#include <wg_cdynamicslotvector.h>

namespace wg
{

	class ModalOverlay;
	typedef	StrongPtr<ModalOverlay>	ModalOverlay_p;
	typedef	WeakPtr<ModalOverlay>		ModalOverlay_wp;

	//____ ModalOverlay __________________________________________________________

	class ModalOverlay : public Overlay
	{
		friend class Slot;
		friend class CSlots;

	public:

		//____ Slot ___________________________________________________________

		class Slot : public Overlay::Slot
		{
			friend class ModalOverlay;
			friend class CSlots;
			template<class S> friend class CDynamicSlotVector;
			template<class S> friend class SlotVector;

		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Geometry _________________________________________________

			void			setOrigo(const Placement origo);
			inline Placement	origo() const { return m_origo; }

			void			setGeo(const Rect& geometry);

			void			setOffset(const Coord& ofs);
			inline Coord	offset() const { return m_placementGeo.pos(); }

			void			setSize(const Size& size);

			void			move(const Coord& ofs);

		protected:

			Slot(SlotHolder * pHolder) : Overlay::Slot(pHolder) {}

			inline ModalOverlay * _holder() { return static_cast<ModalOverlay*>(Overlay::Slot::_holder()); }
			inline const ModalOverlay * _holder() const { return static_cast<const ModalOverlay*>(Overlay::Slot::_holder()); }

			const static bool safe_to_relocate = false;

			bool		m_bVisible = true;
			Placement	m_origo = Placement::NorthWest;
			Rect		m_placementGeo;			// Widgets geo relative anchor and hotspot. Setting width and height to 0 uses Widgets preferredSize() dynamically.
												// Setting just one of them to 0 uses Widgets matchingHeight() or matchingWidth() dynamically.
			Widget_wp	m_pKeyFocus;			// Pointer at child that held focus when this modal was last on top.

		};


		class CSlots;
		typedef	StrongComponentPtr<CSlots>	CModalSlotVector_p;
		typedef	WeakComponentPtr<CSlots>	CModalSlotVector_wp;

		//____ CSlots ________________________________________________________

		class CSlots : public CDynamicSlotVector<Slot>
		{
			friend class ModalOverlay;
		public:

			//.____ Content _______________________________________________________

			iterator	pushFront(const Widget_p& pWidget, const Rect& geometry, Placement origo = Placement::NorthWest);
			iterator	pushFront(const Widget_p& pWidget, const Coord& pos, Placement origo = Placement::NorthWest) { return pushFront(pWidget, Rect(pos, 0, 0), origo); }

			iterator	pushBack(const Widget_p& pWidget, const Rect& geometry, Placement origo = Placement::NorthWest);
			iterator	pushBack(const Widget_p& pWidget, const Coord& pos, Placement origo = Placement::NorthWest) { return pushBack(pWidget, Rect(pos, 0, 0), origo); }

			//.____ Misc __________________________________________________________

			inline CModalSlotVector_p	ptr() { return CModalSlotVector_p(this); }

		protected:

			CSlots(SlotHolder * pHolder) : CDynamicSlotVector<Slot>(pHolder) {}

			const ModalOverlay *	_holder() const { return static_cast<const ModalOverlay*>(CDynamicSlotVector<Slot>::_holder()); }
			ModalOverlay *	_holder() { return static_cast<ModalOverlay*>(CDynamicSlotVector<Slot>::_holder()); }

		};


		//.____ Creation __________________________________________

		static ModalOverlay_p	create() { return ModalOverlay_p(new ModalOverlay()); }

		//.____ Components _______________________________________

		CSlots		modalSlots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal ______________________________________________________

		spx				_matchingHeight(spx width, int scale = -1) const override;
		spx				_matchingWidth(spx height, int scale = -1) const override;

		SizeSPX			_preferredSize(int scale = -1) const override;

	protected:
		ModalOverlay();
		virtual ~ModalOverlay();
		virtual Widget* _newOfMyType() const  override { return new ModalOverlay(); };


	private:

		void            _refreshRealGeo(Slot * pSlot, bool bForceResize = false);

		ModalOverlay *	_getModalOverlay() const  override { return const_cast<ModalOverlay*>(this); }

		void			_updateKeyboardFocus();

		// Overloaded from Widget

		void			_cloneContent(const Widget * _pOrg) override;
		void			_resize(const SizeSPX& size, int scale = -1) override;
		void			_receive(Msg * pMsg) override;

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *		_findWidget( const CoordSPX& ofs, SearchMode mode ) override;

		//TODO: We should allow replacement of modal slots.
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override { return Overlay::_replaceChild(pSlot, pNewChild); }

		void			_childRequestResize( StaticSlot * pSlot ) override;
		void			_releaseChild(StaticSlot * pSlot) override;

		void			_didAddSlots(StaticSlot * pSlot, int nb) override;
		void			_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void			_willEraseSlots(StaticSlot * pSlot, int nb) override;
		void			_hideSlots(StaticSlot *, int nb) override;
		void			_unhideSlots(StaticSlot *, int nb) override;

		// Overloaded from Overlay

		const Overlay::Slot * _beginOverlaySlots() const override;
		const Overlay::Slot * _endOverlaySlots() const override;
		int					_sizeOfOverlaySlot() const override;

		Widget_wp			m_pBaseKeyFocus;
	};


} // namespace wg
#endif //WG_MODALOVERLAY_DOT_H
