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
#include <wg_cdynamicslotarray.h>

namespace wg
{

	//____ ModalSlot ___________________________________________________________

	class ModalSlot : public LayerSlot
	{
		friend class ModalLayer;
		friend class CModalSlotArray;
		template<class S> friend class CDynamicSlotArray;
		template<class S> friend class SlotArray;

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
		
		class Holder : public LayerSlot::Holder
		{
		public:
			virtual void	_refreshRealGeo(ModalSlot * pSlot, bool bForceResize = false) = 0;
		};

		ModalSlot(Holder * pHolder) : LayerSlot(pHolder) {}

		inline Holder * _holder() { return static_cast<Holder*>(LayerSlot::_holder()); }
		inline const Holder * _holder() const { return static_cast<const Holder*>(LayerSlot::_holder()); }

		const static bool safe_to_relocate = false;

		bool		m_bVisible = true;
		Origo		m_origo = Origo::NorthWest;
		RectI		m_placementGeo;			// Widgets geo relative anchor and hotspot. Setting width and height to 0 uses Widgets preferredSize() dynamically.
											// Setting just one of them to 0 uses Widgets matchingHeight() or matchingWidth() dynamically.
		Widget_wp	m_pKeyFocus;			// Pointer at child that held focus when this modal was last on top.

	};


	class CModalSlotArray;
	typedef	StrongComponentPtr<CModalSlotArray>	CModalSlotArray_p;
	typedef	WeakComponentPtr<CModalSlotArray>	CModalSlotArray_wp;

	//____ CModalSlotArray ________________________________________________________

	class CModalSlotArray : public CDynamicSlotArray<ModalSlot>
	{
		friend class ModalLayer;
	public:

		//.____ Misc __________________________________________________________

		inline CModalSlotArray_p	ptr() { return CModalSlotArray_p(this); }

		//.____ Content _______________________________________________________

		iterator	pushFront( const Widget_p& pWidget, const Rect& geometry, Origo origo = Origo::NorthWest );
		iterator	pushFront(const Widget_p& pWidget, const Coord& pos, Origo origo = Origo::NorthWest ) { return pushFront( pWidget, Rect(pos,0,0), origo); }

		iterator	pushBack(const Widget_p& pWidget, const Rect& geometry, Origo origo = Origo::NorthWest);
		iterator	pushBack(const Widget_p& pWidget, const Coord& pos, Origo origo = Origo::NorthWest) { return pushBack(pWidget, Rect(pos, 0, 0), origo); }


	protected:

		//____ Holder _________________________________________________

		class Holder : public CDynamicSlotArray<ModalSlot>::Holder
		{
		};

		CModalSlotArray(Holder * pHolder) : CDynamicSlotArray<ModalSlot>(pHolder) {}

		const Holder *	_holder() const { return static_cast<const Holder*>(CDynamicSlotArray<ModalSlot>::_holder()); }
		Holder *	_holder() { return static_cast<Holder*>(CDynamicSlotArray<ModalSlot>::_holder()); }


	};


	class ModalLayer;
	typedef	StrongPtr<ModalLayer>	ModalLayer_p;
	typedef	WeakPtr<ModalLayer>		ModalLayer_wp;

	//____ ModalLayer __________________________________________________________

	class ModalLayer : public Layer, protected CModalSlotArray::Holder
	{
		friend class CModalSlotArray;

	public:

		//.____ Creation __________________________________________

		static ModalLayer_p	create() { return ModalLayer_p(new ModalLayer()); }

		//.____ Components _______________________________________

		CModalSlotArray		modalSlots;

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

		ModalLayer *	_getModalLayer() const  override { return const_cast<ModalLayer*>(this); }

		void			_updateKeyboardFocus();

		// Overloaded from Container

		Widget *		_findWidget( const CoordI& ofs, SearchMode mode ) override;

		// Overloaded from WidgetHolder

		Container *		_container() override { return this; }
		RootPanel *		_root() override { return Container::_root(); }
		Object *		_object() override { return this; }
		const Object *	_object() const override { return this; }

		CoordI			_childPos(const StaticSlot * pSlot) const override { return Layer::_childPos(pSlot); }
		CoordI			_childGlobalPos(const StaticSlot * pSlot) const override { return Layer::_childGlobalPos(pSlot); }
		bool			_isChildVisible(const StaticSlot * pSlot) const override { return Layer::_isChildVisible(pSlot); }
		RectI			_childWindowSection(const StaticSlot * pSlot) const override { return Layer::_childWindowSection(pSlot); }

		void			_childRequestRender(StaticSlot * pSlot) override { return Layer::_childRequestRender(pSlot); }
		void			_childRequestRender(StaticSlot * pSlot, const RectI& rect) override { return Layer::_childRequestRender(pSlot); }

		bool			_childRequestFocus(StaticSlot * pSlot, Widget * pWidget) override { return Layer::_childRequestFocus(pSlot, pWidget); }
		bool			_childReleaseFocus(StaticSlot * pSlot, Widget * pWidget) override { return Layer::_childReleaseFocus(pSlot, pWidget); }

		void			_childRequestInView(StaticSlot * pSlot) override { return Layer::_childRequestInView(pSlot); }
		void			_childRequestInView(StaticSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Layer::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		Widget *		_prevChild(const StaticSlot * pSlot) const override { return Layer::_prevChild(pSlot); }
		Widget *		_nextChild(const StaticSlot * pSlot) const override { return Layer::_nextChild(pSlot); }

		//TODO: We should allow replacement of modal slots.
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override { return Layer::_replaceChild(pSlot, pNewChild); }

		void			_childRequestResize( StaticSlot * pSlot ) override;
		void			_releaseChild(StaticSlot * pSlot) override;

		// Methods for ModalSlotsHolder

		void			_didAddSlots(StaticSlot * pSlot, int nb) override;
		void			_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void			_willEraseSlots(StaticSlot * pSlot, int nb) override;
		void			_hideSlots(StaticSlot *, int nb) override;
		void			_unhideSlots(StaticSlot *, int nb) override;
		void            _refreshRealGeo( ModalSlot * pSlot, bool bForceResize = false ) override;

		// Overloaded from Layer

		const LayerSlot * 	_beginLayerSlots() const override;
		const LayerSlot * 	_endLayerSlots() const override;
		int				_sizeOfLayerSlot() const override;

		// Overloaded from Widget

		int				_matchingHeight(int width) const override;
		int				_matchingWidth(int height) const override;

		SizeI			_preferredSize() const override;

		void			_cloneContent( const Widget * _pOrg ) override;
		void			_resize( const SizeI& size ) override;
		void			_receive( Msg * pMsg ) override;

		//

		class MainSlotAccess : public CStandardSlot { friend class ModalLayer; };
		MainSlotAccess * _baseSlot() { return static_cast<MainSlotAccess*>(&mainSlot); }
		const MainSlotAccess * _baseSlot() const { return static_cast<const MainSlotAccess*>(&mainSlot); }


		Widget_wp			m_pBaseKeyFocus;

	};


} // namespace wg
#endif //WG_MODALLAYER_DOT_H
