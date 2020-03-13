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

#ifndef WG_MODALLAYER_DOT_H
#define WG_MODALLAYER_DOT_H
#pragma once

#include <wg_layer.h>
#include <wg_cdynamicslotvector.h>

namespace wg
{

	class ModalLayer;
	typedef	StrongPtr<ModalLayer>	ModalLayer_p;
	typedef	WeakPtr<ModalLayer>		ModalLayer_wp;

	//____ ModalLayer __________________________________________________________

	class ModalLayer : public Layer
	{
		friend class Slot;
		friend class CSlots;

	public:

		//____ Slot ___________________________________________________________

		class Slot : public Layer::Slot
		{
			friend class ModalLayer;
			friend class CSlots;
			template<class S> friend class CDynamicSlotVector;
			template<class S> friend class SlotVector;

		public:

			//.____ Geometry _________________________________________________

			void			setOrigo(const Origo origo);
			inline Origo	origo() const { return m_origo; }

			void			setGeo(const Rect& geometry);

			void			setOffset(const Coord& ofs);
			inline Coord	offset() const { return Util::rawToQpix(m_placementGeo.pos()); }

			void			setSize(const Size& size);

			void			move(const Coord& ofs);

		protected:

			Slot(SlotHolder * pHolder) : Layer::Slot(pHolder) {}

			inline ModalLayer * _holder() { return static_cast<ModalLayer*>(Layer::Slot::_holder()); }
			inline const ModalLayer * _holder() const { return static_cast<const ModalLayer*>(Layer::Slot::_holder()); }

			const static bool safe_to_relocate = false;

			bool		m_bVisible = true;
			Origo		m_origo = Origo::NorthWest;
			RectI		m_placementGeo;			// Widgets geo relative anchor and hotspot. Setting width and height to 0 uses Widgets preferredSize() dynamically.
												// Setting just one of them to 0 uses Widgets matchingHeight() or matchingWidth() dynamically.
			Widget_wp	m_pKeyFocus;			// Pointer at child that held focus when this modal was last on top.

		};


		class CSlots;
		typedef	StrongComponentPtr<CSlots>	CModalSlotVector_p;
		typedef	WeakComponentPtr<CSlots>	CModalSlotVector_wp;

		//____ CSlots ________________________________________________________

		class CSlots : public CDynamicSlotVector<Slot>
		{
			friend class ModalLayer;
		public:

			//.____ Misc __________________________________________________________

			inline CModalSlotVector_p	ptr() { return CModalSlotVector_p(this); }

			//.____ Content _______________________________________________________

			iterator	pushFront(const Widget_p& pWidget, const Rect& geometry, Origo origo = Origo::NorthWest);
			iterator	pushFront(const Widget_p& pWidget, const Coord& pos, Origo origo = Origo::NorthWest) { return pushFront(pWidget, Rect(pos, 0, 0), origo); }

			iterator	pushBack(const Widget_p& pWidget, const Rect& geometry, Origo origo = Origo::NorthWest);
			iterator	pushBack(const Widget_p& pWidget, const Coord& pos, Origo origo = Origo::NorthWest) { return pushBack(pWidget, Rect(pos, 0, 0), origo); }


		protected:

			CSlots(SlotHolder * pHolder) : CDynamicSlotVector<Slot>(pHolder) {}

			const ModalLayer *	_holder() const { return static_cast<const ModalLayer*>(CDynamicSlotVector<Slot>::_holder()); }
			ModalLayer *	_holder() { return static_cast<ModalLayer*>(CDynamicSlotVector<Slot>::_holder()); }

		};


		//.____ Creation __________________________________________

		static ModalLayer_p	create() { return ModalLayer_p(new ModalLayer()); }

		//.____ Components _______________________________________

		CSlots		modalSlots;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static ModalLayer_p	cast( Object * pObject );


	protected:
		ModalLayer();
		virtual ~ModalLayer();
		virtual Widget* _newOfMyType() const  override { return new ModalLayer(); };


	private:

		void            _refreshRealGeo(Slot * pSlot, bool bForceResize = false);

		ModalLayer *	_getModalLayer() const  override { return const_cast<ModalLayer*>(this); }

		void			_updateKeyboardFocus();

		// Overloaded from Widget

		int				_matchingHeight(int width) const override;
		int				_matchingWidth(int height) const override;

		SizeI			_preferredSize() const override;

		void			_cloneContent(const Widget * _pOrg) override;
		void			_resize(const SizeI& size) override;
		void			_receive(Msg * pMsg) override;

		// Overloaded from Container

		Widget *		_findWidget( const CoordI& ofs, SearchMode mode ) override;

		//TODO: We should allow replacement of modal slots.
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override { return Layer::_replaceChild(pSlot, pNewChild); }

		void			_childRequestResize( StaticSlot * pSlot ) override;
		void			_releaseChild(StaticSlot * pSlot) override;

		void			_didAddSlots(StaticSlot * pSlot, int nb) override;
		void			_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void			_willEraseSlots(StaticSlot * pSlot, int nb) override;
		void			_hideSlots(StaticSlot *, int nb) override;
		void			_unhideSlots(StaticSlot *, int nb) override;

		// Overloaded from Layer

		const Layer::Slot * _beginLayerSlots() const override;
		const Layer::Slot * _endLayerSlots() const override;
		int					_sizeOfLayerSlot() const override;

		//

		class MainSlotAccess : public CStandardSlot { friend class ModalLayer; };
		MainSlotAccess * _baseSlot() { return static_cast<MainSlotAccess*>(&mainSlot); }
		const MainSlotAccess * _baseSlot() const { return static_cast<const MainSlotAccess*>(&mainSlot); }

		Widget_wp			m_pBaseKeyFocus;
	};


} // namespace wg
#endif //WG_MODALLAYER_DOT_H
