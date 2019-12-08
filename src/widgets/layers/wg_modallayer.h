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
#include <wg_cslotarray.h>

namespace wg
{

	class ModalLayer;
	typedef	StrongPtr<ModalLayer>	ModalLayer_p;
	typedef	WeakPtr<ModalLayer>		ModalLayer_wp;

	//____ ModalSlot ___________________________________________________________

	class ModalSlot : public LayerSlot		/** @private */
	{
		friend class ModalLayer;
		friend class CModalSlotArray;
		template<class S> friend class CSlotArray;
		template<class S> friend class SlotArray;

	protected:
		
		class Holder : public LayerSlot::Holder
		{
		};

		ModalSlot(Holder * pHolder) : LayerSlot(pHolder) {}

		const static bool safe_to_relocate = false;

		bool		bVisible = true;
		Origo		origo = Origo::NorthWest;
		RectI		placementGeo;		// Widgets geo relative anchor and hotspot. Setting width and height to 0 uses Widgets preferredSize() dynamically.
										// Setting just one of them to 0 uses Widgets matchingHeight() or matchingWidth() dynamically.
		Widget_wp	pKeyFocus;			// Pointer at child that held focus when this modal was last on top.

	};


	class CModalSlotArray;
	typedef	StrongComponentPtr<CModalSlotArray>	CModalSlotArray_p;
	typedef	WeakComponentPtr<CModalSlotArray>	CModalSlotArray_wp;

	//____ CModalSlotArray ________________________________________________________

	class CModalSlotArray : public CSlotArray<ModalSlot>
	{
		friend class ModalLayer;
	public:

		//____ Holder _________________________________________________

		class Holder : public CSlotArray<ModalSlot>::Holder		/** @private */
		{
		public:
			virtual void	_refreshRealGeo(ModalSlot * pSlot, bool bForceResize = false) = 0;
		};


		/** @private */

		CModalSlotArray( Holder * pHolder ) : CSlotArray<ModalSlot>(pHolder) {}

		//.____ Misc __________________________________________________________

		inline CModalSlotArray_p	ptr() { return CModalSlotArray_p(this); }

		//.____ Content _______________________________________________________

		iterator	add( Widget * pWidget, const Rect& geometry, Origo origo = Origo::NorthWest );
		iterator	add( Widget * pWidget, const Coord& pos, Origo origo = Origo::NorthWest ) { return add( pWidget, Rect(pos,0,0), origo); }

		//.____ Geometry ______________________________________________________

		void		setOrigo( int index, const Origo origo );
		void		setOrigo( iterator it, const Origo origo );
		Origo		origo( int index ) const;
		Origo		origo( iterator it ) const;

		void		setGeo( int index, const Rect& geometry );
		void		setGeo( iterator it, const Rect& geometry );
		Rect		geo( int index ) const;
		Rect		geo( iterator it ) const;

		void		setOfs( int index, const Coord& ofs );
		void		setOfs( iterator it, const Coord& ofs );
		Coord		ofs( int index ) const;
		Coord		ofs( iterator it ) const;

		void		setSize( int index, const Size& size );
		void		setSize( iterator it, const Size& size );
		Size		size( int index ) const;
		Size		size( iterator it ) const;

		void		move( int index, const Coord& ofs );
		void		move( iterator it, const Coord& ofs );

	protected:
		void 		_setOrigo(ModalSlot * p, const Origo origo);
		void 		_setGeo(ModalSlot * p, const Rect& geometry);
		void 		_setOfs(ModalSlot * p, const Coord& ofs);
		void 		_setSize(ModalSlot * p, const Size& size);
		void 		_move( ModalSlot * p, const Coord& ofs );

		const Holder *	_holder() const { return static_cast<const Holder*>(CSlotArray<ModalSlot>::_holder()); }
		Holder *	_holder() { return static_cast<Holder*>(CSlotArray<ModalSlot>::_holder()); }


	};


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

		CoordI			_childPos(const BasicSlot * pSlot) const override { return Layer::_childPos(pSlot); }
		CoordI			_childGlobalPos(const BasicSlot * pSlot) const override { return Layer::_childGlobalPos(pSlot); }
		bool			_isChildVisible(const BasicSlot * pSlot) const override { return Layer::_isChildVisible(pSlot); }
		RectI			_childWindowSection(const BasicSlot * pSlot) const override { return Layer::_childWindowSection(pSlot); }

		void			_childRequestRender(BasicSlot * pSlot) override { return Layer::_childRequestRender(pSlot); }
		void			_childRequestRender(BasicSlot * pSlot, const RectI& rect) override { return Layer::_childRequestRender(pSlot); }

		bool			_childRequestFocus(BasicSlot * pSlot, Widget * pWidget) override { return Layer::_childRequestFocus(pSlot, pWidget); }
		bool			_childReleaseFocus(BasicSlot * pSlot, Widget * pWidget) override { return Layer::_childReleaseFocus(pSlot, pWidget); }

		void			_childRequestInView(BasicSlot * pSlot) override { return Layer::_childRequestInView(pSlot); }
		void			_childRequestInView(BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Layer::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		Widget *		_prevChild(const BasicSlot * pSlot) const override { return Layer::_prevChild(pSlot); }
		Widget *		_nextChild(const BasicSlot * pSlot) const override { return Layer::_nextChild(pSlot); }

		//TODO: We should allow replacement of modal slots.
		void			_replaceChild(BasicSlot * pSlot, Widget * pNewChild) override { return Layer::_replaceChild(pSlot, pNewChild); }

		void			_childRequestResize( BasicSlot * pSlot ) override;
		void			_releaseChild(BasicSlot * pSlot) override;

		// Methods for ModalSlotsHolder

		void			_didAddSlots(BasicSlot * pSlot, int nb) override;
		void			_didMoveSlots(BasicSlot * pFrom, BasicSlot * pTo, int nb) override;
		void			_willRemoveSlots(BasicSlot * pSlot, int nb) override;
		void			_hideSlots(BasicSlot *, int nb) override;
		void			_unhideSlots(BasicSlot *, int nb) override;
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

		class MainSlotAccess : public CBasicSlot { friend class ModalLayer; };
		MainSlotAccess * _baseSlot() { return static_cast<MainSlotAccess*>(&mainSlot); }
		const MainSlotAccess * _baseSlot() const { return static_cast<const MainSlotAccess*>(&mainSlot); }


		Widget_wp			m_pBaseKeyFocus;

	};


} // namespace wg
#endif //WG_MODALLAYER_DOT_H
