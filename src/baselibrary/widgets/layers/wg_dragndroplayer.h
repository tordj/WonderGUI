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
#include <wg_dataset.h>
#include <wg_key.h>

namespace wg
{

	class   DragNDropLayer;
	typedef	StrongPtr<DragNDropLayer>	DragNDropLayer_p;
	typedef	WeakPtr<DragNDropLayer>		DragNDropLayer_wp;

	//____ DragNDropLayer ____________________________________________________________

	class DragNDropLayer : public Layer
	{
	public:

		//____ Slot ___________________________________________________________

		class Slot : public Layer::Slot
		{
			friend class DragNDropLayer;
		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

		protected:

			Slot(SlotHolder * pHolder) : Layer::Slot(pHolder) {}
		};


		//.____ Creation __________________________________________

		static DragNDropLayer_p  create() { return DragNDropLayer_p(new DragNDropLayer()); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

	protected:
		DragNDropLayer();
		virtual ~DragNDropLayer();
		virtual Widget* _newOfMyType() const override { return new DragNDropLayer(); };


//        DragNDropLayer *    _getDragNDropLayer() const { return const_cast<DragNDropLayer*>(this); }

		// Overloaded from Widget

		void            _cloneContent(const Widget * _pOrg) override;
		void            _receive(Msg * pMsg) override;
		void            _render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *        _findWidget( const Coord& ofs, SearchMode mode ) override;

		void            _childRequestResize(StaticSlot * pSlot) override;
		void			_releaseChild(StaticSlot * pSlot) override;

		// Overloaded from Layer

		const Layer::Slot *	_beginLayerSlots() const override;
		const Layer::Slot *	_endLayerSlots() const override;
		int					_sizeOfLayerSlot() const override;

		void				_onRequestRender(const Rect& rect, const Layer::Slot * pSlot) override;    // rect is in our coordinate system.

		// Internal

		void            _complete( Widget * pDeliveredTo, ModifierKeys modKeys, Coord pointerPos );
		void            _cancel( ModifierKeys modKeys, Coord pointerPos );
		void            _replaceDragWidget( Widget * pNewWidget );



		enum DragState
		{
			Idle,
			Picking,		// Mouse button pressed, awaiting drag to pass treshold
			Picked,			// Drag passed treshold, DropPickMsg sent.
			Dragging,		// We have a dataset, a drag-widget and are dragging.
			Targeting,      // We are hovering a widget that has accepted our target probing.
			Delivering,     // We have released mouse button on a targeted widget. Deliver + Complete/Cancel cycle is taking place.
		};

		DragState		m_dragState = DragState::Idle;

		Slot			m_dragSlot;            // Slot for widget being dragged, when it is dragged.

		Widget_p		m_pPicked;
		BasicDataset_p	m_pDataset;

		MU				m_dragStartTreshold;
		Coord			m_dragWidgetOfs;                // Drag widgets offset from pointer.

		Widget_wp       m_pProbed;                     // Last widget we sent a DropProbeMsg to. To avoid sending multiple messages in a row to same while hovering.
		Widget_wp       m_pTargeted;                   // Widget targeted while in state Targeting.
	};




} // namespace wg
#endif //WG_DRAGNDROPLAYER_DOT_H

