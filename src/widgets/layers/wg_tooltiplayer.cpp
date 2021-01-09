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

#include <wg_tooltiplayer.h>
#include <wg_msg.h>
#include <wg_internal.h>
#include <wg_base.h>
#include <wg_inputhandler.h>
#include <wg_boxskin.h>
#include <wg_textdisplay.h>

namespace wg
{

	const TypeInfo TooltipLayer::TYPEINFO = { "TooltipLayer", &Layer::TYPEINFO };
	const TypeInfo TooltipLayer::Slot::TYPEINFO = { "TooltipLayer::Slot", &Layer::Slot::TYPEINFO };


	//____ constructor ____________________________________________________________

	TooltipLayer::TooltipLayer() : m_tooltipSlot(this)
	{
	}

	//____ Destructor _____________________________________________________________

	TooltipLayer::~TooltipLayer()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& TooltipLayer::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setTooltipGenerator() ______________________________________________

	void TooltipLayer::setTooltipGenerator(const std::function<Widget_p(Position& direction, const Widget*, const Border&)> func)
	{
		m_tooltipGenerator = func;
	}

	//____ setTooltipPosition() ______________________________________________

	void TooltipLayer::setTooltipPosition(Origo direction, Border spacing, bool bTooltipAroundPointer)
	{
		m_defaultPosition.direction = direction;
		m_defaultPosition.spacing = spacing;
		m_defaultPosition.bTooltipAroundPointer = bTooltipAroundPointer;
	}

	//____ setDisplayTooltips() _______________________________________________

	void TooltipLayer::setDisplayTooltips(bool bDisplay)
	{
		m_bDisplayTooltips = bDisplay;
		if (!m_bDisplayTooltips)
			_closeTooltip();
	}

	//____ _updateTooltipGeo() ________________________________________________

	void TooltipLayer::_updateTooltipGeo()
	{
		//TODO: Implement!
	}

	//____ _openTooltip() _____________________________________________________

	void TooltipLayer::_openTooltip()
	{
		if (!m_tooltipSlot.isEmpty() || !m_bDisplayTooltips )
			return;

		Widget* pHovered = _findWidget(m_hoverPos, SearchMode::ActionTarget);
		if (pHovered == nullptr || pHovered->tooltip().isEmpty() )
			return;

		Rect hoveredGeo = pHovered->globalGeo() - globalPos();
		Border widgetMargin{ hoveredGeo.y, m_size.w - hoveredGeo.right(), m_size.h - hoveredGeo.bottom(), hoveredGeo.x };

		m_activePosition = m_defaultPosition;
		Widget_p pWidget = m_tooltipGenerator(m_activePosition, pHovered, widgetMargin);

		if (pWidget)
		{
			m_tooltipSlot._setWidget(pWidget);

			Size tooltipSize = pWidget->preferredSize();

			Position& position = m_activePosition;

			Rect center = position.bTooltipAroundPointer ? Rect(m_hoverPos) : hoveredGeo;
			center += position.spacing;

			Coord tooltipPos;

			// Take care of vertical position

			if( position.direction == Origo::NorthWest || position.direction == Origo::North || position.direction == Origo::NorthEast )
			{
				if (center.y < tooltipSize.h)
					tooltipPos.y = center.y + center.h;
				else
					tooltipPos.y = center.y - tooltipSize.h;
			}
			else if (position.direction == Origo::SouthWest || position.direction == Origo::South || position.direction == Origo::SouthEast)
			{
				if (m_size.h - center.bottom() < tooltipSize.h)
					tooltipPos.y = center.y - tooltipSize.h;
				else
					tooltipPos.y = center.y + center.h;
			}
			else
			{
				tooltipPos.y = (center.y + center.h / 2) - tooltipSize.h / 2;
			}

			// Take care of horizontal position

			if (position.direction == Origo::NorthWest || position.direction == Origo::West || position.direction == Origo::SouthWest)
			{
				if (position.direction == Origo::West && center.x < tooltipSize.w)
					tooltipPos.x = center.x + center.w;
				else
					tooltipPos.x = center.x - tooltipSize.w;
			}
			else if (position.direction == Origo::NorthEast || position.direction == Origo::East || position.direction == Origo::SouthEast)
			{
				if (position.direction == Origo::East && m_size.w - center.right() < tooltipSize.w)
					tooltipPos.x = center.x - tooltipSize.w;
				else
					tooltipPos.x = center.x + center.w;
			}
			else
			{
				tooltipPos.x = (center.x + center.w / 2) - tooltipSize.w / 2;
			}

			// Make sure to stay inside screen (if possible).

			limit(tooltipPos.x, 0, m_size.w - tooltipSize.w);
			limit(tooltipPos.y, 0, m_size.h - tooltipSize.h);

			//

			m_tooltipSlot._setGeo({ tooltipPos, tooltipSize });
			_requestRender(m_tooltipSlot.m_geo);
		}
	}

	//____ _closeTooltip() ____________________________________________________

	void TooltipLayer::_closeTooltip()
	{
		if (!m_tooltipSlot.isEmpty())
		{
			if (!m_tooltipSlot.m_geo.isEmpty())
			{
				_requestRender(m_tooltipSlot.m_geo);
				m_tooltipSlot.m_geo.clear();
			}

			m_tooltipSlot._setWidget(nullptr);
			m_hoverCountdown = m_hoverMillisec;			
		}
	}


	//____ _receive() _________________________________________________________

	void TooltipLayer::_receive(Msg* _pMsg)
	{
		Layer::_receive(_pMsg);

		switch (_pMsg->type())
		{
		case MsgType::MouseEnter:
			_startReceiveUpdates();
			// No break, fall through to next case on purpose.
		case MsgType::MouseMove:
		{
			Coord mousePos = static_cast<InputMsg*>(_pMsg)->pointerPos() - globalPos();
			Widget* pHovered = _findWidget(mousePos, SearchMode::ActionTarget);

			if (pHovered->tooltip().isEmpty())
				pHovered = nullptr;

			if (!m_tooltipSlot.isEmpty())
			{
				if (pHovered != m_pHoverWidget)
				{
					_closeTooltip();

					m_pHoverWidget = pHovered;
					m_hoverPos = mousePos;
					m_hoverCountdown = m_hoverMillisec;
				}
				else if (abs(mousePos.x - m_hoverPos.x) > m_openTooltipMaxMovement || abs(mousePos.y - m_hoverPos.y) > m_openTooltipMaxMovement)
				{
					_closeTooltip();
					m_bBlockReopen = true;
				}
			}
			else
			{
				if (pHovered != m_pHoverWidget)
				{
					m_bBlockReopen = false;
					m_pHoverWidget = pHovered;
				}

				m_hoverPos = mousePos;
				m_hoverCountdown = m_hoverMillisec;
			}
			break;
		}
		case MsgType::MouseLeave:
			_stopReceiveUpdates();
			m_hoverPos = { -1,-1 };
			break;

		default:
			break;
		}
	}

	//____ _update() __________________________________________________________

	void TooltipLayer::_update(int microPassed, int64_t microsecTimestamp)
	{
		if (Base::inputHandler()->isAnyButtonPressed())
			return;

		if (m_hoverCountdown > 0 && m_pHoverWidget && !m_bBlockReopen)
		{
			m_hoverCountdown -= microPassed / 1000;
			if (m_hoverCountdown <= 0)
			{
				m_hoverCountdown = 0;
				_openTooltip();
			}
		}


	}

	//____ _slotTypeInfo() ____________________________________________________

	const TypeInfo& TooltipLayer::_slotTypeInfo(const StaticSlot* pSlot) const
	{
		if (pSlot == &mainSlot)
			return DynamicSlot::TYPEINFO;

		return Slot::TYPEINFO;
	}

	//____ _findWidget() ______________________________________________________

	Widget* TooltipLayer::_findWidget(const Coord& ofs, SearchMode mode)
	{
		if (!Rect(m_size).contains(ofs))
			return nullptr;

		if (mode == SearchMode::ActionTarget)
		{
			if (!mainSlot.isEmpty())
			{
				//TODO: Take padding into account

				Widget* pChild = OO(mainSlot)._widget();			
				if (pChild->isContainer())
					return OO(static_cast<Container*>(pChild))->_findWidget(ofs, mode);
				else if (pChild->markTest(ofs))
					return pChild;
			}
			return nullptr;
		}
		else
			return Layer::_findWidget(ofs, mode);
	}

	//____ _childRequestResize() ______________________________________________

	void TooltipLayer::_childRequestResize(StaticSlot* pSlot)
	{
		if (pSlot == &mainSlot)
			_requestResize();
		else
			_updateTooltipGeo();
	}

	//____ _beginLayerSlots() _________________________________________________

	const Layer::Slot* TooltipLayer::_beginLayerSlots() const
	{
		return &m_tooltipSlot;
	}

	//____ _endLayerSlots() ______________________________________________________

	const Layer::Slot* TooltipLayer::_endLayerSlots() const
	{
		if (m_tooltipSlot.isEmpty())
			return &m_tooltipSlot;

		return &m_tooltipSlot + 1;
	}

	//____ _sizeOfLayerSlot() ____________________________________________________

	int	TooltipLayer::_sizeOfLayerSlot() const
	{
		return sizeof(Slot);
	}

	//____ _defaultTooltipGenerator() ____________________________________________

	Widget_p TooltipLayer::_defaultTooltipGenerator(Position& position, const Widget* pHovered, const Border& margin)
	{
		BoxSkin_p pSkin = BoxSkin::create(1, Color::LightYellow, Color::Black);
		pSkin->setContentPadding(2);
		auto pWidget = TextDisplay::create();
		pWidget->skin = pSkin;

		pWidget->text.set(pHovered->tooltip());
		return pWidget;
	}
}
