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



#include <wg_filler.h>
#include <wg_boxskin.h>

namespace wg
{
    using namespace Util;
	const char DragNDropLayer::CLASSNAME[] = {"DragNDropLayer"};

	//____ Constructor ____________________________________________________________

	DragNDropLayer::DragNDropLayer()
	{
	}

	//____ Destructor _____________________________________________________________

	DragNDropLayer::~DragNDropLayer()
	{
		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );
	}

	//____ isInstanceOf() _________________________________________________________

	bool DragNDropLayer::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Layer::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * DragNDropLayer::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	DragNDropLayer_p DragNDropLayer::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return DragNDropLayer_p( static_cast<DragNDropLayer*>(pObject) );

		return 0;
	}

	//____ _findWidget() __________________________________________________________

	Widget * DragNDropLayer::_findWidget( const CoordI& ofs, SearchMode mode )
	{
		// Widget being dragged is totally transparent to the mouse, so we just
		// forward to our child.

		if( m_baseSlot.pWidget )
		{
			if (m_baseSlot.pWidget->isContainer())
			{
				Widget * pRes = static_cast<Container*>(m_baseSlot.pWidget)->_findWidget(ofs, mode);
				if (pRes)
					return pRes;
			}
			else if (mode == SearchMode::Geometry || m_baseSlot.markTest(ofs))
				return m_baseSlot.pWidget;
		}
		else if( mode == SearchMode::Geometry )
			return this;

		return nullptr;
	}

	//____ _childRequestResize() ______________________________________________

	void DragNDropLayer::_childRequestResize(Slot * pSlot)
	{
		if( pSlot == &m_baseSlot )
			_requestResize();
		else
		{
			auto p = static_cast<LayerSlot*>(pSlot);
			p->setSize(p->preferredSize());

			//TODO: Should we request render (on both sizes) too?
		}
	}

	//____ _releaseChild() ____________________________________________________

	void DragNDropLayer::_releaseChild(Slot * pSlot)
	{
		if (pSlot == &m_baseSlot)
			Layer::_releaseChild(pSlot);
		else
			_replaceDragWidget(nullptr);
	}

	//____ _beginLayerSlots() _______________________________________________

	const LayerSlot * DragNDropLayer::_beginLayerSlots() const
	{
		return &m_dragSlot;
	}

	//____ _endLayerSlots() ________________________________________________

	const LayerSlot * DragNDropLayer::_endLayerSlots() const
	{
		if( m_dragSlot.pWidget )
			return (&m_dragSlot) + 1;
		else
			return &m_dragSlot;
	}

	//____ _sizeOfLayerSlot() _______________________________________________

	int DragNDropLayer::_sizeOfLayerSlot() const
	{
		return sizeof(LayerSlot);
	}

	//____ _onRequestRender() _______________________________________________

	void DragNDropLayer::_onRequestRender(const RectI& rect, const LayerSlot * pSlot)
	{
		// We don't mask against drag widget, it is assumed to be too small/transparent/irregular
		// for that to make sense.

		// Clip our geometry and pass it on

		_requestRender( RectI( rect, RectI(0,0,m_size) ));
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
			case MsgType::Tick:
			{
				break;
			}

			case MsgType::MouseDrag:
			{
				auto pMsg = static_cast<MouseDragMsg*>(_pMsg);
				if (pMsg->button() != MouseButton::Left)
					break;

				switch (m_dragState)
				{
					case DragState::Picking:
					{
						CoordI total = pMsg->draggedTotal();
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
						// Move the drag-widget onscreen.

						_requestRender(m_dragSlot.geo);
						m_dragSlot.geo.setPos( pMsg->pointerPos() + m_dragWidgetOfs );
						_requestRender(m_dragSlot.geo);

// MOVE TO TICK!						// Check if we entered/left a (possible) target.

						CoordI ofs = _toLocal(pMsg->pointerPos());

						Widget * pProbed = _findWidget(ofs, SearchMode::ActionTarget );

						while (pProbed && pProbed != this && !pProbed->isDropTarget())
							pProbed = pProbed->parent();

						if( pProbed && pProbed != this && pProbed != m_pProbed )
						{
							m_pProbed = pProbed;
							Base::msgRouter()->post(new DropProbeMsg(pProbed, m_pickCategory, m_pPayload, m_pPicked, this, pMsg->modKeys(), pMsg->pointerPos()));
						}

						break;
					}

					case DragState::Targeting:
					{
						// Move the drag-widget onscreen.

						_requestRender(m_dragSlot.geo);
						m_dragSlot.geo.setPos( pMsg->pointerPos() + m_dragWidgetOfs );
						_requestRender(m_dragSlot.geo);


// MOVE TO TICK!                        // Check if our target has changed

						CoordI ofs = _toLocal(pMsg->pointerPos());

						Widget * pHovered = _findWidget(ofs, SearchMode::ActionTarget );

						while (pHovered && pHovered != this && !pHovered->isDropTarget())
							pHovered = pHovered->parent();

						if( pHovered != m_pTargeted )
						{
							// Untarget previous target. Probing possibly new target we leave for next round.

							if( m_pTargeted )
								Base::msgRouter()->post(new DropLeaveMsg(m_pTargeted, m_pickCategory, m_pPayload, m_pPicked, pMsg->modKeys(), pMsg->pointerPos()));

							m_pTargeted = nullptr;
							m_dragState = DragState::Dragging;
						}

						// Send move messages to targeted widget

						if( m_pTargeted )                                   // Check our weak pointer just in case it has been deleted...
						{
							Base::msgRouter()->post(new DropMoveMsg(m_pTargeted, m_pickCategory, m_pPayload, m_pPicked, m_dragSlot.pWidget, this, pMsg->modKeys(), pMsg->pointerPos()));
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

				auto pSource = Widget::cast(pMsg->originalSource());

				while (pSource && pSource != this && !pSource->isPickable())
					pSource = pSource->parent();

				if (pSource && pSource != this && pSource->isPickable())
				{
					m_pPicked = pSource;
					m_pickCategory = pSource->pickCategory();
					m_tickRouteId = Base::msgRouter()->addRoute( MsgType::Tick, this );
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
							Base::msgRouter()->post(new DropDeliverMsg(m_pTargeted, m_pickCategory, m_pPayload, m_pPicked, this, pMsg->modKeys(), pMsg->pointerPos()));
							m_dragState = DragState::Delivering;
							Base::msgRouter()->post(new DropLeaveMsg(m_pTargeted, m_pickCategory, m_pPayload, m_pPicked, pMsg->modKeys(), pMsg->pointerPos()));
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

				if (pMsg->hasPayload() && m_dragState == DragState::Picked )
				{
					// Set payload

					m_pPayload = pMsg->payload();

					// Set/generate drag widget (widget actually dragged across the screen)

					auto pDragWidget = pMsg->dragWidget();
					SizeI    dragWidgetSize;

					if (pDragWidget)
					{
						pDragWidget->releaseFromParent();
						m_dragWidgetOfs = pMsg->dragWidgetPointerOfs();
						m_dragSlot.replaceWidget(this, pDragWidget);
						dragWidgetSize = m_dragSlot.preferredSize();
					}
					else
					{
						m_dragWidgetOfs = CoordI(0,0) - pMsg->pickOfs();
						dragWidgetSize = m_pPicked->size();                             //TODO: Possible source of error, if picked widget changes size before the render call.
					}

					CoordI mousePos = _toLocal(pMsg->pointerPos());
					m_dragSlot.geo = { mousePos + m_dragWidgetOfs, dragWidgetSize };

					_requestRender(m_dragSlot.geo);
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

					Base::msgRouter()->post(new DropEnterMsg(pTargeted, m_pickCategory, m_pPayload, m_pPicked, m_dragSlot.pWidget,  this, pMsg->modKeys(), pMsg->pointerPos()));

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

				if( pMsg->dragWidget() != m_dragSlot.pWidget )
					_replaceDragWidget(pMsg->dragWidget());

				break;
			}

			case MsgType::DropEnter:
			{
				auto pMsg = static_cast<DropEnterMsg*>(_pMsg);

				// Check if we need to change drag widget

				if( pMsg->dragWidget() != m_dragSlot.pWidget )
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

	void DragNDropLayer::_cancel( ModifierKeys modKeys, CoordI pointerPos )
	{
		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );

		if( m_dragSlot.pWidget )
		{
			_requestRender(m_dragSlot.geo);
			m_dragSlot.replaceWidget(this, nullptr);
		}

		if( m_pTargeted )
		{
			Base::msgRouter()->post(new DropLeaveMsg(m_pTargeted, m_pickCategory, m_pPayload, m_pPicked, modKeys, pointerPos));
			m_pTargeted = nullptr;
		}

		Base::msgRouter()->post(new DropCancelMsg(m_pPicked, m_pickCategory, nullptr, modKeys, pointerPos));
		m_pPicked = nullptr;
		m_pPayload = nullptr;
		m_dragState = DragState::Idle;
	}

	//____ _complete() _______________________________________________________________

	void DragNDropLayer::_complete( Widget * pDeliveredTo, ModifierKeys modKeys, CoordI pointerPos )
	{
		assert( !m_pTargeted );

		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );

		if( m_dragSlot.pWidget )
		{
			_requestRender(m_dragSlot.geo);
			m_dragSlot.replaceWidget(this, nullptr);
		}

		Base::msgRouter()->post(new DropCompleteMsg(m_pPicked, pDeliveredTo, m_pickCategory, m_pPayload, modKeys, pointerPos));
		m_pPicked = nullptr;
		m_pPayload = nullptr;
		m_dragState = DragState::Idle;
	}

	//____ _replaceDragWidget() ______________________________________________________

	void DragNDropLayer::_replaceDragWidget( Widget * pNewWidget )
	{

		if (pNewWidget)
			pNewWidget->releaseFromParent();
		m_dragSlot.replaceWidget(this, pNewWidget );

		SizeI newSize = pNewWidget ? m_dragSlot.preferredSize() : SizeI(0, 0);
		SizeI maxSize = SizeI::max(m_dragSlot.geo.size(), newSize);

		m_dragSlot.geo.setSize(newSize);

		_requestRender({ m_dragSlot.geo.pos(), maxSize });
	}


	//____ _render() __________________________________________________________

	void DragNDropLayer::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		// Generate drag widget as an image of picked widget if missing and needed./*/*

		if( m_dragState == DragState::Dragging && !m_dragSlot.pWidget )
		{
			SizeI sz = qpixToRaw(m_pPicked->size());

			auto pFactory = pDevice->surfaceFactory();
			auto pCanvas = pFactory->createSurface(rawToPixels(sz),PixelFormat::BGRA_8);
			pCanvas->fill( Color::Transparent );

            RectI noClip(rawToPixels(sz));

            auto pOldClip   = pDevice->clipList();
            int  nOldClip   = pDevice->clipListSize();
			auto pOldCanvas = pDevice->canvas();
			Color oldTint   = pDevice->tintColor();

            pDevice->setClipList(1, &noClip);
            pDevice->setCanvas(pCanvas);
			pDevice->setTintColor( {oldTint.r, oldTint.g, oldTint.b, (uint8_t)(oldTint.a*0.75f)});
			m_pPicked->_render(pDevice, sz, sz);
			pDevice->setCanvas(pOldCanvas);
			pDevice->setTintColor(oldTint);
            pDevice->setClipList(nOldClip, pOldClip);

			auto pImage = Image::create();
			pImage->setImage( pCanvas );

//            auto pImage = Filler::create();
//            pImage->setPreferredSize({16,16});
//            pImage->setSkin(BoxSkin::create( 1, Color::Red, Color::Black ));


			m_dragSlot.replaceWidget(this, pImage);
            m_dragSlot.setSize(sz);
		}

		Layer::_render(pDevice,_canvas,_window);
	}


} // namespace wg
