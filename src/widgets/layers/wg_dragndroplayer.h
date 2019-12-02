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

#ifndef WG_DRAGNDROPLAYER_DOT_H
#define WG_DRAGNDROPLAYER_DOT_H
#pragma once

#include <wg_layer.h>
#include <wg_payload.h>
#include <wg_key.h>

namespace wg
{
	class   DragNDropLayer;
	typedef	StrongPtr<DragNDropLayer>	DragNDropLayer_p;
	typedef	WeakPtr<DragNDropLayer>		DragNDropLayer_wp;

	
	class DnDLayerSlot : public LayerSlot
	{
		friend class DragNDropLayer;
	protected:
		DnDLayerSlot(SlotHolder *pHolder) : LayerSlot(pHolder) {}
	};


	//____ DragNDropLayer ____________________________________________________________

	class DragNDropLayer : public Layer, protected LayerSlotHolder
	{
	public:

		//.____ Creation __________________________________________

		static DragNDropLayer_p  create() { return DragNDropLayer_p(new DragNDropLayer()); }

		//.____ Identification __________________________________________

		bool                    isInstanceOf( const char * pClassName ) const override;
		const char *            className( void ) const override;
		static const char       CLASSNAME[];
		static DragNDropLayer_p  cast( Object * pObject );

	protected:
		DragNDropLayer();
		virtual ~DragNDropLayer();
		virtual Widget* _newOfMyType() const override { return new DragNDropLayer(); };


//        DragNDropLayer *    _getDragNDropLayer() const { return const_cast<DragNDropLayer*>(this); }

		// Overloaded from Container

		Widget *        _findWidget( const CoordI& ofs, SearchMode mode ) override;

		// Overloaded from WidgetHolder

		Container *		_container() override { return this; }
		RootPanel *		_root() override { return Container::_root(); }
		Object *		_object() override { return this; }

		CoordI			_childPos(BasicSlot * pSlot) const { return Layer::_childPos(pSlot); }
		CoordI			_childGlobalPos(BasicSlot * pSlot) const override { return Layer::_childGlobalPos(pSlot); }
		bool			_isChildVisible(BasicSlot * pSlot) const override { return Layer::_isChildVisible(pSlot); }
		RectI			_childWindowSection(BasicSlot * pSlot) const override { return Layer::_childWindowSection(pSlot); }

		void			_childRequestRender(BasicSlot * pSlot) { return Layer::_childRequestRender(pSlot); }
		void			_childRequestRender(BasicSlot * pSlot, const RectI& rect) { return Layer::_childRequestRender(pSlot); }

		bool			_childRequestFocus(BasicSlot * pSlot, Widget * pWidget) override { return Layer::_childRequestFocus(pSlot, pWidget); }
		bool			_childReleaseFocus(BasicSlot * pSlot, Widget * pWidget) override { return Layer::_childReleaseFocus(pSlot, pWidget); }

		void			_childRequestInView(BasicSlot * pSlot) override { return Layer::_childRequestInView(pSlot); }
		void			_childRequestInView(BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Layer::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		Widget *		_prevChild(const BasicSlot * pSlot) const { return Layer::_prevChild(pSlot); }
		Widget *		_nextChild(const BasicSlot * pSlot) const { return Layer::_nextChild(pSlot); }

		void			_replaceChild(BasicSlot * pSlot, Widget * pNewChild) override { return Layer::_replaceChild(pSlot, pNewChild); }

		void            _childRequestResize(BasicSlot * pSlot) override;
		void			_releaseChild(BasicSlot * pSlot) override;

		// Overloaded from Layer

		const LayerSlot *	_beginLayerSlots() const override;
		const LayerSlot *	_endLayerSlots() const override;
		int					_sizeOfLayerSlot() const override;

		void				_onRequestRender(const RectI& rect, const LayerSlot * pSlot) override;    // rect is in our coordinate system.

		// Overloaded from Widget

		void            _cloneContent( const Widget * _pOrg ) override;
		void            _receive( Msg * pMsg ) override;
		void            _render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;

		// Internal

		void            _complete( Widget * pDeliveredTo, ModifierKeys modKeys, Coord pointerPos );
		void            _cancel( ModifierKeys modKeys, Coord pointerPos );
		void            _replaceDragWidget( Widget * pNewWidget );



		enum DragState
		{
			Idle,
			Picking,		// Mouse button pressed, awaiting drag to pass treshold
			Picked,			// Drag passed treshold, DropPickMsg sent.
			Dragging,		// We have a payload, a drag-widget and are dragging.
			Targeting,      // We are hovering a widget that has accepted our target probing.
			Delivering,     // We have released mouse button on a targeted widget. Deliver + Complete/Cancel cycle is taking place.
		};

		class BaseSlotAccess : public BaseSlot { friend class DragNDropLayer; };
		BaseSlotAccess * _baseSlot() { return static_cast<BaseSlotAccess*>(&m_baseSlot); }
		const BaseSlotAccess * _baseSlot() const { return static_cast<const BaseSlotAccess*>(&m_baseSlot); }


		RouteId        m_tickRouteId;

		DragState		m_dragState = DragState::Idle;

		DnDLayerSlot       m_dragSlot;            // Slot for widget being dragged, when it is dragged.

		Widget_p		m_pPicked;
		Payload_p		m_pPayload;

		int				m_dragStartTreshold = 3;
		CoordI           m_dragWidgetOfs;                // Drag widgets offset from pointer.

		Widget_wp       m_pProbed;                     // Last widget we sent a DropProbeMsg to. To avoid sending multiple messages in a row to same while hovering.
		Widget_wp       m_pTargeted;                   // Widget targeted while in state Targeting.
	};




} // namespace wg
#endif //WG_DRAGNDROPLAYER_DOT_H

