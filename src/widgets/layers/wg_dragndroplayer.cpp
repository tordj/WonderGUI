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



#include <wg_filler.h>
#include <wg_boxskin.h>

namespace wg
{
	const char DragNDropLayer::CLASSNAME[] = {"DragNDropLayer"};

	//____ Constructor ____________________________________________________________

	DragNDropLayer::DragNDropLayer()
	{
	}

	//____ Destructor _____________________________________________________________

	DragNDropLayer::~DragNDropLayer()
	{
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

	Widget * DragNDropLayer::_findWidget( const Coord& ofs, SearchMode mode )
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
			else if (mode == SearchMode::Geometry || m_baseSlot.pWidget->markTest(ofs))
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
			p->geo.setSize(p->pWidget->preferredSize());
			p->pWidget->_setSize(p->geo);

			//TODO: Should we request render (on both sizes) too?
		}
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

	void DragNDropLayer::_onRequestRender(const Rect& rect, const LayerSlot * pSlot)
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
		if (_pMsg->type() != MsgType::MouseMove)
			int x = 0;

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
						if (total.x + total.y > m_dragStartTreshold)
						{
							Base::msgRouter()->post(new DropPickMsg(m_pPicked, this, pMsg->modKeys(), pMsg->pointerPos()));
							m_dragState = DragState::Picked;
						}
						break;
					}

					case DragState::Dragging:
					{
						// Move the drag-widget onscreen.

						_childRequestRender(&m_dragSlot);
						m_dragSlot.geo += pMsg->draggedNow();
						_childRequestRender(&m_dragSlot);

						// Check if we entered/left a (possible) target.


						break;
					}
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
					m_dragState = DragState::Picking;
				}
				break;
			}

			case MsgType::MouseRelease:
			{
				auto pMsg = static_cast<MouseReleaseMsg*>(_pMsg);
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
						_requestRender(m_dragSlot.geo);
						m_dragSlot.replaceWidget(this, nullptr);

						m_pPicked = nullptr;
						m_pPayload = nullptr;
						m_dragState = DragState::Idle;
						Base::msgRouter()->post(new DropCancelMsg(m_pPicked, m_pickCategory, nullptr, pMsg->modKeys(), pMsg->pointerPos()));
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
					m_pPayload = pMsg->payload();

					auto pDragWidget = pMsg->dragWidget();
					if (!pDragWidget)
					{
						auto p = Filler::create();
						p->setPreferredSize({ 16, 16 });
						p->setSkin(BoxSkin::create( 1, Color::Navy, Color::White));

						pDragWidget = p;

						//TODO: Insert our own drag widget.
					}

					m_dragSlot.replaceWidget(this, pDragWidget);

					Coord mousePos = toLocal(pMsg->pointerPos());

					m_dragSlot.geo = { mousePos + pMsg->dragWidgetPointerOfs(), pDragWidget->preferredSize() };

					_childRequestRender(&m_dragSlot);
					m_dragState = DragState::Dragging;
				}
				else
				{
					Base::msgRouter()->post(new DropCancelMsg(m_pPicked, m_pickCategory, nullptr, pMsg->modKeys(), pMsg->pointerPos()));
					m_pPicked = nullptr;
					m_pPayload = nullptr;
					m_dragState = DragState::Idle;
				}
				break;
			}


			default:
				break;
		}


		Layer::_receive(_pMsg);
	}



} // namespace wg
