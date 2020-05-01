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

#include <wg_knob.h>
#include <wg_gfxdevice.h>
#include <wg_msg.h>

namespace wg
{
	using namespace Util;

	const TypeInfo Knob::TYPEINFO = { "Knob", &Widget::TYPEINFO };

	//____ constructor ____________________________________________________________

	Knob::Knob()
	{
	}

	//____ Destructor _____________________________________________________________

	Knob::~Knob()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Knob::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ preferredSize() __________________________________________________________

	Size Knob::preferredSize() const
	{
		if (m_pSkin)
			return m_pSkin->preferredSize();
		else
			return Size();
	}

	//____ setSteps() ________________________________________________________

	void Knob::setSteps(int nbSteps)
	{
		if (nbSteps < 0 )
		{
			//TODO: Error handling!
			return;
		}

		if (nbSteps != m_nbSteps)
		{
			m_nbSteps = nbSteps;
			if( nbSteps > 0 )
				setValue(m_value);		// Have the value aligned to the steps.
		}
	}

	//____ setDragAxis() _____________________________________________________

	void Knob::setDragAxis(Axis axis)
	{
		m_dragAxis = axis;
	}

	//____ setDragRange() ____________________________________________________

	void Knob::setDragRange(MU length)
	{
		if (length <= 0)
		{
			//TODO: Error handling
		}

		m_dragRange = length;
	}

	//____ setWheelStepSize() ______________________________________________________

	void Knob::setWheelStepSize(float stepSize)
	{
		m_wheelStepSize = stepSize;
	}

	//____ setValue() _________________________________________________________

	void Knob::setValue(float value)
	{
		if (value != m_value)
			_setValue(value);
	}

	//____ receive() __________________________________________________________

	void Knob::receive(Msg * _pMsg)
	{
		switch (_pMsg->type())
		{
			case MsgType::WheelRoll:
			{
				auto pMsg = static_cast<WheelRollMsg*>(_pMsg);

				if (pMsg->wheel() != 1)
					break;

				float value;
				if (m_wheelStepSize != 0.f)
					value = m_value + float(pMsg->distance().y) * m_wheelStepSize;
				else if (m_nbSteps > 0)
					value = m_value + float(pMsg->distance().y) / m_nbSteps;
				else
					value = m_value + float(pMsg->distance().y) / 20.f;				// Default increase step is 5%.

				_setValue(value);

				_pMsg->swallow();
				break;
			}

			case MsgType::MousePress:
			{
				auto pMsg = static_cast<MousePressMsg*>(_pMsg);

				if (pMsg->button() != MouseButton::Left)
					break;

				m_valueAtPress = m_value;
				m_dragged = 0;
				pMsg->swallow();
				break;
			}

			case MsgType::MouseRelease:
			{
				auto pMsg = static_cast<MousePressMsg*>(_pMsg);

				if (pMsg->button() != MouseButton::Left)
					break;

				pMsg->swallow();
				break;
			}

			case MsgType::MouseDrag:
			{
				auto pMsg = static_cast<MouseDragMsg*>(_pMsg);

				if (pMsg->button() != MouseButton::Left)
					break;

				m_dragged = m_dragAxis == Axis::Y ? m_dragged - pMsg->draggedNow().y : m_dragged + pMsg->draggedNow().x;

				float newValue = m_valueAtPress + m_dragged / m_dragRange;

				if (newValue > 1.f)
					m_dragged = (1.f - m_valueAtPress) * m_dragRange;

				if (newValue < 0.f)
					m_dragged = -m_valueAtPress * m_dragRange;
	
				_setValue(newValue);
				pMsg->swallow();
				break;
			}

			case MsgType::MouseClick:
			case MsgType::MouseDoubleClick:
			case MsgType::MouseRepeat:
				if (static_cast<MouseButtonMsg*>(_pMsg)->button() == MouseButton::Left)
					_pMsg->swallow();
				break;
		}

		Widget::receive(_pMsg);
	}

	//____ _cloneContent() _______________________________________________________

	void Knob::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		Knob * pOrg = (Knob*) _pOrg;
	}

	//____ _render() __________________________________________________________

	void Knob::_render(GfxDevice * pDevice, const Rect& canvas, const Rect& window)
	{
		if (m_pSkin)
			m_pSkin->render(pDevice, canvas, m_state, m_value);
	}

	//____ _setValue() __________________________________________________________

	void Knob::_setValue(float value)
	{
		limit(value, 0.f, 1.f);

		if (m_nbSteps > 0)
		{
			int val = int(value * m_nbSteps + 0.5f);
			value = 1.f / val;
		}

		if (value != m_value)
		{
			if (m_pSkin)
				_requestRender(m_pSkin->fractionChangeRect(m_size, m_state, m_value, value));
			m_value = value;
		}
	}

} // namespace wg
