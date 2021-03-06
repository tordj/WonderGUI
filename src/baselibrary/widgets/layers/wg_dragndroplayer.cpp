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

#include <wg_dragndroplayer.h>
#include <wg_base.h>
#include <wg_msgrouter.h>
#include <wg_msg.h>
#include <wg_gfxdevice.h>
#include <wg_surfacefactory.h>
#include <wg_patches.h>
#include <wg_image.h>
#include <wg_internal.h>



#include <wg_filler.h>
#include <wg_boxskin.h>

namespace wg
{
	using namespace Util;
	const TypeInfo DragNDropLayer::TYPEINFO = { "DragNDropLayer", &Layer::TYPEINFO };
	const TypeInfo DragNDropLayer::Slot::TYPEINFO = { "DragNDropLayer::Slot", &Layer::Slot::TYPEINFO };

	//____ constructor ____________________________________________________________

	DragNDropLayer::DragNDropLayer() : m_dragSlot(this)
	{
		m_dragStartTreshold = 3 * MU::qpixPerPoint();
	}

	//____ Destructor _____________________________________________________________

	DragNDropLayer::~DragNDropLayer()
	{
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& DragNDropLayer::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	DragNDropLayer::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		if (pSlot == &mainSlot)
			return DynamicSlot::TYPEINFO;

		return Slot::TYPEINFO;
	}

	//____ _findWidget() __________________________________________________________

	Widget * DragNDropLayer::_findWidget( const Coord& ofs, SearchMode mode )
	{
		// Widget being dragged is totally transparent to the mouse, so we just
		// forward to our child.

		if( mainSlot._widget() )
		{
			Widget * pWidget = mainSlot._widget();

			if (pWidget->isContainer())
			{
				Widget * pRes = static_cast<OContainer*>(pWidget)->_findWidget(ofs, mode);
				if (pRes)
					return pRes;
			}
			else if (mode == SearchMode::Geometry || mainSlot._widget()->markTest(ofs))
				return mainSlot._widget();
		}
		else if( mode == SearchMode::Geometry )
			return this;

		return nullptr;
	}

	//____ _childRequestResize() ______________________________________________

	void DragNDropLayer::_childRequestResize(StaticSlot * pSlot)
	{
		if( pSlot == &mainSlot )
			_requestResize();
		else
		{
			auto p = static_cast<Slot*>(pSlot);

			Size pref = p->_widget()->preferredSize();
			Size max = Size::max(pref, p->m_geo.size());

			_requestRender(Rect({ 0,0,m_size }, { p->m_geo.pos(), max }));
			p->_setSize(pref);
		}
	}

	//____ _releaseChild() ____________________________________________________

	void DragNDropLayer::_releaseChild(StaticSlot * pSlot)
	{
		if (pSlot == &mainSlot)
			Layer::_releaseChild(pSlot);
		else
			_replaceDragWidget(nullptr);
	}

	//____ _beginLayerSlots() _______________________________________________

	const Layer::Slot * DragNDropLayer::_beginLayerSlots() const
	{
		return &m_dragSlot;
	}

	//____ _endLayerSlots() ________________________________________________

	const Layer::Slot * DragNDropLayer::_endLayerSlots() const
	{
		if( m_dragSlot._widget() )
			return (&m_dragSlot) + 1;
		else
			return &m_dragSlot;
	}

	//____ _sizeOfLayerSlot() _______________________________________________

	int DragNDropLayer::_sizeOfLayerSlot() const
	{
		return sizeof(Slot);
	}

	//____ _onRequestRender() _______________________________________________

	void DragNDropLayer::_onRequestRender(const Rect& rect, const Layer::Slot * pSlot)
	{
		// We don't mask against drag widget, it is assumed to be too small/transparent/irregular
		// for that to make sense.

		// Clip our geometry and pass it on

		_requestRender( Rect( rect, Rect(0,0,m_size) ));
	}

	//____ _cloneContent() ____________________________________________________

	void DragNDropLayer::_cloneContent( const Widget * _pOrg )
	{

	}

	//____ _receive() __________________________________________________________

	void DragNDropLayer::_receive( Msg * _pMsg )
	{
		switch (_pMsg->type())
		{
			case MsgType::MouseDrag:
			{
				auto pMsg = static_cast<MouseDragMsg*>(_pMsg);
				if (pMsg->button() != MouseButton::Left)
					break;

				switch (m_dragState)
				{
					case DragState::Picking:
					{
						Coord total = pMsg->draggedTotal();
						if (abs(total.x) + abs(total.y) > m_dragStartTreshold)
						{
							Coord pickOfs = pMsg->startPos() - m_pPicked->globalPos();
							Base::msgRouter()->post(new DropPickMsg(m_pPicked, pickOfs, this, pMsg->modKeys(), pMsg->pointerPos()));
							m_dragState = DragState::Picked;
						}
						break;
					}

					case DragState::Dragging:
					{
						Coord pointerPos = pMsg->pointerPos();

						// Move the drag-widget onscreen.

						_requestRender(m_dragSlot.m_geo);
						m_dragSlot.m_geo.setPos( pointerPos + m_dragWidgetOfs );
						_requestRender(m_dragSlot.m_geo);

// MOVE TO TICK!						// Check if we entered/left a (possible) target.

						Coord ofs = toLocal(pointerPos);

						Widget * pProbed = _findWidget(ofs, SearchMode::ActionTarget );

						while (pProbed && pProbed != this && !pProbed->isDropTarget())
							pProbed = pProbed->parent();

						if( pProbed && pProbed != this && pProbed != m_pProbed )
						{
							m_pProbed = pProbed;
							Base::msgRouter()->post(new DropProbeMsg(pProbed, m_pickCategory, m_pDataset, m_pPicked, this, pMsg->modKeys(), pMsg->pointerPos()));
						}

						break;
					}

					case DragState::Targeting:
					{
						Coord pointerPos = pMsg->pointerPos();

						// Move the drag-widget onscreen.

						_requestRender(m_dragSlot.m_geo);
						m_dragSlot.m_geo.setPos( pointerPos + m_dragWidgetOfs );
						_requestRender(m_dragSlot.m_geo);


// MOVE TO TICK!                        // Check if our target has changed

						Coord ofs = toLocal(pointerPos);

						Widget * pHovered = _findWidget(ofs, SearchMode::ActionTarget );

						while (pHovered && pHovered != this && !pHovered->isDropTarget())
							pHovered = pHovered->parent();

						if( pHovered != m_pTargeted )
						{
							// Untarget previous target. Probing possibly new target we leave for next round.

							if( m_pTargeted )
								Base::msgRouter()->post(new DropLeaveMsg(m_pTargeted, m_pickCategory, m_pDataset, m_pPicked, pMsg->modKeys(), pMsg->pointerPos()));

							m_pTargeted = nullptr;
							m_dragState = DragState::Dragging;
						}

						// Send move messages to targeted widget

						if( m_pTargeted )                                   // Check our weak pointer just in case it has been deleted...
						{
							Base::msgRouter()->post(new DropMoveMsg(m_pTargeted, m_pickCategory, m_pDataset, m_pPicked, m_dragSlot._widget(), this, pMsg->modKeys(), pMsg->pointerPos()));
						}

						break;
					}

					default:
						break;
				}

				break;
			}

			case MsgType::MousePress:
			{
				auto pMsg = static_cast<MousePressMsg*>(_pMsg);
				if (pMsg->button() != MouseButton::Left || m_dragState != DragState::Idle || pMsg->sourceRawPtr() == nullptr )
					break;

				auto pSource = static_cast<Widget*>(pMsg->originalSource().rawPtr());

				while (pSource && pSource != this && !pSource->isPickable())
					pSource = pSource->parent();

				if (pSource && pSource != this && pSource->isPickable())
				{
					m_pPicked = pSource;
					m_pickCategory = pSource->pickCategory();
					m_dragState = DragState::Picking;
				}
				break;
			}

			case MsgType::MouseRelease:
			{
				auto pMsg = static_cast<MouseReleaseMsg*>(_pMsg);
				if (pMsg->button() != MouseButton::Left)
					break;

				switch (m_dragState)
				{
					case Picking:
					{
						m_pPicked = nullptr;
						m_dragState = DragState::Idle;
						break;
					}
					case Picked:
					{
						assert(false);		// There should be no mouse release msg before our DropPickMsg is returned.
					}
					case Dragging:
					{
						_cancel( pMsg->modKeys(), pMsg->pointerPos());
						break;
					}
					case Targeting:
					{
						if( m_pTargeted )
						{
							Base::msgRouter()->post(new DropDeliverMsg(m_pTargeted, m_pickCategory, m_pDataset, m_pPicked, this, pMsg->modKeys(), pMsg->pointerPos()));
							m_dragState = DragState::Delivering;
							Base::msgRouter()->post(new DropLeaveMsg(m_pTargeted, m_pickCategory, m_pDataset, m_pPicked, pMsg->modKeys(), pMsg->pointerPos()));
							m_pTargeted = nullptr;
						}
						else
							_cancel( pMsg->modKeys(), pMsg->pointerPos());
						break;
					}
					default:
						break;
				}
				break;
			}

			case MsgType::DropPick:
			{
				auto pMsg = static_cast<DropPickMsg*>(_pMsg);

				assert(m_dragState == DragState::Picked);

				if (pMsg->hasDataset() && m_dragState == DragState::Picked )
				{
					// Set dataset

					m_pDataset = pMsg->dataset();

					// Set/generate drag widget (widget actually dragged across the screen)

					Widget_p pDragWidget = pMsg->dragWidget();
					SizeI    dragWidgetSize;

					if (pDragWidget)
					{
						pDragWidget->releaseFromParent();
						m_dragWidgetOfs = pMsg->dragWidgetPointerOfs();
						m_dragSlot._setWidget(pDragWidget);
						dragWidgetSize = m_dragSlot._widget()->preferredSize();
					}
					else
					{
						m_dragWidgetOfs = Coord(0,0) - pMsg->pickOfs();
						dragWidgetSize = m_pPicked->size();                             //TODO: Possible source of error, if picked widget changes size before the render call.
					}

					Coord mousePos = toLocal(pMsg->pointerPos());
					m_dragSlot.m_geo = { Coord(mousePos + m_dragWidgetOfs).aligned(), dragWidgetSize };
					_requestRender(m_dragSlot.m_geo);
					m_dragState = DragState::Dragging;
				}
				else
					_cancel( pMsg->modKeys(), pMsg->pointerPos());

				break;
			}

			case MsgType::DropProbe:
			{
				auto pMsg = static_cast<DropProbeMsg*>(_pMsg);

				// Check if our probe was acccepted and in that case start targeting.

				if( pMsg->isAccepted() )
				{
					Widget * pTargeted = static_cast<Widget*>(pMsg->sourceRawPtr());

					Base::msgRouter()->post(new DropEnterMsg(pTargeted, m_pickCategory, m_pDataset, m_pPicked, m_dragSlot._widget(),  this, pMsg->modKeys(), pMsg->pointerPos()));

					m_pProbed = nullptr;
					m_pTargeted = pTargeted;
					m_dragState = DragState::Targeting;
				}

				break;
			}

			case MsgType::DropMove:
			{
				auto pMsg = static_cast<DropMoveMsg*>(_pMsg);

				// Check if we need to change drag widget

				if( pMsg->dragWidget() != m_dragSlot._widget())
					_replaceDragWidget(pMsg->dragWidget());

				break;
			}

			case MsgType::DropEnter:
			{
				auto pMsg = static_cast<DropEnterMsg*>(_pMsg);

				// Check if we need to change drag widget

				if( pMsg->dragWidget() != m_dragSlot._widget())
					_replaceDragWidget(pMsg->dragWidget());

				break;
			}

			case MsgType::DropDeliver:
			{
				auto pMsg = static_cast<DropDeliverMsg*>(_pMsg);

				// Check if our delivery was accepted and in that case complete, otherwise cancel.

				if( pMsg->isAccepted() )
					_complete( pMsg->deliveredTo(), pMsg->modKeys(), pMsg->pointerPos() );
				else
					_cancel(pMsg->modKeys(), pMsg->pointerPos());

				break;
			}


			default:
				break;
		}

		Layer::_receive(_pMsg);
	}

	//____ _cancel() ________________________________________________________________

	void DragNDropLayer::_cancel( ModifierKeys modKeys, Coord pointerPos )
	{
		if( m_dragSlot._widget())
		{
			_requestRender(m_dragSlot.m_geo);
			m_dragSlot._setWidget(nullptr);
		}

		if( m_pTargeted )
		{
			Base::msgRouter()->post(new DropLeaveMsg(m_pTargeted, m_pickCategory, m_pDataset, m_pPicked, modKeys, pointerPos));
			m_pTargeted = nullptr;
		}

		Base::msgRouter()->post(new DropCancelMsg(m_pPicked, m_pickCategory, nullptr, modKeys, pointerPos));
		m_pPicked = nullptr;
		m_pDataset = nullptr;
		m_dragState = DragState::Idle;
	}

	//____ _complete() _______________________________________________________________

	void DragNDropLayer::_complete( Widget * pDeliveredTo, ModifierKeys modKeys, Coord pointerPos )
	{
		assert( !m_pTargeted );

		if( m_dragSlot._widget())
		{
			_requestRender(m_dragSlot.m_geo);
			m_dragSlot._setWidget(nullptr);
		}

		Base::msgRouter()->post(new DropCompleteMsg(m_pPicked, pDeliveredTo, m_pickCategory, m_pDataset, modKeys, pointerPos));
		m_pPicked = nullptr;
		m_pDataset = nullptr;
		m_dragState = DragState::Idle;
	}

	//____ _replaceDragWidget() ______________________________________________________

	void DragNDropLayer::_replaceDragWidget( Widget * pNewWidget )
	{

		if (pNewWidget)
			pNewWidget->releaseFromParent();
		m_dragSlot._setWidget(pNewWidget );

		Size newSize = pNewWidget ? m_dragSlot._widget()->preferredSize() : Size();
		Size maxSize = Size::max(m_dragSlot.m_geo.size(), newSize);

		m_dragSlot.m_geo.setSize(newSize);

		_requestRender({ m_dragSlot.m_geo.pos(), maxSize });
	}


	//____ _render() __________________________________________________________

	void DragNDropLayer::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		// Generate drag widget as an image of picked widget if missing and needed./*/*

		if( m_dragState == DragState::Dragging && !m_dragSlot._widget())
		{
			Size sz = m_pPicked->size();

			auto pFactory = pDevice->surfaceFactory();
			auto pCanvas = pFactory->createSurface(sz.px(),PixelFormat::BGRA_8);
			pCanvas->fill( Color::Transparent );

			pDevice->beginCanvasUpdate(pCanvas);
			OO(m_pPicked)->_render(pDevice, sz, sz);
			pDevice->endCanvasUpdate();

			auto pImage = Image::create();
			pImage->setImage( pCanvas );

			//TODO: Allow for tinted image.

			m_dragSlot._setWidget(pImage);
			m_dragSlot._setSize(sz);
		}

		Layer::_render(pDevice,_canvas,_window);
	}


} // namespace wg
