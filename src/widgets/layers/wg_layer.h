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
#ifndef WG_LAYER_DOT_H
#define WG_LAYER_DOT_H
#pragma once

#include <wg_container.h>
#include <wg_cslot.h>


namespace wg
{

	class Layer;
	typedef	StrongPtr<Layer>	Layer_p;
	typedef	WeakPtr<Layer>		Layer_wp;


	//____ LayerSlot ___________________________________________________________

	class LayerSlot : public BasicSlot		/** @private */
	{
		friend class Layer;
	public:
		class Holder : public BasicSlot::Holder
		{
		};

	protected:
		LayerSlot(Holder * pHolder) : BasicSlot(pHolder) {}

		inline void setSize( SizeI size ) { geo.setSize(size); BasicSlot::_setSize(size); }
		inline void setGeo(RectI _geo) { geo = _geo; BasicSlot::_setSize(geo.size()); }

		RectI	geo;
	};



	/**
	 * @brief	Base class for containers that provides layers of different kinds.
	 *
	 * Layer is the base class for widget containers that are used as layers.
	 *
	 * The concept of Layers in WonderGUI is that a layer is a widget that provides
	 * a special purpose layer on top of the rest of the GUI hierarchy. The placement of
	 * this layer in the widget hierarchy decides on what layer things such as popups,
	 * modal dialog boxes, tooltips, notifications etc appear. (note: not all of these
	 * mentioned conceptual layers are implemented yet).
	 *
	 * For example, a Menubar widget that opens a menu will place the menu widget in the
	 * first PopupLayer it encounters while traversing it ancestors from itself upwards.
	 * In similar way, placing a widget in a ModalLayer will make it modal only against the
	 * children of that specific layer.
	 *
	 * All layers have a base child, which is the child through which the normal hierarchy continues.
	 * Children that resides within the layer itself are usually referred to as layer children.
	 *
	 **/

	class Layer : public Container
	{

	public:

		class CMainSlot : public CSlotImpl<BasicSlot>
		{
			friend class Layer;
		public:
			using CSlotImpl<BasicSlot>::operator=;
		protected:
			CMainSlot(SlotHolder * pHolder) : CSlotImpl(pHolder) {}
		};


		//.____ Components _______________________________________

		CMainSlot		mainSlot;


		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static Layer_p	cast( Object * pObject );

	protected:
		Layer(SlotHolder * pSlotHolder);

		// Overloaded from SlotHolder



		CoordI		_childPos( const BasicSlot * pSlot ) const override;

		void		_childRequestRender( BasicSlot * pSlot );
		void		_childRequestRender( BasicSlot * pSlot, const RectI& rect );
//		void		_childRequestResize( Slot * pSlot ) override;

		Widget *	_prevChild( const BasicSlot * pSlot ) const;
		Widget *	_nextChild( const BasicSlot * pSlot ) const;

		void		_releaseChild( BasicSlot * pSlot );
		void		_replaceChild(BasicSlot * pSlot, Widget * pNewWidget);

		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;


		// Overloaded from ChildHolder

		Object *	_object() override { return this; }

		// Overloaded from Widget

		void		_resize(const SizeI& size) override;
		void		_cloneContent( const Widget * _pOrg ) override;

		int			_matchingHeight(int width) const override;
		int			_matchingWidth(int height) const override;

		SizeI		_preferredSize() const override;


		//

		virtual	void	_onRequestRender( const RectI& rect, const LayerSlot * pSlot );	// rect is in our coordinate system.

		virtual const LayerSlot * _beginLayerSlots() const = 0;
		virtual const LayerSlot * _endLayerSlots() const = 0;
		virtual int			_sizeOfLayerSlot() const = 0;

		inline LayerSlot * _beginLayerSlots() { return const_cast<LayerSlot*>(const_cast<const Layer*>(this)->_beginLayerSlots()); }
		inline LayerSlot * _endLayerSlots() { return const_cast<LayerSlot*>(const_cast<const Layer*>(this)->_endLayerSlots()); }

		inline LayerSlot * _incLayerSlot( LayerSlot * pSlot, int sizeOf ) const { return (LayerSlot*) (((char*)pSlot)+sizeOf); }
		inline const LayerSlot * _incLayerSlot( const LayerSlot * pSlot, int sizeOf ) const { return (const LayerSlot*) (((char*)pSlot)+sizeOf); }

		inline LayerSlot * _decLayerSlot( LayerSlot * pSlot, int sizeOf ) const { return (LayerSlot*) (((char*)pSlot)-sizeOf); }
		inline const LayerSlot * _decLayerSlot( const LayerSlot * pSlot, int sizeOf ) const { return (const LayerSlot*) (((char*)pSlot)-sizeOf); }
	};



} // namespace wg
#endif //WG_LAYER_DOT_H
