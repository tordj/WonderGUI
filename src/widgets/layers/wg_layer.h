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
#include <wg_cstandardslot.h>


namespace wg
{

	class Layer;
	typedef	StrongPtr<Layer>	Layer_p;
	typedef	WeakPtr<Layer>		Layer_wp;


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

		//____ Slot ___________________________________________________________

		class Slot : public StaticSlot
		{
			friend class Layer;

		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Geometry _________________________________________________

			inline Coord	pos() const { return Util::qpixToMU(m_geo.pos()); }
			inline Size		size() const { return Util::qpixToMU(m_geo.size()); }
			inline Rect		geo() const { return Util::qpixToMU(m_geo); }

		protected:
			Slot(SlotHolder * pHolder) : StaticSlot(pHolder) {}

			inline void _setSize(SizeI size) { m_geo.setSize(size); StaticSlot::_setSize(size); }
			inline void _setGeo(RectI geo) { m_geo = geo; StaticSlot::_setSize(geo.size()); }

			RectI	m_geo;
		};


		//.____ Components _______________________________________

		CStandardSlot		mainSlot;


		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

	protected:
		Layer();

		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo(SlotWithGeo& package) const override;
		void		_nextSlotWithGeo(SlotWithGeo& package) const override;

		CoordI		_childPos( const StaticSlot * pSlot ) const override;

		void		_childRequestRender( StaticSlot * pSlot ) override;
		void		_childRequestRender( StaticSlot * pSlot, const RectI& rect ) override;
//		void		_childRequestResize( StaticSlot * pSlot ) override;

		Widget *	_prevChild( const StaticSlot * pSlot ) const override;
		Widget *	_nextChild( const StaticSlot * pSlot ) const override;

		void		_releaseChild( StaticSlot * pSlot ) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewWidget) override;

		// Overloaded from Widget

		void		_resize(const SizeI& size) override;
		void		_cloneContent( const Widget * _pOrg ) override;

		int			_matchingHeight(int width) const override;
		int			_matchingWidth(int height) const override;

		SizeI		_preferredSize() const override;


		//

		virtual	void	_onRequestRender( const RectI& rect, const Slot * pSlot );	// rect is in our coordinate system.

		virtual const Slot * _beginLayerSlots() const = 0;
		virtual const Slot * _endLayerSlots() const = 0;
		virtual int			_sizeOfLayerSlot() const = 0;

		inline Slot * _beginLayerSlots() { return const_cast<Slot*>(const_cast<const Layer*>(this)->_beginLayerSlots()); }
		inline Slot * _endLayerSlots() { return const_cast<Slot*>(const_cast<const Layer*>(this)->_endLayerSlots()); }

		inline Slot * _incLayerSlot( Slot * pSlot, int sizeOf ) const { return (Slot*) (((char*)pSlot)+sizeOf); }
		inline const Slot * _incLayerSlot( const Slot * pSlot, int sizeOf ) const { return (const Slot*) (((char*)pSlot)+sizeOf); }

		inline Slot * _decLayerSlot( Slot * pSlot, int sizeOf ) const { return (Slot*) (((char*)pSlot)-sizeOf); }
		inline const Slot * _decLayerSlot( const Slot * pSlot, int sizeOf ) const { return (const Slot*) (((char*)pSlot)-sizeOf); }

	};



} // namespace wg
#endif //WG_LAYER_DOT_H
