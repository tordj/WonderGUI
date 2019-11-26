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
#include <wg_islotarray.h>

namespace wg
{

	class ModalLayer;
	typedef	StrongPtr<ModalLayer>	ModalLayer_p;
	typedef	WeakPtr<ModalLayer>		ModalLayer_wp;

	//____ ModalSlot ___________________________________________________________

	class ModalSlot : public LayerSlot		/** @private */
	{
		friend class ModalLayer;
		friend class IModalSlots;
		template<class S> friend class ISlotArray;
		template<class S> friend class SlotArray;

	protected:
		ModalSlot() : origo(Origo::NorthWest) {}

		const static bool safe_to_relocate = false;

		bool		bVisible = true;
		Origo		origo;
		RectI		placementGeo;		// Widgets geo relative anchor and hotspot. Setting width and height to 0 uses Widgets preferredSize() dynamically.
										// Setting just one of them to 0 uses Widgets matchingHeight() or matchingWidth() dynamically.
		Widget_wp	pKeyFocus;			// Pointer at child that held focus when this modal was last on top.

	};



	//____ ModalSlotsHolder _________________________________________________

	class ModalSlotsHolder : public SlotArrayHolder		/** @private */
	{
	public:
		virtual void	_refreshRealGeo(ModalSlot * pSlot, bool bForceResize = false) = 0;
	};

	class IModalSlots;
	typedef	StrongInterfacePtr<IModalSlots>	IModalSlots_p;
	typedef	WeakInterfacePtr<IModalSlots>	IModalSlots_wp;

	//____ IModalSlots ________________________________________________________

	class IModalSlots : public ISlotArray<ModalSlot>
	{
	public:
		/** @private */

		IModalSlots( SlotArray<ModalSlot> * pSlotArray, ModalSlotsHolder * pHolder ) : ISlotArray<ModalSlot>(pSlotArray,pHolder) {}

		//.____ Misc __________________________________________________________

		inline IModalSlots_p	ptr() { return IModalSlots_p(this); }

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

		const ModalSlotsHolder *	_holder() const { return static_cast<ModalSlotsHolder*>(m_pHolder); }
		ModalSlotsHolder *	_holder() { return static_cast<ModalSlotsHolder*>(m_pHolder); }


	};


	//____ ModalLayer __________________________________________________________

	class ModalLayer : public Layer, protected ModalSlotsHolder
	{
		friend class IModalSlots;

	public:

		//.____ Creation __________________________________________

		static ModalLayer_p	create() { return ModalLayer_p(new ModalLayer()); }

		//.____ Interfaces _______________________________________

		IModalSlots	modals;

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

		void			_childRequestResize( Slot * pSlot ) override;
		void			_releaseChild(Slot * pSlot) override;

		// Methods for ModalSlotsHolder

		void			_didAddSlots(Slot * pSlot, int nb) override;
		void			_didMoveSlots(Slot * pFrom, Slot * pTo, int nb) override;
		void			_willRemoveSlots(Slot * pSlot, int nb) override;
		void			_hideSlots(Slot *, int nb) override;
		void			_unhideSlots(Slot *, int nb) override;
		Object *		_object() override { return this;  }
		WidgetHolder *	_widgetHolder() override { return this; }
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

		class BaseSlotAccess : public BaseSlot { friend class ModalLayer; };
		BaseSlotAccess * _baseSlot() { return static_cast<BaseSlotAccess*>(&m_baseSlot); }
		const BaseSlotAccess * _baseSlot() const { return static_cast<const BaseSlotAccess*>(&m_baseSlot); }


		Widget_wp			m_pBaseKeyFocus;

		SlotArray<ModalSlot>	m_modals;

	};


} // namespace wg
#endif //WG_MODALLAYER_DOT_H
